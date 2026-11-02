#include "Engine/draw.h"
#include "m.hpp"
#include "v.hpp"
#include <vector>
#include <math.h>

/*to compile:

g++ kubus.cpp m.cpp v.cpp draw.cpp -o kubus.exe -lglfw3 -lopengl32 -lgdi32
.\kubus.exe

*/

struct edge {
    int v1;
    int v2;
};

void tekenKubus(Draw &d) {
    std::vector<Vector> vertices = {
        Vector({ 1,  1,  1, 1}),
        Vector({ 1, -1,  1, 1}),
        Vector({-1, -1,  1, 1}),
        Vector({-1,  1,  1, 1}),
        Vector({ 1,  1, -1, 1}),
        Vector({ 1, -1, -1, 1}),
        Vector({-1, -1, -1, 1}),
        Vector({-1,  1, -1, 1})
    };

    std::vector<edge> edges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    float pi = 3.14159265f;

    float zp = -1.0f;
    Matrix4 P({zp,  0,  0, 0,
                0, zp,  0, 0,
                0,  0, zp, 0,
                0,  0,  1, 0});

    Matrix4 S({300, 0,   0, 0,
               0,   300, 0, 0,
               0,   0,   1, 0,
               0,   0,   0, 1});

    Matrix4 T({1, 0, 0, 400,
               0, 1, 0, 300,
               0, 0, 1, 0,
               0, 0, 0, 1});

    Matrix4 T_achteruit({1, 0, 0, 0,
                         0, 1, 0, 0,
                         0, 0, 1, -3,
                         0, 0, 0, 1});

    Transformatie rx = Transformatie::rotatie(30.0f * pi / 180.0f, X_AS);
    Matrix4 R_x = rx.naarMatrix();

    Transformatie ry = Transformatie::rotatie(d.getTime(), Y_AS);
    Matrix4 R_y = ry.naarMatrix();

    Matrix gecombineerd = T * S * P * T_achteruit * R_x * R_y;

    for (const edge &e : edges) {
        Vector a = gecombineerd.naarCartesisch(gecombineerd * vertices[e.v1]);
        Vector b = gecombineerd.naarCartesisch(gecombineerd * vertices[e.v2]);
        d.line(a.at(0), a.at(1), b.at(0), b.at(1));
    }
}


int main() {
    Draw d(800, 600, "Kubus", tekenKubus);
    return 0;
}

