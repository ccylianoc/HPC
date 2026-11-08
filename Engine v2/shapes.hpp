#pragma once

#include <GL/glew.h>
#include <vector>
#include "matrix.hpp"
#include "transformatie.hpp"

// Een Shape is een tekenbare vorm met eigen geometrie op de GPU.
// De vertices komen uit shapes.h en bestaan uit 8 floats per vertex:
// x, y, z (positie), nx, ny, nz (normaal) en u, v (textuurcoordinaten).

struct Materiaal {
    float ambient[3]  = {0.3f, 0.3f, 0.3f};
    float diffuse[3]  = {0.8f, 0.8f, 0.8f};
    float specular[3] = {1.0f, 1.0f, 1.0f};
    float shininess   = 32.0f;
    float alpha       = 1.0f;   // 1.0 = ondoorzichtig
    bool gebruiktTextuur = false;
};


class Shape {
    protected:
        GLuint vao, vbo;
        GLsizei aantalVertices;
        GLuint ebo = 0;
        GLsizei aantalIndices = 0;

        // Zet de vertices in een VAO/VBO en koppelt de attributen
        // pos, normaal en uv aan het actieve shader-programma.
        void upload(const std::vector<float> &vertices, GLuint programma);

    private:
        Materiaal materiaal;
        Transform transform;

    public:
        void setMateriaal(const Materiaal &m) { materiaal = m; }
        const Materiaal &getMateriaal() const { return materiaal; }

        // Geeft toegang tot de transformatie van deze vorm, zodat de
        // gebruiker schaal, rotatie en translatie kan instellen.
        Transform &getTransform() { return transform; }
        const Transform &getTransform() const { return transform; }

        // Modelmatrix op basis van de huidige transformatie.
        Matrix4 getModel() const { return transform.model(); }

        Shape();
        virtual ~Shape();

        // Elke vorm tekent zichzelf met het actieve shader-programma.
        virtual void draw() const = 0;

        Shape(const Shape &) = delete;
        Shape &operator=(const Shape &) = delete;
        void uploadIndices(const std::vector<unsigned int> &indices);
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

// Bol met diameter 1, gecentreerd op de oorsprong.
class Sphere : public Shape {
    public:
        explicit Sphere(GLuint programma);
        void draw() const override;
};

// Cilinder met diameter 1 en hoogte 1, gecentreerd op de oorsprong.
// De as staat langs y; de zijkant is opgebouwd uit een ring van
// segmenten met per vertex een naar buiten wijzende normaal.
class Cylinder : public Shape {
    public:
        explicit Cylinder(GLuint programma, int segmenten = 48);
        void draw() const override;
};
