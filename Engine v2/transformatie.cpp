#include "transformatie.hpp"

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
    Matrix4 schaling   = Transformatie::schaling(sx, sy, sz).naarMatrix();
    Matrix4 rotatieX   = Transformatie::rotatie(rx, X_AS).naarMatrix();
    Matrix4 rotatieY   = Transformatie::rotatie(ry, Y_AS).naarMatrix();
    Matrix4 rotatieZ   = Transformatie::rotatie(rz, Z_AS).naarMatrix();
    Matrix4 translatie = Transformatie::translatie(tx, ty, tz).naarMatrix();

    return translatie * rotatieZ * rotatieY * rotatieX * schaling;
}
