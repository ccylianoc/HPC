#include "shaders.hpp"
#include <stdexcept>
#include <vector>

Shader::Shader(GLenum type, const std::string &bron) : type(type) {
    id = glCreateShader(type);
    const char *ptr = bron.c_str();
    glShaderSource(id, 1, &ptr, NULL);
}

Shader::~Shader() {
    if (id != 0) {
        glDeleteShader(id);
    }
}

void Shader::compile() {
    glCompileShader(id);

    // Opvragen of het compileren gelukt is. Zonder deze check
    // krijg je alleen een leeg scherm zonder enige uitleg.
    GLint gelukt = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &gelukt);

    if (!gelukt) {
        // De log kan lang zijn, dus eerst de lengte opvragen.
        GLint lengte = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lengte);

        std::vector<char> log(lengte > 1 ? lengte : 1);
        glGetShaderInfoLog(id, lengte, NULL, log.data());

        std::string soort = (type == GL_VERTEX_SHADER) ? "vertex" : "fragment";
        throw std::runtime_error(
            "Compileren van " + soort + "-shader mislukt:\n" + log.data());
    }
}

Program::Program(const Shader &vertex, const Shader &fragment) {
    id = glCreateProgram();
    glAttachShader(id, vertex.getId());
    glAttachShader(id, fragment.getId());
    glLinkProgram(id);

    GLint gelukt = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &gelukt);

    if (!gelukt) {
        GLint lengte = 0;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &lengte);

        std::vector<char> log(lengte > 1 ? lengte : 1);
        glGetProgramInfoLog(id, lengte, NULL, log.data());

        throw std::runtime_error(
            std::string("Linken van shader-programma mislukt:\n") + log.data());
    }

    // De shaders zitten nu in het programma; de losse objecten
    // mogen weg. Dit is geen fout: het programma blijft werken.
    glDetachShader(id, vertex.getId());
    glDetachShader(id, fragment.getId());
}

Program::~Program() {
    if (id != 0) {
        glDeleteProgram(id);
    }
}

void Program::use() const {
    glUseProgram(id);
}

GLuint Program::release() {
    GLuint oud = id;
    id = 0;
    return oud;
}
