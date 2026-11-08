#include "camera.hpp"

Camera::Camera(const Vector &positie, float fov, float aspect, float nearZ, float farZ)
    : positie(positie), fov(fov), aspect(aspect), nearZ(nearZ), farZ(farZ) {}

void Camera::setPositie(const Vector &p) { positie = p; }
Vector Camera::getPositie() const { return positie; }

Matrix4 Camera::projectie() const {
    // Perspectief met projectievlak op z = -zp, oog in de oorsprong.
    // Rij-major, zoals Matrix4 verwacht; naarGL() transponeert.
    float zp = 1.0f;
    return Matrix4({
        zp, 0,  0,  0,
        0,  zp, 0,  0,
        0,  0,  zp, 0,
        0,  0,  1,  0
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
    return Matrix4::eenheid();
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
