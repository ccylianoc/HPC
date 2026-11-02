#pragma once

// GLEW moet VOOR GLFW geincludeerd worden, anders kent GLFW
// de OpenGL-definities niet en krijg je compileerfouten.
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include "shaders.hpp"
#include "matrix.hpp"

// De World is het hart van de engine: hij beheert het venster,
// de OpenGL-context en de render-loop. Alle andere onderdelen
// (vormen, camera, licht) worden via deze class aangestuurd.
class World {
    private:
        GLFWwindow *venster;
        int breedte, hoogte;

        // Kubusgegevens, zoals in les 18: ribbe 2, van -1 tot 1.
        GLuint vao, vbo, ebo;
        GLuint programma;

        // Wordt elke frame aangeroepen. In latere stappen tekent
        // deze functie de scene; nu alleen het leegmaken.
        void render();

    public:
        World(int breedte, int hoogte, const std::string &titel);

        // Maakt het venster, initialiseert GLEW en start de loop.
        // Blokkeert tot het venster gesloten wordt.
        void run();

        ~World();
};
