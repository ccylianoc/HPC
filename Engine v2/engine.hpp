#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>
#include <utility>
#include "shaders.hpp"
#include "matrix.hpp"
#include "shapes.hpp"
#include "camera.hpp"
#include "lightsource.hpp"
#include "transformatie.hpp"

class World {
    private:
        GLFWwindow *venster;
        int breedte, hoogte;

        GLuint programma;
        std::vector<std::unique_ptr<Shape>> shapes;
        void tekenShape(size_t i);

        void render();
        void verwerkInvoer();
        void meetFrame(double renderTijd);
        bool toetsPress(int toets);   // true bij de overgang van los naar ingedrukt

        // Toestand van invoer en animatie
        std::vector<bool> toetsVorig = std::vector<bool>(GLFW_KEY_LAST + 1, false);
        bool gepauzeerd = false;
        double animTijd = 0.0;
        double laatsteTijd = 0.0;
        double frameDt = 0.0;      // duur van het vorige frame in seconden

        // Efficientie en meting
        bool geoptimaliseerd = true;
        bool vsync = false;
        int meetFrames = 0;
        double meetStart = 0.0;
        double renderSom = 0.0;
        void resetMeting();

    public:
        World(int breedte, int hoogte, const std::string &titel);

        // Voegt een vorm toe aan de voorstelling en geeft een verwijzing
        // ernaar terug, zodat je transformaties en eigenschappen kunt
        // instellen. Er is geen maximum aantal vormen. De argumenten gaan
        // naar de constructor van de vorm (na het shader-programma).
        //   Cube &kubus = wereld.addShape<Cube>();
        //   Cylinder &pijp = wereld.addShape<Cylinder>(12);   // 12 segmenten
        template <typename T, typename... Args>
        T &addShape(Args &&...args) {
            static_assert(std::is_base_of<Shape, T>::value,
                          "addShape verwacht een subclass van Shape");
            glUseProgram(programma);
            shapes.push_back(std::make_unique<T>(programma, std::forward<Args>(args)...));
            return static_cast<T &>(*shapes.back());
        }

        size_t aantalShapes() const { return shapes.size(); }

        Camera camera;

        Lightsource lichtbron;

        void run();

        ~World();
};
