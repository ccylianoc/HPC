#include "camera.hpp"

Camera::Camera(const Vector &positie, float fov, float aspect, float nearZ, float farZ)
    : positie(positie), fov(fov), aspect(aspect), nearZ(nearZ), farZ(farZ) {}

void Camera::setPositie(const Vector &p) { positie = p; }
Vector Camera::getPositie() const { return positie; }

Matrix4 Camera::projectie() const {
    float zp = fov;
    return Matrix4({
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, -zp, -1,
        0, 0, -zp, 0
    });
}

Matrix4 Camera::view() const {
    return Transformatie::translatie(-positie.at(0), -positie.at(1), -positie.at(2)).naarMatrix();
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
