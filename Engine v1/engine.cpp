#include "engine.hpp"
#include <stdexcept>
#include <cmath>

// De acht hoekpunten van de kubus, ribbe 2 (van -1 tot 1),
// precies zoals in les 18.
static const GLfloat kubusVertices[] = {
    -1, -1,  1,   // 0 voorkant linksonder
     1, -1,  1,   // 1 voorkant rechtsonder
     1,  1,  1,   // 2 voorkant rechtsboven
    -1,  1,  1,   // 3 voorkant linksboven
    -1, -1, -1,   // 4 achterkant linksonder
     1, -1, -1,   // 5 achterkant rechtsonder
     1,  1, -1,   // 6 achterkant rechtsboven
    -1,  1, -1    // 7 achterkant linksboven
};

// De twaalf ribben als indexparen, zoals in les 18.
static const GLushort kubusEdges[] = {
    0, 1,  1, 2,  2, 3,  3, 0,   // voorkant
    4, 5,  5, 6,  6, 7,  7, 4,   // achterkant
    0, 4,  1, 5,  2, 6,  3, 7    // verbindingen voor-achter
};

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

    // --- Shaders, zoals in les 18 ---
    const std::string vertexBron = R"(#version 330 core
        in vec3 pos;
uniform mat4 projection;
uniform mat4 translation;
uniform mat4 rotation;
void main() {
vec4 clip = projection * translation * rotation * vec4(pos, 1.0);
gl_Position = vec4(clip.xyz / clip.w, 1.0);

})";

    const std::string fragmentBron = R"(#version 330 core
out vec4 fragColor;
void main() {
    fragColor = vec4(0.5, 0.5, 1.0, 1.0);
})";

    Shader vertex(GL_VERTEX_SHADER, vertexBron);
    vertex.compile();
    Shader fragment(GL_FRAGMENT_SHADER, fragmentBron);
    fragment.compile();
    Program prog(vertex, fragment);
    programma = prog.release();
    glUseProgram(programma);

    // --- VAO/VBO/EBO, zoals in les 18 ---
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kubusVertices), kubusVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kubusEdges), kubusEdges, GL_STATIC_DRAW);

    GLint posLocation = glGetAttribLocation(programma, "pos");
    glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posLocation);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void World::render() {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(programma);

    // Perspectief via de Transformatie-class: 45 graden gezichtsveld,
    // aspectratio van het venster, en een bereik van 0.1 tot 100.
    Matrix4 projectie = Transformatie::perspectief(
        45.0f, (float)breedte / (float)hoogte, 0.1f, 100.0f).naarMatrix();

    // Kubus naar achteren, zodat hij voor de camera staat.
    Matrix4 translatie = Transformatie::translatie(0, 0, -3).naarMatrix();

    // Animatie: rotatie rond de y-as op basis van de tijd.
    float hoek = (float)glfwGetTime();
    Matrix4 rotatie = Transformatie::rotatie(hoek, Y_AS).naarMatrix();

    GLfloat glProjectie[16], glTranslatie[16], glRotatie[16];
    projectie.naarGL(glProjectie);
    translatie.naarGL(glTranslatie);
    rotatie.naarGL(glRotatie);

    glUniformMatrix4fv(glGetUniformLocation(programma, "projection"), 1, GL_FALSE, glProjectie);
    glUniformMatrix4fv(glGetUniformLocation(programma, "translation"), 1, GL_FALSE, glTranslatie);
    glUniformMatrix4fv(glGetUniformLocation(programma, "rotation"), 1, GL_FALSE, glRotatie);

    glBindVertexArray(vao);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_SHORT, 0);
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
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
    if (venster) {
        glfwDestroyWindow(venster);
    }
    glfwTerminate();
}
