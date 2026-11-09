#include "camera.hpp"
#include <cmath>
#include <vector>

// Afstand tussen oog en wereldoorsprong; moet gelijk zijn aan de
// translatie(0, 0, -3) in view().
static const float KIJKAFSTAND = 3.0f;

// Zet een Matrix (resultaat van operator*) om naar Matrix4 via een
// vector<float>(16), rij-major.
static Matrix4 naarMatrix4(const Matrix &m) {
    std::vector<float> waarden(16);
    for (int r = 0; r < 4; r++)
        for (int k = 0; k < 4; k++)
            waarden[r * 4 + k] = m.at(r, k);
    return Matrix4(waarden);
}

// Orthografische matrix (zelfde z-afbeelding als glOrtho), rij-major.
// hb = halve breedte, hh = halve hoogte van het zichtbare volume.
static Matrix4 orthoMatrix(float hb, float hh, float n, float f) {
    std::vector<float> w = {
        1.0f / hb, 0,         0,                  0,
        0,         1.0f / hh, 0,                  0,
        0,         0,         -2.0f / (f - n),    -(f + n) / (f - n),
        0,         0,         0,                  1
    };
    return Matrix4(w);
}

Camera::Camera(const Vector &positie, float fov, float aspect, float nearZ, float farZ,
               float beginYaw, float beginPitch)
    : positie(positie), fov(fov), aspect(aspect), nearZ(nearZ), farZ(farZ),
      yawHoek(beginYaw), pitchHoek(beginPitch) {
    draai(0, 0);   // begrenst de pitch
}

void Camera::setPositie(const Vector &p) { positie = p; }
Vector Camera::getPositie() const { return positie; }

Matrix4 Camera::projectie() const {
    float n = nearZ;
    float f = farZ;

    // Gemeenschappelijke "pixel-schaal" voor de niet-perspectief modi:
    // het zichtbare volume is op kijkafstand even groot als bij perspectief,
    // zodat het beeld bij wisselen van modus ongeveer even groot blijft.
    float halveHoogte  = KIJKAFSTAND * std::tan(fov / 2.0f);
    float halveBreedte = halveHoogte * aspect;

    // NB: de klassieke lesmatrices voor ortho/isometrisch zetten z op 0.
    // Hier blijft de z-component behouden (zoals glOrtho), zodat
    // GL_DEPTH_TEST blijft werken: anders krijgt elk fragment dezelfde
    // diepte en bepaalt de tekenvolgorde wat zichtbaar is.
    switch (projType) {
        case PROJ_ORTHO: {
            return orthoMatrix(halveBreedte, halveHoogte, n, f);
        }
        case PROJ_PARALLEL: {
            // Oblique (cabinet) projectie: schuifvlak van x en y als functie
            // van de diepte, rond het vlak z = -KIJKAFSTAND. Diepte 0.5 onder 45 graden.
            float k = 0.5f * std::cos(0.785398163f);
            float shx = -k, shy = -k;
            std::vector<float> s = {
                1, 0, shx, shx * KIJKAFSTAND,
                0, 1, shy, shy * KIJKAFSTAND,
                0, 0, 1,   0,
                0, 0, 0,   1
            };
            Matrix m = orthoMatrix(halveBreedte, halveHoogte, n, f) * Matrix4(s);
            return naarMatrix4(m);
        }
        case PROJ_ISOMETRISCH: {
            // 45 graden om Y, daarna ca. 35,264 graden om X, om het punt
            // (0, 0, -KIJKAFSTAND) zodat de scene in beeld blijft.
            float hoekY = 0.785398163f;
            float hoekX = std::atan(1.0f / std::sqrt(2.0f));
            Matrix4 naarMidden = Transformatie::translatie(0, 0, -KIJKAFSTAND).naarMatrix();
            Matrix4 rotX = Transformatie::rotatie(hoekX, X_AS).naarMatrix();
            Matrix4 rotY = Transformatie::rotatie(hoekY, Y_AS).naarMatrix();
            Matrix4 terugNaarOorsprong = Transformatie::translatie(0, 0, KIJKAFSTAND).naarMatrix();
            Matrix m = orthoMatrix(halveBreedte, halveHoogte, n, f) * naarMidden * rotX * rotY * terugNaarOorsprong;
            return naarMatrix4(m);
        }
        case PROJ_PERSPECTIEF:
        default:
            break;
    }

    float t = 1.0f / std::tan(fov / 2.0f);

    // Rij-major. Let op: (2,3) = (2fn)/(n-f) en (3,2) = -1.
    return Matrix4({
        t / aspect, 0,  0,                          0,
        0,          t,  0,                          0,
        0,          0,  (f + n) / (n - f),          (2 * f * n) / (n - f),
        0,          0,  -1,                         0
    });
}


Matrix4 Camera::view() const {
    Matrix naarOog = Transformatie::translatie(-positie.at(0), -positie.at(1), -positie.at(2)).naarMatrix();
    Matrix pitch   = Transformatie::rotatie(pitchHoek, X_AS).naarMatrix();
    Matrix yaw     = Transformatie::rotatie(yawHoek, Y_AS).naarMatrix();
    Matrix terug   = Transformatie::translatie(0, 0, -3).naarMatrix();

    Matrix m = naarOog * pitch * yaw * terug;

    std::vector<float> waarden(16);
    for (int r = 0; r < 4; r++)
        for (int k = 0; k < 4; k++)
            waarden[r * 4 + k] = m.at(r, k);

    return Matrix4(waarden);
}

Vector Camera::oogWereld() const {
    // view = T(-p) * Rx(pitch) * Ry(yaw) * T(0,0,-3)
    // inverse = T(0,0,3) * Ry(-yaw) * Rx(-pitch) * T(p); toegepast op de oorsprong.
    Matrix m = Transformatie::translatie(0, 0, KIJKAFSTAND).naarMatrix()
             * Transformatie::rotatie(-yawHoek, Y_AS).naarMatrix()
             * Transformatie::rotatie(-pitchHoek, X_AS).naarMatrix()
             * Transformatie::translatie(positie.at(0), positie.at(1), positie.at(2)).naarMatrix();
    return Vector({m.at(0, 3), m.at(1, 3), m.at(2, 3)});
}


Matrix4 Camera::compleet() const {
    Matrix m = projectie() * view();
    Matrix4 uit;
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            uit.set(r, c, m.at(r, c));
    return uit;
}

Matrix4 Camera::zonderProjectie() const {
    return view();
}

Matrix4 Camera::zonderTranslatieEnProjectie() const {
    Matrix pitch = Transformatie::rotatie(pitchHoek, X_AS).naarMatrix();
    Matrix yaw   = Transformatie::rotatie(yawHoek, Y_AS).naarMatrix();
    Matrix m = pitch * yaw;
    return naarMatrix4(m);
}

void Camera::beweeg(float dx, float dy, float dz) {
    positie.set(0, positie.at(0) + dx);
    positie.set(1, positie.at(1) + dy);
    positie.set(2, positie.at(2) + dz);
}

void Camera::draai(float dyaw, float dpitch) {
    yawHoek   += dyaw;
    pitchHoek += dpitch;
    // voorkom over de kop slaan
    if (pitchHoek >  1.5f) pitchHoek =  1.5f;
    if (pitchHoek < -1.5f) pitchHoek = -1.5f;
}

void Camera::setOrientatie(float nieuweYaw, float nieuwePitch) {
    yawHoek = nieuweYaw;
    pitchHoek = nieuwePitch;
    draai(0, 0);   // begrenst de pitch
}
