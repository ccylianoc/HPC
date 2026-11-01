# Matrix en Vector libraries ---> Graphics Engine

Voor High Preformance Computing moeten wij onze eigen vector en matrix library schrijven. Het idee is dat we deze gebruiken om verschillende vormen en perspectieven kunnen weergeven voor onze graphics engine.

Gekregen van school hebben we de draw.h en draw.cpp

Voor vector pakken we de bestaande vector library en overriden we deze.

Voor matrix willen we matrixen van (n x m) kunnen maken en (n x n) tot 4x4

We willen ook vectoren en matrixen opereren met elkaar hiervoor moeten we ook gebruik maken van een homogeen coordinaat, dus als we een standaard vector van 2x1 hebben moet deze omgezet worden in een matrix van 3x1 met een homogeen coordinaat 1. gebruiken we een 2x1 vector met een 2x2 matrix zou dit niet nodig moeten zijn maar omdat we met graphics werken gaan we deze bijna niet gebruiken.

In matrix willen we een parent class Matrix dit moet de basis bevatten voor alle matix dan voor alle child classen dus MatrixN Matrix3 en Matrix4 willen we ook eenheids matrices toevoegen en extra dingen die we nodig hebben

Dan hebben we in matrix ook een class Transformatie die alle verschillende transformaties (schalen, roteren, transleren en perspectief) als enum heeft en ook alle parameters mee geeft aan de nieuwe operatoren van de transformaties. Dus per transformatie moet het ook de juiste transformatie matrices hebben om toe te passen. Hier moeten wij ook een parameter aangeven om te schalen met i of roteren etc. 

```draw.h
#include <GLFW/glfw3.h>

class Draw {
    public:
    // Creates a window of the given width, height and title and calls the given drawing function 
    Draw(int width, int height, const char *windowTitle, void func(Draw &d));
  
    // Draws a point at coordinates (x, y)
    void point(float x, float y);
  
    // Draws a line from coordinates (x1, y1) to (x2, y2)
    void line(float x1, float y1, float x2, float y2);
  
    // Sets the drawing color; components range from 0 to 1
    void color(float r, float g, float b);
  
    // Returns the time in seconds since starting the program
    double getTime();

    private:
    // Callback functions cannot be class instance function so must be static
    static void keyCallBack(GLFWwindow *window, int key, int scancode, int action, int mods);
};
```

```draw.cpp
#include <stdlib.h>
#include "draw.h"

Draw::Draw(int width, int height, const char *windowTitle, void func(Draw &d)) {
    // Initialize the GLFW library
    glfwInit();

    // Create a window
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Synchronize buffer swap interval with refresh rate
    glfwSwapInterval(1);

    // Set the callback function for keypresses
    glfwSetKeyCallback(window, keyCallBack);

    // Define orthographic projection
    glOrtho(0, width, 0, height, -1, 1);

    // Set point size
    glPointSize(4);

    // Set line width
    glLineWidth(2);
  
    // Main loop
    while (!glfwWindowShouldClose(window)) {

        // Clear the window
        glClear(GL_COLOR_BUFFER_BIT);

        // Set default color
        glColor3f(1, 1, 1);

        // Call drawing function
        func(*this);

        // Show buffer
        glfwSwapBuffers(window);

        // Check for events (window closing)
        glfwPollEvents();
    }

    // Terminate the GLFW library
    glfwTerminate();
}

void Draw::point(float x, float y) {
    glBegin(GL_POINTS);
    glVertex2f(x, y);
    glEnd();
}

void Draw::line(float x1, float y1, float x2, float y2) {
    // Draw a line
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void Draw::color(float r, float g, float b) {
    glColor3f(r, g, b);
}

double Draw::getTime() {
    return glfwGetTime();
}

// Exit when a key is pressed
void Draw::keyCallBack(GLFWwindow *window, int key, int scancode, int action, int mods) {
    glfwTerminate();
    exit(0);
}
```

