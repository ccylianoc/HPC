#pragma once

#include <GL/glew.h>
#include <vector>
#include "matrix.hpp"
#include "transformatie.hpp"
#include "properties.hpp"
#include <memory>
#include <type_traits>
#include <utility>

// Een Shape is een tekenbare vorm met eigen geometrie op de GPU.
// De vertices komen uit shapes.h en bestaan uit 8 floats per vertex:
// x, y, z (positie), nx, ny, nz (normaal) en u, v (textuurcoordinaten).

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
        std::vector<std::unique_ptr<Property>> eigenschappen;
        float snelheid[3] = {0.0f, 0.0f, 0.0f};   // rotatiesnelheid in rad/s

        // Cache van de modelmatrix; wordt alleen herberekend als de
        // versie van de Transform verandert.
        mutable Matrix4 modelCache;
        mutable unsigned long cacheVersie = (unsigned long)-1;

    public:
        void setMateriaal(const Materiaal &m) { materiaal = m; }
        const Materiaal &getMateriaal() const { return materiaal; }

        // Voegt een eigenschap toe (kleur, opaciteit, textuur, ...) en past
        // het materiaal meteen aan. Gebruik:
        //   vorm.addProperty<Kleur>(1.0f, 0.0f, 0.0f);
        // Latere eigenschappen van hetzelfde soort overschrijven eerdere.
        template <typename P, typename... Args>
        P &addProperty(Args &&...args) {
            static_assert(std::is_base_of<Property, P>::value,
                          "addProperty verwacht een subclass van Property");
            auto eigenschap = std::make_unique<P>(std::forward<Args>(args)...);
            eigenschap->apply(materiaal);
            P &verwijzing = *eigenschap;
            eigenschappen.push_back(std::move(eigenschap));
            return verwijzing;
        }

        // Laat de vorm vanzelf draaien: rotatie = snelheid * tijd (rad/s).
        // Alle drie nul betekent geen animatie.
        void setRotatieSnelheid(float x, float y, float z) {
            snelheid[0] = x; snelheid[1] = y; snelheid[2] = z;
        }
        float getRotatieSnelheid(int as) const { return snelheid[as]; }
        bool isGeanimeerd() const {
            return snelheid[0] != 0.0f || snelheid[1] != 0.0f || snelheid[2] != 0.0f;
        }

        // Geeft toegang tot de transformatie van deze vorm, zodat de
        // gebruiker schaal, rotatie en translatie kan instellen.
        Transform &getTransform() { return transform; }
        const Transform &getTransform() const { return transform; }

        // Modelmatrix met cache: alleen opnieuw berekend bij een gewijzigde Transform.
        const Matrix4 &getModel() const {
            if (cacheVersie != transform.versie()) {
                modelCache = transform.model();
                cacheVersie = transform.versie();
            }
            return modelCache;
        }

        // Modelmatrix zonder cache (voor de "naieve" meetmodus).
        Matrix4 getModelOngecachet() const { return transform.model(); }

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
