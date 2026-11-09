#include "transformatie.hpp"
#include <cmath>
#include <vector>
#include <cstdio>

Transform::Transform()
    : sx(1), sy(1), sz(1),
      tx(0), ty(0), tz(0),
      rx(0), ry(0), rz(0) {}

// Alle setters verhogen de versie alleen als de waarde echt verandert.
void Transform::setSchaal(float x, float y, float z) {
    if (x == sx && y == sy && z == sz) return;
    sx = x; sy = y; sz = z; versie_++;
}
void Transform::setSchaal(float s) { setSchaal(s, s, s); }

void Transform::setTranslatie(float x, float y, float z) {
    if (x == tx && y == ty && z == tz) return;
    tx = x; ty = y; tz = z; versie_++;
}

void Transform::setRotatie(float x, float y, float z) {
    if (x == rx && y == ry && z == rz) return;
    rx = x; ry = y; rz = z; versie_++;
}

void Transform::schaal(float x, float y, float z) { setSchaal(sx * x, sy * y, sz * z); }
void Transform::transleer(float x, float y, float z) { setTranslatie(tx + x, ty + y, tz + z); }
void Transform::roteer(float x, float y, float z) { setRotatie(rx + x, ry + y, rz + z); }

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

    // Model = T * R * S, rij-major (zoals Matrix4 verwacht).
    // Rij 0: R-rij 0 geschaald met sx, sy, sz
    std::vector<float> m = {
        r00 * sx, r01 * sy, r02 * sz, tx,
        r10 * sx, r11 * sy, r12 * sz, ty,
        r20 * sx, r21 * sy, r22 * sz, tz,
        0.0f,     0.0f,     0.0f,     1.0f
    };

    return Matrix4(m);
}
