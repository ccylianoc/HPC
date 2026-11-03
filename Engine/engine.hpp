#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include "shaders.hpp"
#include "matrix.hpp"

class World {
    private:
        GLFWwindow *venster;
        int breedte, hoogte;

        GLuint vao, vbo, ebo;
        GLuint programma;

        void render();

    public:
        World(int breedte, int hoogte, const std::string &titel);

        void run();

        ~World();
};
