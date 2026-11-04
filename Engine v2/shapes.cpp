#include "shapes.hpp"
#include "shapes.h"
#include <stdexcept>

Shape::Shape() : vao(0), vbo(0), aantalVertices(0), model(4, 4) {}

Shape::~Shape() {
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

void Shape::upload(const std::vector<float> &vertices, GLuint programma) {
    // 8 floats per vertex: 3 positie, 3 normaal, 2 kleur (rgb = alpha-bron).
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


    // kleur: 3 floats op offset 24
    GLint kleurLoc = glGetAttribLocation(programma, "kleur");
    if (kleurLoc >= 0) {
        glVertexAttribPointer(kleurLoc, 3, GL_FLOAT, GL_FALSE, stride,
                              (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(kleurLoc);
    }

    glBindVertexArray(0);
}

void Shape::uploadIndices(const std::vector<unsigned int> &indices) {
    aantalIndices = (GLsizei)indices.size();
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 (GLsizeiptr)(indices.size() * sizeof(unsigned int)),
                 indices.data(), GL_STATIC_DRAW);
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

// --- Sphere ---

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

            vertices.push_back(x); vertices.push_back(y); vertices.push_back(z); // positie
            vertices.push_back(x); vertices.push_back(y); vertices.push_back(z); // normaal (eenheidsbol)
            vertices.push_back(0.5f + 0.5f * std::atan2(z, x) / PI);              // u
            vertices.push_back(1.0f - (float)i / stacks);                         // v
        }
    }

    // Indices voor de driehoeken
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
    uploadIndices(indices);   // zie stap 3
}
