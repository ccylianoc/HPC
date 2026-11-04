#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <memory>
#include "shaders.hpp"
#include "matrix.hpp"
#include "shapes.hpp"

class World {
    private:
        GLFWwindow *venster;
        int breedte, hoogte;

        GLuint programma;
        GLuint texture;
        std::vector<std::unique_ptr<Shape>> shapes;
        void tekenShape(size_t i, float hoek);

        void render();

    public:
        World(int breedte, int hoogte, const std::string &titel);

        void run();

        ~World();
};