```m.hpp
// (Dit wordt de nieuwe matrix.hpp)
// Include alle nodige headers
// Include vector.hpp 
#include "v.hpp"
#include <array>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

// class Matrix (Parent class voor MatrixN, Matrix 3 en Matrix4)
/*
    private:
        int rij, kol;
        std::vector<float> m;
    public:
        Matrix(int rij, int kol);
        Matrix(int rij, int kol, const std::vector<float> &waarden);
        int rijen() const;
        int kolommen() const;
        void set(int rij, int kol, float waarde);
        float at(int rij, int kol) const;
        Matrix operator*(const Matrix &andere) const;
        Vector operator*(const Vector &v) const;
        Matrix getransponeerd() const;
        Matrix operator+(const Matrix &andere) const;
        Matrix operator-(const Matrix &andere) const;
        bool operator==(const Matrix &andere) const;
        bool operator!=(const Matrix &andere) const;
        float determinant() const;
        Matrix inverse() const;

*/
/*
    class MatrixN : public Matrix{
        
    }
*/

class Matrix{
    private:
        int rij, kol;
        std::vector<float> m;
    public:
        Matrix(int rij, int kol);
        Matrix(int rij, int kol, const std::vector<float> &waarden);
        int rijen() const;
        int kolommen() const;
        void set(int rij, int kol, float waarde);
        float at(int rij, int kol) const;
        Matrix operator*(const Matrix &andere) const;
        Vector operator*(const Vector &v) const;
        Matrix getransponeerd() const;
        Matrix operator+(const Matrix &andere) const;
        Matrix operator-(const Matrix &andere) const;
        bool operator==(const Matrix &andere) const;
        bool operator!=(const Matrix &andere) const;
        float determinant() const;
        Matrix inverse() const;
};

class MatrixN : public Matrix{

};


// class Transformatie : public Matrix (hier in de transformaties: Rotatie, Translatie, Schaling, Perspectief)
/*
    private:
        enum TransformatieType {ROTATIE, TRANSLATIE, SCHALING, PERSPECTIEF};
        TransformatieType type;
        enum AsType {X_AS, Y_AS, Z_AS};
    public:
        Transformatie(TransformatieType type, float waarde, AsType as = X_AS);
        static Transformatie rotatie(float hoek, AsType as);
        static Transformatie translatie(float dx, float dy, float dz);
        static Transformatie schaling(float sx, float sy, float sz);
        static Transformatie perspectief(float fovy, float aspect, float near, float far);
        
*/
```
```v.hpp
// (dit wordt de nieuwe vector.hpp)
/* Include alle nodige headers 
#include <vector>
#include <cmath>
#include <iostream>
#include <stdexcept>

*/
#include <vector>
#include <cmath>
#include <iostream>
#include <stdexcept>

/* class Vector (2D tot en met 4D) 
    private:
        std::vector<float> v;
    public:
        Vector(int n);
        Vector(const std::vector<float> &data);
        int size() const;
        float at(int i) const;
        void set(int i, float waarde);
        float length() const;
        Vector normalized() const;
        Vector operator+(const Vector &other) const;
        Vector operator-(const Vector &other) const;
        Vector operator*(float s) const;
        Vector operator/(float s) const;
        float dot(const Vector &other) const;
        Vector cross(const Vector &other) const;
        bool operator==(const Vector &other) const;
        bool operator!=(const Vector &other) const;
        friend std::ostream &operator<<(std::ostream &os, const Vector &vec);

*/

class Vector {
    private:
        std::vector<float> v;

    public:
        Vector(int n);
        Vector(const std::vector<float> &data);

        int size() const;
        float at(int i) const;
        void set(int i, float waarde);

        float length() const{
            float som = 0.0f;
            for (float c : v) som += c * c;
            return std::sqrt(som);
        }

        Vector normalized() const{
            float len = length();
            if (len == .0f) return Vector(v.size());
            Vector result(v.size());
            for (int i = 0; i < v.size(); i++) result.v[i] = v[i] / len;
            return result;
        }

        Vector operator+(const Vector &other) const{
            Vector result(v.size());
            for (int i = 0; i < v.size(); i++) result.v[i] = v[i] + other.v[i];
            return result;
        }
        Vector operator-(const Vector &other) const{
            Vector result(v.size());
            for (int i = 0; i < v.size(); i++) result.v[i] = v[i] - other.v[i];
            return result;
        }
        Vector operator*(float s) const{
            Vector result(v.size());
            for (int i = 0; i < v.size(); i++) result.v[i] = v[i] * s;
            return result;
        }
        Vector operator/(float s) const{
            Vector result(v.size());
            for (int i = 0; i < v.size(); i++) result.v[i] = v[i] / s;
            return result;
        }

        float dot(const Vector &other) const{
            float som = .0f;
            for (int i = 0; i < v.size(); i++) som += v[i] * other.v[i];
            return som;
        }

        Vector cross(const Vector &other) const{
            if (v.size() != 3 || other.v.size() != 3) {
                throw std::invalid_argument("Cross product is only defined for 3D vectors.");
            }
            return Vector({v[1] * other.v[2] - v[2] * other.v[1],
                           v[2] * other.v[0] - v[0] * other.v[2],
                           v[0] * other.v[1] - v[1] * other.v[0]});
        }

        bool operator==(const Vector &other) const{
            if (v.size() != other.v.size()) return false;
            for (int i = 0; i < v.size(); i++) {
                if (std::fabs(v[i] - other.v[i]) > 1e-6f) return false;
            }
            return true;
        }

        bool operator!=(const Vector &other) const { return !(*this == other); }

        friend std::ostream &operator<<(std::ostream &os, const Vector &vec) {
            os << "(";
            for (int i = 0; i < vec.v.size(); i++) {
                os << vec.v[i];
                if (i < vec.v.size() - 1) os << ", ";
            }
            os << ")";
            return os;
        }

};
```

Dit is alles wat ik tot nu toe heb, kan je uitleggen wat ik kan veranderen en het even een stappen plan voor mij maken van wat ik kan doen (met tussentijdse test) zodat ik dit succecvol kan maken, het snap en ook een goed eindproduct heb voor school