#include "engine.hpp"
#include <stdexcept>
#include <cmath>

World::World(int breedte, int hoogte, const std::string &titel)
    : venster(nullptr), breedte(breedte), hoogte(hoogte) {

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

    // --- Shaders ---
    const std::string vertexBron =
        R"(#version 330 core
        in vec2 uv;
        in vec3 pos;
        in vec3 normaal;
        uniform mat4 projection;
        uniform mat4 view;
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
            vec4 clip = projection * view * wereld;
            gl_Position = vec4(clip.xyz / clip.w, 1.0);
        })";

    const std::string fragmentBron =
        R"(#version 330 core
        in vec2 fragUV;
        in vec3 fragNormaal;
        in vec3 fragPos;
        in vec3 fragObjectPos;
        out vec4 fragColor;

        uniform sampler2D tex;
        uniform bool gebruiktTexture;
        uniform vec3 lightPos;
        uniform vec3 lightColor;

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

            vec3 basiskleur = gebruiktTextuur ? texture(tex, fragUV).rgb : vec3(1.0);

            vec3 ambient = lightColor * material.ambient * basiskleur;

            float diffuseFactor = max(0.0, dot(n, naarLicht));
            vec3 diffuse = diffuseFactor * lightColor * material.diffuse * basiskleur;
            
            vec3 reflectie = reflect(-naarLicht, n);
            vec3 naarOog = normalize(-fragPos);
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

    // --- Shapes aanmaken; het programma moet actief zijn voor de
    // attribuut-locaties opgevraagd kunnen worden. ---
    shapes.push_back(std::make_unique<Cube>(programma));
    shapes.push_back(std::make_unique<Pyramid>(programma));

    // --- Materialen (eenmalig instellen) ---
    Materiaal goud;
    goud.ambient[0] = 0.24725f; goud.ambient[1] = 0.1995f;  goud.ambient[2] = 0.0745f;
    goud.diffuse[0] = 0.75164f; goud.diffuse[1] = 0.60648f; goud.diffuse[2] = 0.22648f;
    goud.specular[0] = 0.628281f; goud.specular[1] = 0.555802f; goud.specular[2] = 0.366065f;
    goud.shininess = 32.0f;
    goud.alpha = 1.0f;
    shapes[0]->setMateriaal(goud);

    Materiaal glas;
    glas.ambient[0] = 0.1f;  glas.ambient[1] = 0.1f;  glas.ambient[2] = 0.15f;
    glas.diffuse[0] = 0.3f;  glas.diffuse[1] = 0.4f;  glas.diffuse[2] = 0.5f;
    glas.specular[0] = 1.0f; glas.specular[1] = 1.0f; glas.specular[2] = 1.0f;
    glas.shininess = 64.0f;
    glas.alpha = 0.4f;      // doorzichtig
    shapes[1]->setMateriaal(glas);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void World::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programma);

    // Perspectief via de Transformatie-class.
    Matrix4 projectie = Transformatie::perspectief(
        45.0f, (float)breedte / (float)hoogte, 0.1f, 100.0f).naarMatrix();

    // Camera staat op de oorsprong en kijkt naar -z; de view-matrix
    // schuift de wereld 4 eenheden naar voren.
    Matrix4 view = Transformatie::translatie(0, 0, -4).naarMatrix();

    GLfloat glProjectie[16], glView[16];
    projectie.naarGL(glProjectie);
    view.naarGL(glView);

    glUniform3f(glGetUniformLocation(programma, "lightPos"), 0.2f, 1.2f, 0.0f);
    glUniform3f(glGetUniformLocation(programma, "lightColor"), 1.0f, 1.0f, 1.0f);

    glUniformMatrix4fv(glGetUniformLocation(programma, "projection"), 1, GL_FALSE, glProjectie);
    glUniformMatrix4fv(glGetUniformLocation(programma, "view"), 1, GL_FALSE, glView);

    float hoek = (float)glfwGetTime();

    // Pass 1: ondoorzichtige objecten (schrijven diepte).
    for (size_t i = 0; i < shapes.size(); i++) {
        if (shapes[i]->getMateriaal().alpha >= 1.0f) tekenShape(i, hoek);
    }

    // Pass 2: transparante objecten, zonder diepte te schrijven.
    glDepthMask(GL_FALSE);
    for (size_t i = 0; i < shapes.size(); i++) {
        if (shapes[i]->getMateriaal().alpha < 1.0f) tekenShape(i, hoek);
    }
    glDepthMask(GL_TRUE);
}

void World::tekenShape(size_t i, float hoek) {
    // Materiaal-uniforms per object.
    const Materiaal &m = shapes[i]->getMateriaal();
    glUniform3fv(glGetUniformLocation(programma, "material.ambient"), 1, m.ambient);
    glUniform3fv(glGetUniformLocation(programma, "material.diffuse"), 1, m.diffuse);
    glUniform3fv(glGetUniformLocation(programma, "material.specular"), 1, m.specular);
    glUniform1f(glGetUniformLocation(programma, "material.shininess"), m.shininess);
    glUniform1f(glGetUniformLocation(programma, "alpha"), m.alpha);

    // Kubus links, piramide rechts. Elke vorm draait om zijn eigen y-as.
    Matrix4 schaal = Transformatie::schaling(1.5f, 1.5f, 1.5f).naarMatrix();
    Matrix4 rotatie = Transformatie::rotatie(hoek, Y_AS).naarMatrix();
    Matrix4 positie = Transformatie::translatie(i == 0 ? -1.2f : 1.2f, 0, 0).naarMatrix();

    // Volgorde: eerst schalen, dan roteren, dan verplaatsen.
    Matrix model = positie * rotatie * schaal;
    shapes[i]->setModel(model);

    GLfloat glModel[16];
    model.naarGL(glModel);
    glUniformMatrix4fv(glGetUniformLocation(programma, "model"), 1, GL_FALSE, glModel);

    shapes[i]->draw();
}

void World::run() {
    while (!glfwWindowShouldClose(venster)) {
        int w, h;
        glfwGetFramebufferSize(venster, &w, &h);
        glViewport(0, 0, w, h);

        render();

        glfwSwapBuffers(venster);
        glfwPollEvents();
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