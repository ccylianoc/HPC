#include "transformatie.hpp"
#include <cmath>
#include <vector>

Transform::Transform()
    : sx(1), sy(1), sz(1),
      tx(0), ty(0), tz(0),
      rx(0), ry(0), rz(0) {}

void Transform::setSchaal(float x, float y, float z) { sx = x; sy = y; sz = z; }
void Transform::setSchaal(float s) { setSchaal(s, s, s); }

void Transform::setTranslatie(float x, float y, float z) { tx = x; ty = y; tz = z; }

void Transform::setRotatie(float x, float y, float z) { rx = x; ry = y; rz = z; }

void Transform::schaal(float x, float y, float z) { sx *= x; sy *= y; sz *= z; }
void Transform::transleer(float x, float y, float z) { tx += x; ty += y; tz += z; }
void Transform::roteer(float x, float y, float z) { rx += x; ry += y; rz += z; }

Matrix4 Transform::model() const {
    float cx = std::cos(rx), sxr = std::sin(rx);
    float cy = std::cos(ry), syr = std::sin(ry);
    float cz = std::cos(rz), szr = std::sin(rz);

    // R = Rz * Ry * Rx
    float r00 = cz * cy;
    float r01 = cz * syr * sxr - szr * cx;
    float r02 = cz * syr * cx + szr * sxr;
    float r10 = szr * cy;
    float r11 = szr * syr * sxr + cz * cx;
    float r12 = szr * syr * cx - cz * sxr;
    float r20 = -syr;
    float r21 = cy * sxr;
    float r22 = cy * cx;

    // Model = T * R * S, kolom-major zoals OpenGL verwacht.
    std::vector<float> m = {
        r00 * sx,  r10 * sx,  r20 * sx,  0.0f,
        r01 * sy,  r11 * sy,  r21 * sy,  0.0f,
        r02 * sz,  r12 * sz,  r22 * sz,  0.0f,
        tx,        ty,        tz,        1.0f
    };

    return Matrix4(m);
}
