// compile on MacOs:
// c++ -I /opt/homebrew/include -L /opt/homebrew/lib -o glfw_rotating_cube_texture -lglfw -lglew -framework OpenGL -DGL_SILENCE_DEPRECATION glfw_rotating_cube_texture.cpp

// compile on Windows:
// g++ -o glfw_rotating_cube_texture.exe glfw_rotating_cube_texture.cpp -lglfw3 -lopengl32 -lglew32

#include <cstdlib>
#include <cstdio>
#define _USE_MATH_DEFINES
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define ROTATION_SPEED 0.5 // radians per second
#define BUFSIZE 256 // size of error log buffer

// Exit when a key is pressed
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    glfwTerminate();
    exit(0);
}

int main(int argc, char** argv) {
    // Initialize the GLFW library
    if (!glfwInit()) {
        exit(-1);
    }

    // Create a windowed mode window and its OpenGL context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = glfwCreateWindow(640, 480, "Rotating cube", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(-1);
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Init GLEW
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        exit(-1);
    }

    // Synchronize buffer swap interval with refresh rate
    glfwSwapInterval(1);

    // Set the callback function for keypresses
    glfwSetKeyCallback(window, keyCallback);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Compile vertex shader
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const char *vsSource =
        R"(#version 330 core
        in vec3 pos;
        in vec3 col;
        in vec2 tex;
        out vec3 color;
        out vec2 texCoord;
        uniform mat4 projection;
        uniform mat4 translation;
        uniform mat4 rotation;
        uniform float rotationAngle;
        void main() {
            color = col;
            texCoord = tex;
            float s = sin(rotationAngle);
            float c = cos(rotationAngle);
            mat4 rotationY = mat4(c, 0, -s, 0,
                                  0, 1,  0, 0,
                                  s, 0,  c, 0,
                                  0, 0,  0, 1); // Rotation matrix in column-major order
            gl_Position = projection * translation * rotation * rotationY * vec4(pos, 1.0);
        })";
    glShaderSource(vs, 1, &vsSource, NULL);
    glCompileShader(vs);
    GLint status;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
    if (!status) {
        char log[BUFSIZE];
        glGetShaderInfoLog(vs, BUFSIZE, NULL, log);
        puts(log);
        exit(-1);
    }

    // Compile fragment shader
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fsSource = 
        R"(#version 330 core
        in vec3 color;
        in vec2 texCoord;
        uniform sampler2D textureSampler;
        out vec4 fragColor;
        void main() {
            fragColor = texture(textureSampler, texCoord) * vec4(color, 1.0);
        })";
    glShaderSource(fs, 1, &fsSource, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
    if (!status) {
        char log[BUFSIZE];
        glGetShaderInfoLog(fs, BUFSIZE, NULL, log);
        puts(log);
        exit(-1);
    }

    // Combine the two shaders into a program
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);
    glUseProgram(program);

    // Vertices for cube
    GLfloat vertices[] = {
        1,  1,  1, 1, 1, 1, 5, 0, // front top right
        1, -1,  1, 1, 0, 1, 5, 5, // front bottom right
       -1, -1,  1, 0, 0, 1, 0, 5, // front bottom left
       -1,  1,  1, 0, 1, 1, 0, 0, // front top left
        1,  1, -1, 1, 1, 0, 0, 0, // back top right
        1, -1, -1, 1, 0, 0, 0, 5, // back bottom right
       -1, -1, -1, 0, 0, 0, 5, 5, // back bottom left
       -1,  1, -1, 0, 1, 0, 5, 0, // back top left
    };

    // Edges for cube
    GLushort edges[] = {
        0, 1, 2, 2, 3, 0, // front
        4, 5, 6, 6, 7, 4, // back
        0, 4, 1, 1, 4, 5, // bottom
        2, 6, 3, 3, 6, 7, // top
        0, 3, 4, 4, 7, 3, // left
        1, 2, 5, 5, 6, 2  // right
    };

 
    // Create and bind a Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create and bind a Vertex Buffer Object
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    // Fill the VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
     
    // Create and bind an Element Buffer Object
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    
    // Fill the EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(edges), edges, GL_STATIC_DRAW);
     
    // Positions using 3 floats per vertex
    GLint posLocation = glGetAttribLocation(program, "pos");
    glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 32, 0);
    glEnableVertexAttribArray(posLocation);

    // Colors using 3 floats per vertex with an offset of 12 bytes
    GLint colLocation = glGetAttribLocation(program, "col");
    glVertexAttribPointer(colLocation, 3, GL_FLOAT, GL_FALSE, 32, (void *)12);
    glEnableVertexAttribArray(colLocation);

    // Texture coordinates using 2 floats per vertex with an offset of 24 bytes
    GLint texLocation = glGetAttribLocation(program, "tex");
    glVertexAttribPointer(texLocation, 2, GL_FLOAT, GL_FALSE, 32, (void *)24);
    glEnableVertexAttribArray(texLocation);

    // Set the background color
    glClearColor(0.1, 0.1, 0.1, 1); // r, g, b, alpha

    // Perspective projection matrix
    GLint projectionLocation = glGetUniformLocation(program, "projection");
    GLfloat projection[] = {1, 0,  0,  0,
                            0, 1,  0,  0,
                            0, 0, -1, -1,
                            0, 0, -1,  0};
    glUniformMatrix4fv(projectionLocation, 1, GL_TRUE, projection);

    // Translation matrix; move everything 3 units to the back
    GLint translationLocation = glGetUniformLocation(program, "translation");
    GLfloat translation[] = {1, 0, 0,  0,
                             0, 1, 0,  0,
                             0, 0, 1, -3,
                             0, 0, 0,  1};
    glUniformMatrix4fv(translationLocation, 1, GL_TRUE, translation);

    // Fixed rotation matrix; rotate 30° around x-axis
    GLint rotationLocation = glGetUniformLocation(program, "rotation");
    GLfloat c = cos(M_PI / 6);
    GLfloat s = sin(M_PI / 6);
    GLfloat rotation[] = {1, 0,  0, 0,
                          0, c, -s, 0,
                          0, s,  c, 0,
                          0, 0,  0, 1};
    glUniformMatrix4fv(rotationLocation, 1, GL_TRUE, rotation);

    // Get location of rotation angle uniform
    GLint rotationAngleLocation = glGetUniformLocation(program, "rotationAngle");
  
    // Create a texture buffer
    GLuint texture;
    glGenTextures(1, &texture);

    // Bind the texture buffer
    glBindTexture(GL_TEXTURE_2D, texture);

    // Fill the texture buffer
    const int SIZE = 64;
    GLubyte check[SIZE * SIZE * 3];
    for (int i = 0; i < SIZE * SIZE * 3; i += 3) {
        if (((i <  SIZE * SIZE * 3 / 2) && (i % (3 * SIZE) <  3 * SIZE / 2)) || 
            ((i >= SIZE * SIZE * 3 / 2) && (i % (3 * SIZE) >= 3 * SIZE / 2))) {
            check[i] = 127;
            check[i + 1] = 127;
            check[i + 2] = 127;
        } else {
            check[i] = 255;
            check[i + 1] = 255;
            check[i + 2] = 255;
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SIZE, SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, check);

    // Specify how the texture should be scaled
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Set a square viewport
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport((width - height) / 2, 0, height, height);

        // Clear canvas
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set rotation angle
        GLfloat angle = ROTATION_SPEED * glfwGetTime();
        glUniform1f(rotationAngleLocation, angle);

        // Tell opengl to draw the vertices in the arrayBuffer pointed to
        // by the indices in the elementArrayBuffer as a series of lines
        glDrawElements(GL_TRIANGLES, sizeof(edges) / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

        // Swap buffers
        glfwSwapBuffers(window);

        // Check for keypress
        glfwPollEvents();
    }
    
    // Terminate the GLFW library
    glfwTerminate();
    return 0;
}