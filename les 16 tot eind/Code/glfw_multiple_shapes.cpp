// compile on MacOs:
// c++ -I /opt/homebrew/include -L /opt/homebrew/lib -o glfw_multiple_shapes -lglfw -lglew -framework OpenGL -DGL_SILENCE_DEPRECATION glfw_multiple_shapes.cpp

// compile on Windows:
// g++ -o glfw_multiple_shapes.exe glfw_multiple_shapes.cpp -lglfw3 -lopengl32 -lglew32

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
    GLFWwindow* window = glfwCreateWindow(640, 480, "Rotating shapes", NULL, NULL);
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

    // Compile vertex shader
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const char *vsSource = R"(
        #version 330 core
        in vec3 pos;
        in vec3 nor;
        out vec3 normal;
        out vec3 fragPos;
        uniform mat4 projection;
        uniform mat4 translation;
        uniform mat4 rotation;
        uniform float rotationAngle;
        void main() {
            float s = sin(rotationAngle);
            float c = cos(rotationAngle);
            mat4 rotationY = mat4(c, 0, -s, 0,
                                  0, 1,  0, 0,
                                  s, 0,  c, 0,
                                  0, 0,  0, 1); // Rotation matrix in column-major order
            gl_Position = projection * translation * rotation * rotationY * vec4(pos, 1.0);
            normal = mat3(rotation * rotationY) * nor; // We don't want translation or projection for the normal
            fragPos = vec3(translation * rotation * rotationY * vec4(pos, 1.0)); // Position in 3D space before projection
        }
        )";
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
    const char *fsSource = R"(
        #version 330 core
        in vec3 normal;
        in vec3 fragPos;
        uniform vec3 color;
        out vec4 fragColor = vec4(color, 1.0f);
        float ambient = 0.5f;
        vec3 lightPos = vec3(0.5f, 2.0f, 0.0f);
        void main() {
            vec3 normalizedNormal = normalize(normal);
            vec3 reverseLightDirection = normalize(lightPos - fragPos);
            float diffuse = 0.5f * max(0.0f, dot(normalizedNormal, reverseLightDirection));
            fragColor.rgb *= ambient + diffuse;
        }
        )";
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
    GLfloat verticesCube[] = { // coordinates and normals
         0.5,  0.5,  0.5, 0, 0, 1, // front top right
         0.5, -0.5,  0.5, 0, 0, 1, // front bottom right
        -0.5, -0.5,  0.5, 0, 0, 1, // front bottom left
        -0.5, -0.5,  0.5, 0, 0, 1, // front bottom left
        -0.5,  0.5,  0.5, 0, 0, 1, // front top left
         0.5,  0.5,  0.5, 0, 0, 1, // front top right

         0.5,  0.5, -0.5, 0, 0, -1, // back top right
         0.5, -0.5, -0.5, 0, 0, -1, // back bottom right
        -0.5, -0.5, -0.5, 0, 0, -1, // back bottom left
        -0.5, -0.5, -0.5, 0, 0, -1, // back bottom left
        -0.5,  0.5, -0.5, 0, 0, -1, // back top left
         0.5,  0.5, -0.5, 0, 0, -1, // back top right

        0.5,  0.5,  0.5, 1, 0, 0, // front top right
        0.5, -0.5,  0.5, 1, 0, 0, // front bottom right
        0.5, -0.5, -0.5, 1, 0, 0, // back bottom right
        0.5, -0.5, -0.5, 1, 0, 0, // back bottom right
        0.5,  0.5, -0.5, 1, 0, 0, // back top right
        0.5,  0.5,  0.5, 1, 0, 0, // front top right

        -0.5,  0.5,  0.5, -1, 0, 0, // front top left
        -0.5, -0.5,  0.5, -1, 0, 0, // front bottom left
        -0.5, -0.5, -0.5, -1, 0, 0, // back bottom left
        -0.5, -0.5, -0.5, -1, 0, 0, // back bottom left
        -0.5,  0.5, -0.5, -1, 0, 0, // back top left
        -0.5,  0.5,  0.5, -1, 0, 0, // front top left

         0.5,  0.5,  0.5, 0, 1, 0, // front top right
        -0.5,  0.5,  0.5, 0, 1, 0, // front top left
        -0.5,  0.5, -0.5, 0, 1, 0, // back top left
        -0.5,  0.5, -0.5, 0, 1, 0, // back top left
         0.5,  0.5, -0.5, 0, 1, 0, // back top right
         0.5,  0.5,  0.5, 0, 1, 0, // front top right

         0.5, -0.5,  0.5, 0, -1, 0, // front bottom right
        -0.5, -0.5,  0.5, 0, -1, 0, // front bottom left
        -0.5, -0.5, -0.5, 0, -1, 0, // back bottom left
        -0.5, -0.5, -0.5, 0, -1, 0, // back bottom left
         0.5, -0.5, -0.5, 0, -1, 0, // back bottom right
         0.5, -0.5,  0.5, 0, -1, 0  // front bottom right
    };

    // Vertices for pyramid
    GLfloat verticesPyramid[] = { // coordinates and normals
         0.5, -0.5,  0.5,  0, -1,  0, // front bottom right
        -0.5, -0.5,  0.5,  0, -1,  0, // front bottom left
        -0.5, -0.5, -0.5,  0, -1,  0, // back bottom left
        -0.5, -0.5, -0.5,  0, -1,  0, // back bottom left
         0.5, -0.5, -0.5,  0, -1,  0, // back bottom right
         0.5, -0.5,  0.5,  0, -1,  0, // front bottom right

         0.5, -0.5,  0.5,  2,  1,  0, // front bottom right
         0.5, -0.5, -0.5,  2,  1,  0, // back bottom right
         0,    0.5,  0,    2,  1,  0, // top

         0.5, -0.5,  0.5,  0,  1,  2, // front bottom right
        -0.5, -0.5,  0.5,  0,  1,  2, // front bottom left
         0,    0.5,  0,    0,  1,  2, // top

        -0.5, -0.5,  0.5, -2,  1,  0, // front bottom left
        -0.5, -0.5, -0.5, -2,  1,  0, // back bottom left
         0,    0.5,  0,   -2,  1,  0, // top

         0.5, -0.5, -0.5,  0,  1, -2, // back bottom right
        -0.5, -0.5, -0.5,  0,  1, -2, // back bottom left
         0,    0.5,  0,    0,  1, -2  // top
    };

    // Create two Vertex Array Objects
    GLuint vao[2];
    glGenVertexArrays(2, vao);

    // Bind the first VAO
    glBindVertexArray(vao[0]);

    // Create 2 Vertex Buffer Objects
    GLuint vbo[2];
    glGenBuffers(2, vbo);
    
    // Bind the first VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    
    // Fill the first VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCube), verticesCube, GL_STATIC_DRAW);
        
    // Get the location of the position attribute and enable it
    GLint posLocation = glGetAttribLocation(program, "pos");
    glEnableVertexAttribArray(posLocation);

    // Positions using 3 floats per vertex, stride of 24 bytes (6 32-bit floats)
    glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 24, 0);

    // Get the location of the normal vector attribute and enable it
    GLint normalLocation = glGetAttribLocation(program, "nor");
    glEnableVertexAttribArray(normalLocation);

    // Normals using 3 floats per vertex, stride of 24 bytes (6 32-bit floats), offset of 12 bytes
    glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 24, (GLvoid *)12);

    // Bind the second VAO
    glBindVertexArray(vao[1]);

    // Bind the second VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    
    // Fill the second VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPyramid), verticesPyramid, GL_STATIC_DRAW);

    // Enable the position attribute array also for the second vao
    glEnableVertexAttribArray(posLocation);

    // Positions using 3 floats per vertex, stride of 24 bytes (6 32-bit floats)
    glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 24, 0);

    // Enable the normal vector attribute array also for the second vao
    glEnableVertexAttribArray(normalLocation);

    // Normals using 3 floats per vertex, stride of 24 bytes (6 32-bit floats), offset of 12 bytes
    glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 24, (GLvoid *)12);

    // Set the background color
    glClearColor(0.1, 0.1, 0.1, 1); // r, g, b, alpha

    // Perspective projection matrix
    GLint projectionLocation = glGetUniformLocation(program, "projection");
    GLfloat projection[] = {1, 0,  0,  0,
                            0, 1,  0,  0,
                            0, 0, -1, -1,
                            0, 0, -1,  0};
    glUniformMatrix4fv(projectionLocation, 1, GL_TRUE, projection);

    // Translation matrices; move everything 3 units to the back and 1 to either the left or the right
    GLint translationLocation = glGetUniformLocation(program, "translation");
    GLfloat translationLeft[] = {1, 0, 0, -1,
                                 0, 1, 0,  0,
                                 0, 0, 1, -3,
                                 0, 0, 0,  1};
    GLfloat translationRight[] = {1, 0, 0,  1,
                                  0, 1, 0,  0,
                                  0, 0, 1, -3,
                                  0, 0, 0,  1};

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

    // Get location of color uniform
    GLint colorLocation = glGetUniformLocation(program, "color");

    // Enable z-buffer test
    glEnable(GL_DEPTH_TEST);

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

        // Bind the first VAO
        glBindVertexArray(vao[0]);

        // Specify the translation matrix
        glUniformMatrix4fv(translationLocation, 1, GL_TRUE, translationLeft);

        // Specify the color
        glUniform3f(colorLocation, 0, 0, 1); // blue

        // Tell opengl to draw the vertices in the VBO as a series of triangles
        glDrawArrays(GL_TRIANGLES, 0, sizeof(verticesCube) / sizeof(GLfloat) / 6);

        // Bind the second VAO
        glBindVertexArray(vao[1]);

        // Specify the translation matrix
        glUniformMatrix4fv(translationLocation, 1, GL_TRUE, translationRight);

        // Specify the color
        glUniform3f(colorLocation, 0, 1, 0); // green

        // Tell opengl to draw the vertices in the VBO as a series of triangles
        glDrawArrays(GL_TRIANGLES, 0, sizeof(verticesPyramid) / sizeof(GLfloat) / 6);

        // Swap buffers
        glfwSwapBuffers(window);

        // Check for keypress
        glfwPollEvents();
    }
    
    // Terminate the GLFW library
    glfwTerminate();
    return 0;
}