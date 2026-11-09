/*
 * TOETSENOVERZICHT
 * ----------------
 * Camera (zonder Shift):
 *   W / S            vooruit / achteruit
 *   A / D            links / rechts
 *   Q / E            omhoog / omlaag
 *   Pijltjes         yaw (links/rechts) en pitch (omhoog/omlaag)
 *   (snelheden zijn per seconde, dus onafhankelijk van de framerate)
 *
 * Lichtbron (met Shift ingedrukt; de camera reageert dan niet):
 *   Shift + pijltjes links/rechts   licht in x
 *   Shift + pijltjes omhoog/omlaag  licht in y
 *   Shift + W / S                   licht in -z / +z
 *
 * Projectie:
 *   1  orthografisch
 *   2  parallel (oblique, cabinet)
 *   3  isometrisch
 *   4  perspectief (standaard)
 *
 * Efficientie en meting:
 *   P  animatie pauzeren / hervatten (bij pauze verandert de rotatie niet,
 *      dus de modelmatrix-cache wordt volledig benut)
 *   B  wisselen tussen GEOPTIMALISEERD (viewProjection 1x per frame +
 *      modelcache) en NAIEF (alles per shape opnieuw berekend)
 *   Na het starten en na elke druk op P of B meet het programma 1000
 *   frames en print dan eenmalig de gemiddelde frametijd in milliseconden.
 */

#include "engine.hpp"
#include <stdexcept>
#include <cmath>
#include <cstdio>

World::World(int breedte, int hoogte, const std::string &titel)
    : venster(nullptr), breedte(breedte), hoogte(hoogte),
      camera(Vector({0, 0, 0}), 1.0f, (float)breedte / (float)hoogte, 0.1f, 100.0f),
      lichtbron(Vector({0.2f, 1.2f, 0.0f}), 1.0f, 1.0f, 1.0f){

    if (!glfwInit()) {
        throw std::runtime_error("GLFW initialisatie mislukt.");
    }

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    venster = glfwCreateWindow(breedte, hoogte, titel.c_str(), NULL, NULL);
    if (!venster) {
        glfwTerminate();
        throw std::runtime_error("Venster aanmaken mislukt.");
    }

    glfwMakeContextCurrent(venster);

    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        throw std::runtime_error("GLEW initialisatie mislukt.");
    }

    // Vsync uit, anders meet je alleen de refreshrate van je scherm.
    glfwSwapInterval(0);

    // --- Shaders ---
    const std::string vertexBron =
        R"(#version 330 core
        in vec2 uv;
        in vec3 pos;
        in vec3 normaal;
        uniform mat4 viewProjection;
        uniform mat4 model;
        out vec2 fragUV;
        out vec3 fragNormaal;
        out vec3 fragPos;
        out vec3 fragObjectPos;
        void main() {
            fragUV = uv;
            fragNormaal = mat3(model) * normaal;
            vec4 wereld = model * vec4(pos, 1.0);
            fragPos = wereld.xyz;
            fragObjectPos = pos;
            gl_Position = viewProjection * wereld;
        })";

    const std::string fragmentBron =
        R"(#version 330 core
        in vec2 fragUV;
        in vec3 fragNormaal;
        in vec3 fragPos;
        in vec3 fragObjectPos;
        out vec4 fragColor;

        uniform sampler2D tex;
        uniform bool gebruiktTextuur;
        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform vec3 eyePos;
        uniform vec3 kleur;

        struct Material {
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            float shininess;
        };
        uniform Material material;
        uniform float alpha;

        void main() {
            vec3 n = normalize(fragNormaal);

            float freq = 8.0;
            vec3 perturbatie = vec3(
                sin(fragObjectPos.x * freq),
                sin(fragObjectPos.y * freq),
                sin(fragObjectPos.z * freq)
            ) * 0.15;

            n = normalize(n + perturbatie);

            vec3 naarLicht = normalize(lightPos - fragPos);

            vec3 basiskleur = (gebruiktTextuur ? texture(tex, fragUV).rgb : vec3(1.0)) * kleur;

            vec3 ambient = lightColor * material.ambient * basiskleur;

            float diffuseFactor = max(0.0, dot(n, naarLicht));
            vec3 diffuse = diffuseFactor * lightColor * material.diffuse * basiskleur;

            vec3 reflectie = reflect(-naarLicht, n);
            vec3 naarOog = normalize(eyePos - fragPos);
            float specularFactor = pow(max(0.0, dot(naarOog, reflectie)), material.shininess);
            vec3 specular = specularFactor * lightColor * material.specular;

            fragColor = vec4(ambient + diffuse + specular, alpha);
        })";

    Shader vertex(GL_VERTEX_SHADER, vertexBron);
    vertex.compile();
    Shader fragment(GL_FRAGMENT_SHADER, fragmentBron);
    fragment.compile();
    Program prog(vertex, fragment);
    programma = prog.release();
    glUseProgram(programma);

    glUseProgram(programma);
    glUniform1i(glGetUniformLocation(programma, "tex"), 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void World::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programma);

    // Blending blijft aan (de constructor zet het aan); het transparante
    // materiaal wordt zo echt gemengd met wat erachter staat.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Lichtpositie, lichtkleur en oogpositie komen uit de objecten.
    Vector lp = lichtbron.getPositie();
    glUniform3f(glGetUniformLocation(programma, "lightPos"), lp.at(0), lp.at(1), lp.at(2));
    glUniform3f(glGetUniformLocation(programma, "lightColor"),
                lichtbron.getKleur(0), lichtbron.getKleur(1), lichtbron.getKleur(2));
    Vector oogW = camera.oogWereld();
    glUniform3f(glGetUniformLocation(programma, "eyePos"), oogW.at(0), oogW.at(1), oogW.at(2));

    // Geoptimaliseerd: projectie * view een keer per frame berekenen.
    // (In naieve modus gebeurt dit per shape in tekenShape().)
    if (geoptimaliseerd) {
        Matrix4 vp = camera.compleet();
        GLfloat glVP[16];
        vp.naarGL(glVP);
        glUniformMatrix4fv(glGetUniformLocation(programma, "viewProjection"), 1, GL_FALSE, glVP);
    }

    // Animatietijd loopt alleen door als de animatie niet gepauzeerd is.
    double nu = glfwGetTime();
    if (!gepauzeerd) animTijd += nu - laatsteTijd;
    laatsteTijd = nu;
    float t = (float)animTijd;

    // Vormen met een rotatiesnelheid draaien vanzelf. Alleen de rotatie
    // verandert; positie en schaal blijven staan. Bij pauze zijn de
    // waarden gelijk, dus Transform verhoogt zijn versie niet en blijft
    // de modelmatrix uit de cache komen.
    for (size_t i = 0; i < shapes.size(); i++) {
        if (shapes[i]->isGeanimeerd()) {
            shapes[i]->getTransform().setRotatie(
                shapes[i]->getRotatieSnelheid(0) * t,
                shapes[i]->getRotatieSnelheid(1) * t,
                shapes[i]->getRotatieSnelheid(2) * t);
        }
    }

    // Pass 1: ondoorzichtige objecten (schrijven diepte).
    for (size_t i = 0; i < shapes.size(); i++) {
        if (shapes[i]->getMateriaal().alpha >= 1.0f) tekenShape(i);
    }

    // Pass 2: transparante objecten, zonder diepte te schrijven.
    glDepthMask(GL_FALSE);
    for (size_t i = 0; i < shapes.size(); i++) {
        if (shapes[i]->getMateriaal().alpha < 1.0f) tekenShape(i);
    }
    glDepthMask(GL_TRUE);
}

