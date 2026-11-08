#pragma once
#include "vector.hpp"

class Lightsource {
private:
    Vector positie;
    float kleur[3];

public:
    Lightsource(const Vector &positie, float r, float g, float b);

    void setPositie(const Vector &p);
    Vector getPositie() const;

    void setKleur(float r, float g, float b);
    float getKleur(int i) const;
};
