#include "engine.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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
    // De normaal wordt doorgegeven aan de fragment-shader, zodat je
    // later belichting kunt toevoegen zonder de pipeline te wijzigen.
    const std::string vertexBron = 
        R"(#version 330 core
        in vec3 pos;
        in vec3 normaal;
        in vec2 uv;
        uniform mat4 projection;
        uniform mat4 view;
        uniform mat4 model;
        out vec3 fragNormaal;
        out vec3 fragPos;
        out vec2 fragUV;
        void main() {
            fragNormaal = mat3(model) * normaal;
            fragUV = uv;
            vec4 wereld = model * vec4(pos, 1.0);
            fragPos = wereld.xyz;
            vec4 clip = projection * view * wereld;
            gl_Position = vec4(clip.xyz / clip.w, 1.0);
        })";

    const std::string fragmentBron = 
        R"(#version 330 core
        in vec3 fragNormaal;
        in vec3 fragPos;
        out vec4 fragColor;

        vec3 lightPos = vec3(0.2, 1.2, 0.0);
        vec3 lightColor = vec3(1.0, 1.0, 1.0);

        struct Material {
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            float shininess;
        };
        uniform Material material;

        void main() {
            vec3 n = normalize(fragNormaal);
            vec3 naarLicht = normalize(lightPos - fragPos);

            vec3 ambient = lightColor * material.ambient;

            float diffuseFactor = max(0.0, dot(n, naarLicht));
            vec3 diffuse = diffuseFactor * lightColor * material.diffuse;

            vec3 reflectie = reflect(-naarLicht, n);
            vec3 naarOog = normalize(-fragPos);
            float specularFactor = pow(max(0.0, dot(naarOog, reflectie)), material.shininess);
            vec3 specular = specularFactor * lightColor * material.specular;

            fragColor = vec4(ambient + diffuse + specular, 1.0);
        })";

    Shader vertex(GL_VERTEX_SHADER, vertexBron);
    vertex.compile();
    Shader fragment(GL_FRAGMENT_SHADER, fragmentBron);
    fragment.compile();
    Program prog(vertex, fragment);
    programma = prog.release();
    glUseProgram(programma);

    // --- Texture inladen (eenmalig) ---
    int texBreedte, texHoogte, texKanalen;
    stbi_set_flip_vertically_on_load(true);   // v = 0 onderaan
    unsigned char *data = stbi_load("texture.png", &texBreedte, &texHoogte, &texKanalen, 0);
    if (!data) {
        throw std::runtime_error("Texture inladen mislukt: texture.png");
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texBreedte, texHoogte, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);


    // --- Shapes aanmaken; het programma moet actief zijn voor de
    // attribuut-locaties opgevraagd kunnen worden. ---
    shapes.push_back(std::make_unique<Cube>(programma));
    shapes.push_back(std::make_unique<Pyramid>(programma));

    glEnable(GL_DEPTH_TEST);
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
    glUniform3f(glGetUniformLocation(programma, "viewPos"), 0.0f, 0.0f, 0.0f);

    // Materiaal: goud
    glUniform3f(glGetUniformLocation(programma, "material.ambient"),  0.24725f, 0.1995f, 0.0745f);
    glUniform3f(glGetUniformLocation(programma, "material.diffuse"),  0.75164f, 0.60648f, 0.22648f);
    glUniform3f(glGetUniformLocation(programma, "material.specular"), 0.628281f, 0.555802f, 0.366065f);
    glUniform1f(glGetUniformLocation(programma, "material.shininess"), 32.0f);


    glUniformMatrix4fv(glGetUniformLocation(programma, "projection"), 1, GL_FALSE, glProjectie);
    glUniformMatrix4fv(glGetUniformLocation(programma, "view"), 1, GL_FALSE, glView);

    float hoek = (float)glfwGetTime();

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(programma, "tex"), 0);


    // Kubus links, piramide rechts. Elke vorm draait om zijn eigen y-as.
    for (size_t i = 0; i < shapes.size(); i++) {
        const Materiaal &m = shapes[i]->getMateriaal();
        glUniform3fv(glGetUniformLocation(programma, "material.ambient"), 1, m.ambient);
        glUniform3fv(glGetUniformLocation(programma, "material.diffuse"), 1, m.diffuse);
        glUniform3fv(glGetUniformLocation(programma, "material.specular"), 1, m.specular);
        glUniform1f(glGetUniformLocation(programma, "material.shininess"), m.shininess);
        glUniform1f(glGetUniformLocation(programma, "alpha"), m.alpha);


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
    if (texture) glDeleteTextures(1, &texture);
    shapes.clear();
    if (venster) {
        glfwDestroyWindow(venster);
    }
    glfwTerminate();
}
