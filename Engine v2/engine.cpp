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
    // De normaal wordt doorgegeven aan de fragment-shader, zodat je
    // later belichting kunt toevoegen zonder de pipeline te wijzigen.
    const std::string vertexBron = R"(#version 330 core
in vec3 pos;
in vec3 normaal;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
out vec3 fragNormaal;
void main() {
    fragNormaal = normaal;
    vec4 clip = projection * view * model * vec4(pos, 1.0);
    gl_Position = vec4(clip.xyz / clip.w, 1.0);
})";

    const std::string fragmentBron = R"(#version 330 core
in vec3 fragNormaal;
out vec4 fragColor;
void main() {
    // Voorlopig effen kleur; de normaal is al beschikbaar voor belichting.
    fragColor = vec4(0.5, 0.5, 1.0, 1.0);
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

    glUniformMatrix4fv(glGetUniformLocation(programma, "projection"), 1, GL_FALSE, glProjectie);
    glUniformMatrix4fv(glGetUniformLocation(programma, "view"), 1, GL_FALSE, glView);

    float hoek = (float)glfwGetTime();

    // Kubus links, piramide rechts. Elke vorm draait om zijn eigen y-as.
    for (size_t i = 0; i < shapes.size(); i++) {
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
    shapes.clear();
    if (venster) {
        glfwDestroyWindow(venster);
    }
    glfwTerminate();
}