void World::tekenShape(size_t i) {
    const Materiaal &m = shapes[i]->getMateriaal();
    glUniform3fv(glGetUniformLocation(programma, "material.ambient"), 1, m.ambient);
    glUniform3fv(glGetUniformLocation(programma, "material.diffuse"), 1, m.diffuse);
    glUniform3fv(glGetUniformLocation(programma, "material.specular"), 1, m.specular);
    glUniform1f(glGetUniformLocation(programma, "material.shininess"), m.shininess);
    glUniform1f(glGetUniformLocation(programma, "alpha"), m.alpha);
    glUniform3fv(glGetUniformLocation(programma, "kleur"), 1, m.kleur);

    GLfloat glModel[16];
    if (geoptimaliseerd) {
        // Gecachete modelmatrix (alleen herberekend bij gewijzigde Transform).
        shapes[i]->getModel().naarGL(glModel);
    } else {
        // Naief: viewProjection en model per shape volledig opnieuw berekenen.
        Matrix4 vp = camera.compleet();
        GLfloat glVP[16];
        vp.naarGL(glVP);
        glUniformMatrix4fv(glGetUniformLocation(programma, "viewProjection"), 1, GL_FALSE, glVP);

        Matrix4 model = shapes[i]->getModelOngecachet();
        model.naarGL(glModel);
    }
    glUniformMatrix4fv(glGetUniformLocation(programma, "model"), 1, GL_FALSE, glModel);

    glUniform1i(glGetUniformLocation(programma, "gebruiktTextuur"), m.gebruiktTextuur ? 1 : 0);
    if (m.gebruiktTextuur) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m.textuur);
    }

    shapes[i]->draw();
}

bool World::toetsPress(int toets) {
    bool nu = glfwGetKey(venster, toets) == GLFW_PRESS;
    bool event = nu && !toetsVorig[toets];
    toetsVorig[toets] = nu;
    return event;
}

void World::resetMeting() {
    meetFrames = 0;
    gemeten = false;
    meetStart = glfwGetTime();
}

