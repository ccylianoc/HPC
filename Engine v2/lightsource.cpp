#include "lightsource.hpp"

Lightsource::Lightsource(const Vector &positie, float r, float g, float b)
    : positie(positie) {
    kleur[0] = r; kleur[1] = g; kleur[2] = b;
}

void Lightsource::setPositie(const Vector &p) { positie = p; }
Vector Lightsource::getPositie() const { return positie; }

void Lightsource::setKleur(float r, float g, float b) {
    kleur[0] = r; kleur[1] = g; kleur[2] = b;
}

float Lightsource::getKleur(int i) const { return kleur[i]; }
