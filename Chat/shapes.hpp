#pragma once

#include <GL/glew.h>
#include <vector>
#include "matrix.hpp"

// Een Shape is een tekenbare vorm met eigen geometrie op de GPU.
// De vertices komen uit shapes.h en bestaan uit 6 floats per vertex:
// x, y, z (positie) gevolgd door nx, ny, nz (normaal).
class Shape {
    protected:
        GLuint vao, vbo;
        GLsizei aantalVertices;

        // Zet de vertices in een VAO/VBO en koppelt de attributen
        // pos en normaal aan het actieve shader-programma.
        void upload(const std::vector<float> &vertices, GLuint programma);

        // Modelmatrix: schaal, rotatie en positie van deze vorm.
        Matrix model;

    public:
        Shape();
        virtual ~Shape();

        // Elke vorm tekent zichzelf met het actieve shader-programma.
        virtual void draw() const = 0;

        void setModel(const Matrix &m) { model = m; }
        Matrix getModel() const { return model; }

        Shape(const Shape &) = delete;
        Shape &operator=(const Shape &) = delete;
};

// Kubus met ribbe 1, gecentreerd op de oorsprong (36 vertices).
class Cube : public Shape {
    public:
        explicit Cube(GLuint programma);
        void draw() const override;
};

// Piramide met grondvlak op y = -0.5 en top op (0, 0.5, 0) (18 vertices).
class Pyramid : public Shape {
    public:
        explicit Pyramid(GLuint programma);
        void draw() const override;
};
