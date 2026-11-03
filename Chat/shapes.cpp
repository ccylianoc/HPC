#include "shapes.hpp"
#include "shapes.h"
#include <stdexcept>

Shape::Shape() : vao(0), vbo(0), aantalVertices(0) {}

Shape::~Shape() {
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

void Shape::upload(const std::vector<float> &vertices, GLuint programma) {
    // 6 floats per vertex: 3 voor positie, 3 voor normaal.
    if (vertices.size() % 6 != 0) {
        throw std::invalid_argument("Vertexdata is geen veelvoud van 6 floats [upload]");
    }
    aantalVertices = (GLsizei)(vertices.size() / 6);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(vertices.size() * sizeof(float)),
                 vertices.data(),
                 GL_STATIC_DRAW);

    const GLsizei stride = 6 * sizeof(float);

    // pos: 3 floats op offset 0
    GLint posLoc = glGetAttribLocation(programma, "pos");
    if (posLoc >= 0) {
        glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(posLoc);
    }

    // normaal: 3 floats op offset 12
    GLint normLoc = glGetAttribLocation(programma, "normaal");
    if (normLoc >= 0) {
        glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, stride,
                              (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(normLoc);
    }

    glBindVertexArray(0);
}

// --- Kubus ---

Cube::Cube(GLuint programma) {
    std::vector<float> v(verticesCube, verticesCube + 36 * 6);
    upload(v, programma);
}

void Cube::draw() const {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, aantalVertices);
}

// --- Piramide ---

Pyramid::Pyramid(GLuint programma) {
    std::vector<float> v(verticesPyramid, verticesPyramid + 18 * 6);
    upload(v, programma);
}

void Pyramid::draw() const {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, aantalVertices);
}
