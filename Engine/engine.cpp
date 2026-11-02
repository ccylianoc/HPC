#include "engine.hpp"
#include <stdexcept>

World::World(int breedte, int hoogte, const std::string &titel)
    : venster(nullptr), breedte(breedte), hoogte(hoogte) {

    if (!glfwInit()) {
        throw std::runtime_error("GLFW initialisatie mislukt.");
    }

    // Core profile 3.3: verplicht voor de shader-pipeline.
    // Zonder deze hints krijg je een compatibiliteitscontext
    // waarin moderne shaders niet werken.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

    venster = glfwCreateWindow(breedte, hoogte, titel.c_str(), NULL, NULL);
    if (!venster) {
        glfwTerminate();
        throw std::runtime_error("Venster aanmaken mislukt.");
    }

    glfwMakeContextCurrent(venster);
    glfwSwapInterval(1);

    // GLEW moet NA het aanmaken van de context geinitialiseerd
    // worden, anders zijn de OpenGL-functies nog niet beschikbaar.
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("GLEW initialisatie mislukt.");
    }

    // Viewport instellen op de volledige venstergrootte.
    glViewport(0, 0, breedte, hoogte);
}

void World::render() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void World::run() {
    while (!glfwWindowShouldClose(venster)) {
        render();
        glfwSwapBuffers(venster);
        glfwPollEvents();
    }
}

World::~World() {
    if (venster) {
        glfwDestroyWindow(venster);
    }
    glfwTerminate();
}
