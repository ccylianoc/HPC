#pragma once

#include <GL/glew.h>
#include <string>

// Materiaal bundelt wat de shader per vorm nodig heeft. Je vult het
// meestal niet zelf in, maar via Property-objecten (zie hieronder):
// shape.addProperty<Kleur>(1, 0, 0) past het materiaal van die vorm aan.
struct Materiaal {
    float kleur[3]    = {1.0f, 1.0f, 1.0f};   // RGB-kleur; vermenigvuldigd met textuur
    float ambient[3]  = {0.3f, 0.3f, 0.3f};
    float diffuse[3]  = {0.8f, 0.8f, 0.8f};
    float specular[3] = {1.0f, 1.0f, 1.0f};
    float shininess   = 32.0f;
    float alpha       = 1.0f;   // 1.0 = ondoorzichtig
    GLuint textuur    = 0;      // GL-handle van de bitmap
    bool gebruiktTextuur = false;
};

// Een Property is een eigenschap die een vorm kan krijgen. Elke subclass
// weet zelf welk deel van het Materiaal zij instelt; Shape hoeft dus niet
// te weten welke soorten eigenschappen er bestaan.
class Property {
    public:
        virtual ~Property() = default;
        virtual void apply(Materiaal &m) const = 0;
};

// RGB-kleur van het oppervlak, elke component van 0 tot 1.
class Kleur : public Property {
    private:
        float r, g, b;

    public:
        Kleur(float r, float g, float b);
        void apply(Materiaal &m) const override;
};

// Opaciteit: 1.0 is ondoorzichtig, 0.0 is onzichtbaar.
class Opaciteit : public Property {
    private:
        float alpha;

    public:
        explicit Opaciteit(float alpha);
        void apply(Materiaal &m) const override;
};

// Ambient reflectiecoefficient (per kleurkanaal).
class AmbientCoefficient : public Property {
    private:
        float r, g, b;

    public:
        AmbientCoefficient(float r, float g, float b);
        void apply(Materiaal &m) const override;
};

// Diffuse reflectiecoefficient (per kleurkanaal).
class DiffuseCoefficient : public Property {
    private:
        float r, g, b;

    public:
        DiffuseCoefficient(float r, float g, float b);
        void apply(Materiaal &m) const override;
};

// Speculaire reflectiecoefficient (per kleurkanaal) met glansexponent.
// Een kleine shininess geeft een grote zachte glansvlek, een grote een
// kleine scherpe.
class SpeculaireCoefficient : public Property {
    private:
        float r, g, b;
        float shininess;

    public:
        SpeculaireCoefficient(float r, float g, float b, float shininess = 32.0f);
        void apply(Materiaal &m) const override;
};

// Textuur uit een bitmap (bijv. PNG). De bitmap wordt in de constructor
// ingelezen en naar de GPU gestuurd; er moet dus al een GL-context zijn
// (maak eerst een World aan). De textuur wordt weer vrijgegeven wanneer
// dit object verdwijnt, en is niet kopieerbaar.
class Textuur : public Property {
    private:
        GLuint id;

    public:
        explicit Textuur(const std::string &bestand);
        ~Textuur();

        void apply(Materiaal &m) const override;

        Textuur(const Textuur &) = delete;
        Textuur &operator=(const Textuur &) = delete;
};
