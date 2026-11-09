#include "properties.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdexcept>

// Alle waarden van een eigenschap horen tussen 0 en 1 te liggen.
static void controleerBereik(float waarde, const char *naam) {
    if (waarde < 0.0f || waarde > 1.0f) {
        throw std::invalid_argument(std::string("Waarde buiten 0..1 [") + naam + "]");
    }
}

// Kleur

Kleur::Kleur(float r, float g, float b) : r(r), g(g), b(b) {
    controleerBereik(r, "Kleur");
    controleerBereik(g, "Kleur");
    controleerBereik(b, "Kleur");
}

void Kleur::apply(Materiaal &m) const {
    m.kleur[0] = r; m.kleur[1] = g; m.kleur[2] = b;
}

// Opaciteit

Opaciteit::Opaciteit(float alpha) : alpha(alpha) {
    controleerBereik(alpha, "Opaciteit");
}

void Opaciteit::apply(Materiaal &m) const {
    m.alpha = alpha;
}

// Ambient

AmbientCoefficient::AmbientCoefficient(float r, float g, float b) : r(r), g(g), b(b) {
    controleerBereik(r, "AmbientCoefficient");
    controleerBereik(g, "AmbientCoefficient");
    controleerBereik(b, "AmbientCoefficient");
}

void AmbientCoefficient::apply(Materiaal &m) const {
    m.ambient[0] = r; m.ambient[1] = g; m.ambient[2] = b;
}

// Diffuse

DiffuseCoefficient::DiffuseCoefficient(float r, float g, float b) : r(r), g(g), b(b) {
    controleerBereik(r, "DiffuseCoefficient");
    controleerBereik(g, "DiffuseCoefficient");
    controleerBereik(b, "DiffuseCoefficient");
}

void DiffuseCoefficient::apply(Materiaal &m) const {
    m.diffuse[0] = r; m.diffuse[1] = g; m.diffuse[2] = b;
}

// Speculair

SpeculaireCoefficient::SpeculaireCoefficient(float r, float g, float b, float shininess)
    : r(r), g(g), b(b), shininess(shininess) {
    controleerBereik(r, "SpeculaireCoefficient");
    controleerBereik(g, "SpeculaireCoefficient");
    controleerBereik(b, "SpeculaireCoefficient");
    if (shininess < 0.0f) {
        throw std::invalid_argument("Shininess mag niet negatief zijn [SpeculaireCoefficient]");
    }
}

void SpeculaireCoefficient::apply(Materiaal &m) const {
    m.specular[0] = r; m.specular[1] = g; m.specular[2] = b;
    m.shininess = shininess;
}

// Textuur

Textuur::Textuur(const std::string &bestand) : id(0) {
    int tw, th, tk;
    stbi_set_flip_vertically_on_load(true);

    // Altijd 3 kanalen (RGB) opvragen, ook als het bestand er 1 of 4 heeft.
    unsigned char *data = stbi_load(bestand.c_str(), &tw, &th, &tk, 3);
    if (!data) throw std::runtime_error("Texture inladen mislukt: " + bestand);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    // Rijen van RGB-data zijn niet altijd een veelvoud van 4 bytes.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tw, th, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
}

Textuur::~Textuur() {
    if (id != 0) {
        glDeleteTextures(1, &id);
    }
}

void Textuur::apply(Materiaal &m) const {
    m.textuur = id;
    m.gebruiktTextuur = true;
}
