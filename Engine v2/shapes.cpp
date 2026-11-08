#include "shapes.hpp"
#include "shapes.h"
#include <stdexcept>
#include <cmath>

Shape::Shape() : vao(0), vbo(0), aantalVertices(0) {}

Shape::~Shape() {
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
    if (ebo) glDeleteBuffers(1, &ebo);
}

void Shape::upload(const std::vector<float> &vertices, GLuint programma) {
    // 8 floats per vertex: 3 positie, 3 normaal, 2 textuurcoordinaat.
    if (vertices.size() % 8 != 0) {
        throw std::invalid_argument("Vertexdata is geen veelvoud van 8 floats [upload]");
    }
    aantalVertices = (GLsizei)(vertices.size() / 8);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(vertices.size() * sizeof(float)),
                 vertices.data(),
                 GL_STATIC_DRAW);

    const GLsizei stride = 8 * sizeof(float);

    GLint posLoc = glGetAttribLocation(programma, "pos");
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(posLoc);

    GLint normLoc = glGetAttribLocation(programma, "normaal");
    glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(normLoc);

    GLint uvLoc = glGetAttribLocation(programma, "uv");
    if (uvLoc >= 0) {
        glVertexAttribPointer(uvLoc, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(uvLoc);
    }

    glBindVertexArray(0);
}

void Shape::uploadIndices(const std::vector<unsigned int> &indices) {
    aantalIndices = (GLsizei)indices.size();
    glBindVertexArray(vao);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 (GLsizeiptr)(indices.size() * sizeof(unsigned int)),
                 indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}


// --- Kubus ---

Cube::Cube(GLuint programma) {
    std::vector<float> v(verticesCube, verticesCube + 36 * 8);
    upload(v, programma);
}

void Cube::draw() const {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, aantalVertices);
}

// --- Piramide ---

Pyramid::Pyramid(GLuint programma) {
    std::vector<float> v(verticesPyramid, verticesPyramid + 18 * 8);
    upload(v, programma);
}

void Pyramid::draw() const {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, aantalVertices);
}

// --- Bol ---

Sphere::Sphere(GLuint programma) {
    std::vector<float> vertices;
    const int stacks = 24;
    const int sectors = 48;
    const float PI = 3.14159265359f;

    for (int i = 0; i <= stacks; i++) {
        float phi = PI * i / stacks;          // 0 .. PI
        for (int j = 0; j <= sectors; j++) {
            float theta = 2.0f * PI * j / sectors;
            float x = std::sin(phi) * std::cos(theta);
            float y = std::cos(phi);
            float z = std::sin(phi) * std::sin(theta);

            vertices.push_back(0.5f * x); vertices.push_back(0.5f * y); vertices.push_back(0.5f * z);
            vertices.push_back(x);        vertices.push_back(y);        vertices.push_back(z);
            vertices.push_back(0.5f + 0.5f * std::atan2(z, x) / PI);    // u
            vertices.push_back(1.0f - (float)i / stacks);               // v
        }
    }

    std::vector<unsigned int> indices;
    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < sectors; j++) {
            unsigned int a = i * (sectors + 1) + j;
            unsigned int b = a + sectors + 1;
            indices.push_back(a); indices.push_back(b); indices.push_back(a + 1);
            indices.push_back(a + 1); indices.push_back(b); indices.push_back(b + 1);
        }
    }

    upload(vertices, programma);
    uploadIndices(indices);
}

void Sphere::draw() const {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, aantalIndices, GL_UNSIGNED_INT, 0);
}

// --- Cilinder ---

Cylinder::Cylinder(GLuint programma, int segmenten) {
    if (segmenten < 3) {
        throw std::invalid_argument("Cilinder heeft minstens 3 segmenten nodig");
    }

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    const float PI = 3.14159265359f;
    const float r = 0.5f;      // straal
    const float h = 0.5f;      // halve hoogte

    // Zijkant: per segment twee ringen (onder en boven) met een
    // normaal die loodrecht naar buiten wijst.
    for (int i = 0; i <= segmenten; i++) {
        float hoek = 2.0f * PI * i / segmenten;
        float x = std::cos(hoek);
        float z = std::sin(hoek);
        float u = (float)i / segmenten;

        // onder
        vertices.insert(vertices.end(), {
            r * x, -h, r * z,   x, 0, z,   u, 0
        });
        // boven
        vertices.insert(vertices.end(), {
            r * x,  h, r * z,   x, 0, z,   u, 1
        });
    }

    for (int i = 0; i < segmenten; i++) {
        unsigned int a = i * 2;
        unsigned int b = a + 1;
        unsigned int c = a + 2;
        unsigned int d = a + 3;
        indices.push_back(a); indices.push_back(c); indices.push_back(b);
        indices.push_back(b); indices.push_back(c); indices.push_back(d);
    }

    // Deksels: een middelpunt plus de ring, met een vlakke normaal.
    auto deksel = [&](float y, float ny, float v) {
        unsigned int midden = (unsigned int)(vertices.size() / 8);
        vertices.insert(vertices.end(), { 0, y, 0,  0, ny, 0,  0.5f, 0.5f });
        unsigned int start = (unsigned int)(vertices.size() / 8);
        for (int i = 0; i <= segmenten; i++) {
            float hoek = 2.0f * PI * i / segmenten;
            float x = std::cos(hoek);
            float z = std::sin(hoek);
            vertices.insert(vertices.end(), {
                r * x, y, r * z,  0, ny, 0,  0.5f + 0.5f * x, 0.5f + 0.5f * z
            });
        }
        for (int i = 0; i < segmenten; i++) {
            if (ny > 0) {
                indices.push_back(midden);
                indices.push_back(start + i);
                indices.push_back(start + i + 1);
            } else {
                indices.push_back(midden);
                indices.push_back(start + i + 1);
                indices.push_back(start + i);
            }
        }
        (void)v;
    };

    deksel( h,  1.0f, 1.0f);
    deksel(-h, -1.0f, 0.0f);

    upload(vertices, programma);
    uploadIndices(indices);
}

void Cylinder::draw() const {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, aantalIndices, GL_UNSIGNED_INT, 0);
}
