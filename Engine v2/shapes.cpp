#include "shapes.hpp"
#include "shapes.h"
#include <stdexcept>

Shape::Shape() : vao(0), vbo(0), aantalVertices(0), model(4, 4) {}

Shape::~Shape() {
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

void Shape::upload(const std::vector<float> &vertices, GLuint programma) {
    // 9 floats per vertex: 3 positie, 3 normaal, 3 kleur (rgb = alpha-bron).
    if (vertices.size() % 9 != 0) {
        throw std::invalid_argument("Vertexdata is geen veelvoud van 9 floats [upload]");
    }
    aantalVertices = (GLsizei)(vertices.size() / 9);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(vertices.size() * sizeof(float)),
                 vertices.data(),
                 GL_STATIC_DRAW);

    const GLsizei stride = 9 * sizeof(float);

    GLint posLoc = glGetAttribLocation(programma, "pos");
    if (posLoc >= 0) {
        glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(posLoc);
    }

    GLint normLoc = glGetAttribLocation(programma, "normaal");
    if (normLoc >= 0) {
        glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, stride,
                              (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(normLoc);
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
