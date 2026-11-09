#include "engine.hpp"
#include <iostream>

// how to compile:
// g++ demo.cpp "Engine v2\engine.cpp" "Engine v2\shapes.cpp" "Engine v2\shaders.cpp" "Engine v2\properties.cpp" "Engine v2\matrix.cpp" "Engine v2\vector.cpp" "Engine v2\transformatie.cpp" "Engine v2\camera.cpp" "Engine v2\lightsource.cpp" -o demo.exe -I"Engine v2" -lglfw3 -lglew32 -lopengl32 -lgdi32


int main() {
    try {
        World wereld(800, 600, "Graphics Engine");

        // Kubus: goud
        Cube &kubus = wereld.addShape<Cube>();
        kubus.getTransform().setTranslatie(-1.2f, 0.0f, 0.0f);
        kubus.getTransform().setSchaal(1.5f);
        kubus.setRotatieSnelheid(0.0f, 1.0f, 0.0f);
        kubus.addProperty<AmbientCoefficient>(0.5f, 0.5f, 0.5f);
        kubus.addProperty<DiffuseCoefficient>(0.0f, 0.0f, 0.0f);
        kubus.addProperty<SpeculaireCoefficient>(0.0f, 0.0f, 0.0f);

        // Piramide: glas (doorzichtig)
        Pyramid &piramide = wereld.addShape<Pyramid>();
        piramide.getTransform().setTranslatie(1.2f, 0.0f, 0.0f);
        piramide.getTransform().setSchaal(1.5f);
        piramide.setRotatieSnelheid(-0.7f, 0.0f, 0.0f);
        piramide.addProperty<AmbientCoefficient>(0.1f, 0.1f, 0.15f);
        piramide.addProperty<DiffuseCoefficient>(0.3f, 0.4f, 0.5f);
        piramide.addProperty<SpeculaireCoefficient>(1.0f, 1.0f, 1.0f, 64.0f);
        piramide.addProperty<Opaciteit>(0.4f);

        // Bol: steen met textuur (texture.png moet in de map staan waar je demo.exe start)
        Sphere &bol = wereld.addShape<Sphere>();
        bol.getTransform().setTranslatie(-1.2f, 3.0f, 0.0f);
        bol.getTransform().setSchaal(1.5f);
        bol.setRotatieSnelheid(0.0f, 0.0f, 1.5f);
        bol.addProperty<AmbientCoefficient>(0.4f, 0.4f, 0.4f);
        bol.addProperty<DiffuseCoefficient>(1.0f, 1.0f, 1.0f);
        bol.addProperty<SpeculaireCoefficient>(0.3f, 0.3f, 0.3f, 16.0f);
        bol.addProperty<Textuur>("texture.png");

        // Cilinder: koper
        Cylinder &cilinder = wereld.addShape<Cylinder>();
        cilinder.getTransform().setTranslatie(1.2f, 3.0f, 0.0f);
        cilinder.getTransform().setSchaal(1.5f);
        cilinder.setRotatieSnelheid(0.0f, 0.5f, 0.0f);
        cilinder.addProperty<AmbientCoefficient>(0.19f, 0.07f, 0.02f);
        cilinder.addProperty<DiffuseCoefficient>(0.70f, 0.27f, 0.08f);
        cilinder.addProperty<SpeculaireCoefficient>(0.99f, 0.94f, 0.81f, 64.0f);

        // Een willekeurig aantal vormen: een rij kleine gekleurde kubussen
        for (int i = 0; i < 5; i++) {
            Cube &blokje = wereld.addShape<Cube>();
            blokje.getTransform().setTranslatie(-2.0f + i * 1.0f, -2.0f, 0.0f);
            blokje.getTransform().setSchaal(0.6f);
            blokje.setRotatieSnelheid(0.5f * i, 0.8f, 0.0f);
            blokje.addProperty<Kleur>(i / 4.0f, 1.0f - i / 4.0f, 0.5f);
        }

        // Camera en lichtbron kunnen ook zelf ingesteld worden:
        // wereld.camera.setPositie(Vector({0, 1, 2}));
        // wereld.camera.setOrientatie(0.3f, -0.2f);   // yaw, pitch in radialen
        // wereld.lichtbron.setPositie(Vector({2, 3, 1}));
        // wereld.lichtbron.setKleur(1.0f, 0.9f, 0.8f);

        wereld.run();
    } catch (const std::exception &e) {
        std::cerr << "Fout: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
