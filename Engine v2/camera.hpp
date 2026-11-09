#pragma once
#include "matrix.hpp"
#include "vector.hpp"

// Soort projectie die Camera::projectie() teruggeeft.
enum ProjectieType { PROJ_ORTHO, PROJ_PARALLEL, PROJ_ISOMETRISCH, PROJ_PERSPECTIEF };

class Camera {
private:
    Vector positie;      // oogpunt in de wereld
    float fov;           // gezichtsveld in radialen
    float aspect;        // breedte / hoogte
    float nearZ, farZ;
    float yawHoek = 0.0f;
    float pitchHoek = 0.0f;
    ProjectieType projType = PROJ_PERSPECTIEF;   // perspectief is de standaard

public:
    // beginYaw en beginPitch (radialen) bepalen de beginoriëntatie.
    Camera(const Vector &positie, float fov, float aspect, float nearZ, float farZ,
           float beginYaw = 0.0f, float beginPitch = 0.0f);

    void setPositie(const Vector &p);
    Vector getPositie() const;

    void setProjectieType(ProjectieType t) { projType = t; }
    ProjectieType getProjectieType() const { return projType; }

    Matrix4 projectie() const;              // afhankelijk van ProjectieType
    Matrix4 view() const;                   // view-matrix (zie camera.cpp)
    Matrix4 compleet() const;               // projectie * view
    Matrix4 zonderProjectie() const;        // alleen view
    Matrix4 zonderTranslatieEnProjectie() const;  // alleen rotatie

    void beweeg(float dx, float dy, float dz);
    void draai(float dyaw, float dpitch);
    void setOrientatie(float nieuweYaw, float nieuwePitch);   // absoluut, in radialen
    Vector oog() const { return positie; }

    // Positie van het oog in wereldcoordinaten (inverse van view() op de
    // oorsprong). Nodig voor de specular-berekening in de shader.
    Vector oogWereld() const;

    float yaw() const { return yawHoek; }
    float pitch() const { return pitchHoek; }
};