void World::meetFrame() {
    if (gemeten) return;
    meetFrames++;
    if (meetFrames >= 1000) {
        double totaal = glfwGetTime() - meetStart;
        std::printf("[Meting] modus=%s animatie=%s: gemiddelde frametijd %.4f ms (1000 frames)\n",
                    geoptimaliseerd ? "GEOPTIMALISEERD" : "NAIEF",
                    gepauzeerd ? "gepauzeerd" : "aan",
                    totaal / meetFrames * 1000.0);
        std::fflush(stdout);
        gemeten = true;
    }
}

void World::verwerkInvoer() {
    // Alle toetsen met edge-detectie elk frame pollen, zodat de
    // vorige-toestand altijd actueel is.
    bool k1 = toetsPress(GLFW_KEY_1);
    bool k2 = toetsPress(GLFW_KEY_2);
    bool k3 = toetsPress(GLFW_KEY_3);
    bool k4 = toetsPress(GLFW_KEY_4);
    bool kP = toetsPress(GLFW_KEY_P);
    bool kB = toetsPress(GLFW_KEY_B);

    if (k1) camera.setProjectieType(PROJ_ORTHO);
    if (k2) camera.setProjectieType(PROJ_PARALLEL);
    if (k3) camera.setProjectieType(PROJ_ISOMETRISCH);
    if (k4) camera.setProjectieType(PROJ_PERSPECTIEF);

    if (kP) {
        gepauzeerd = !gepauzeerd;
        resetMeting();
    }
    if (kB) {
        geoptimaliseerd = !geoptimaliseerd;
        resetMeting();
    }

    bool shift = glfwGetKey(venster, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                 glfwGetKey(venster, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;

    if (shift) {
        // Lichtbron verplaatsen
        float lichtStap = 3.0f * (float)frameDt;   // 3 eenheden per seconde
        float dx = 0, dy = 0, dz = 0;
        if (glfwGetKey(venster, GLFW_KEY_LEFT)  == GLFW_PRESS) dx -= lichtStap;
        if (glfwGetKey(venster, GLFW_KEY_RIGHT) == GLFW_PRESS) dx += lichtStap;
        if (glfwGetKey(venster, GLFW_KEY_UP)    == GLFW_PRESS) dy += lichtStap;
        if (glfwGetKey(venster, GLFW_KEY_DOWN)  == GLFW_PRESS) dy -= lichtStap;
        if (glfwGetKey(venster, GLFW_KEY_W)     == GLFW_PRESS) dz -= lichtStap;
        if (glfwGetKey(venster, GLFW_KEY_S)     == GLFW_PRESS) dz += lichtStap;

        if (dx != 0 || dy != 0 || dz != 0) {
            Vector p = lichtbron.getPositie();
            p.set(0, p.at(0) + dx);
            p.set(1, p.at(1) + dy);
            p.set(2, p.at(2) + dz);
            lichtbron.setPositie(p);
        }
        return;
    }

    float stap = 6.0f * (float)frameDt;    // 6 eenheden per seconde
    float kijk = 1.2f * (float)frameDt;    // 1,2 radialen per seconde

    if (glfwGetKey(venster, GLFW_KEY_W) == GLFW_PRESS) camera.beweeg(0, 0, -stap);
    if (glfwGetKey(venster, GLFW_KEY_S) == GLFW_PRESS) camera.beweeg(0, 0,  stap);
    if (glfwGetKey(venster, GLFW_KEY_A) == GLFW_PRESS) camera.beweeg(-stap, 0, 0);
    if (glfwGetKey(venster, GLFW_KEY_D) == GLFW_PRESS) camera.beweeg( stap, 0, 0);
    if (glfwGetKey(venster, GLFW_KEY_SPACE) == GLFW_PRESS) camera.beweeg(0,  stap, 0);
    if (glfwGetKey(venster, GLFW_KEY_E) == GLFW_PRESS) camera.beweeg(0, -stap, 0);

    if (glfwGetKey(venster, GLFW_KEY_LEFT)  == GLFW_PRESS) camera.draai(-kijk, 0);
    if (glfwGetKey(venster, GLFW_KEY_RIGHT) == GLFW_PRESS) camera.draai(kijk, 0);
    if (glfwGetKey(venster, GLFW_KEY_UP)    == GLFW_PRESS) camera.draai(0, -kijk);
    if (glfwGetKey(venster, GLFW_KEY_DOWN)  == GLFW_PRESS) camera.draai(0, kijk);
}


void World::run() {
    laatsteTijd = glfwGetTime();
    double vorigFrame = laatsteTijd;
    resetMeting();

    while (!glfwWindowShouldClose(venster)) {
        glfwPollEvents();

        // Bewegingssnelheid is tijdsafhankelijk, zodat de besturing
        // niet afhangt van de framerate.
        double nuFrame = glfwGetTime();
        frameDt = nuFrame - vorigFrame;
        vorigFrame = nuFrame;

        verwerkInvoer();

        render();
        glfwSwapBuffers(venster);

        meetFrame();
    }
}


World::~World() {
    // Shapes eerst opruimen, terwijl de GL-context nog bestaat.
    shapes.clear();
    if (venster) {
        glfwDestroyWindow(venster);
    }
    glfwTerminate();
}
