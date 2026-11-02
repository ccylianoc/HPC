#include "engine.hpp"

// how to compile: g++ demo.cpp Engine\engine.cpp -o demo.exe -IEngine -lglfw3 -lglew32 -lopengl32 -lgdi32


// g++ demo.cpp Engine\engine.cpp Engine\shaders.cpp Engine\matrix.cpp Engine\vector.cpp -o demo.exe -IEngine -lglfw3 -lglew32 -lopengl32 -lgdi32


int main() {
    World wereld(800, 600, "Graphics Engine");
    wereld.run();
    return 0;
}


