#pragma once

#include <GL/glew.h>
#include <string>

// Een Shader vertegenwoordigt één GLSL-shader (vertex of fragment).
// De broncode wordt in de constructor meegegeven en pas gecompileerd
// wanneer compile() wordt aangeroepen, zodat de aanroeper fouten
// kan opvangen voordat er iets naar de GPU gaat.
class Shader {
    private:
        GLuint id;
        GLenum type;

    public:
        Shader(GLenum type, const std::string &bron);
        ~Shader();

        // Compileert de shader. Gooit een std::runtime_error met de
        // volledige GLSL-foutmelding als het compileren mislukt.
        void compile();

        GLuint getId() const { return id; }

        // Shaders zijn niet kopieerbaar: het GLuint is een handle
        // naar GPU-geheugen dat maar één eigenaar mag hebben.
        Shader(const Shader &) = delete;
        Shader &operator=(const Shader &) = delete;
};

// Een Program koppelt een vertex- en een fragment-shader aan elkaar
// tot één uitvoerbaar shader-programma op de GPU.
class Program {
    private:
        GLuint id;

    public:
        Program(const Shader &vertex, const Shader &fragment);
        ~Program();

        // Maakt dit programma actief voor de volgende tekenopdrachten.
        void use() const;

        GLuint getId() const { return id; }

        Program(const Program &) = delete;
        Program &operator=(const Program &) = delete;

        GLuint release();
};
