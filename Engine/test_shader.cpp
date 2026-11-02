#include "engine.hpp"
#include "shaders.hpp"
#include <iostream>

int main() {
    World wereld(100, 100, "Shader test");

    const std::string vertexBron = R"(#version 330 core
in vec3 pos;
void main() {
    gl_Position = vec4(pos, 1.0);
})";

    const std::string fragmentBron = R"(#version 330 core
out vec4 fragColor;
void main() {
    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
})";

    try {
        Shader vertex(GL_VERTEX_SHADER, vertexBron);
        vertex.compile();
        std::cout << "Vertex shader OK, id = " << vertex.getId() << "\n";

        Shader fragment(GL_FRAGMENT_SHADER, fragmentBron);
        fragment.compile();
        std::cout << "Fragment shader OK, id = " << fragment.getId() << "\n";

        Program programma(vertex, fragment);
        std::cout << "Programma OK, id = " << programma.getId() << "\n";
    } catch (const std::exception &e) {
        std::cout << "FOUT: " << e.what() << "\n";
    }

    return 0;
}
