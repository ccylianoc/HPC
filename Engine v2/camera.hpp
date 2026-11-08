#pragma once
#include "matrix.hpp"
#include "vector.hpp"

class Camera {
private:
    Vector positie;      // oogpunt in de wereld
    float fov;           // gezichtsveld in radialen
    float aspect;        // breedte / hoogte
    float nearZ, farZ;
    float yawHoek = 0.0f;
    float pitchHoek = 0.0f;

public:
    Camera(const Vector &positie, float fov, float aspect, float nearZ, float farZ);

    void setPositie(const Vector &p);
    Vector getPositie() const;

    Matrix4 projectie() const;              // perspectief
    Matrix4 view() const;                   // alleen translatie naar het oog
    Matrix4 compleet() const;               // projectie * view
    Matrix4 zonderProjectie() const;        // alleen view
    Matrix4 zonderTranslatieEnProjectie() const;  // alleen rotatie

    void beweeg(float dx, float dy, float dz);
    void draai(float dyaw, float dpitch);
    Vector oog() const { return positie; }
    float yaw() const { return yawHoek; }
    float pitch() const { return pitchHoek; }
};
