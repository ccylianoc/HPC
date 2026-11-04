# Graphics Engine V2

In deze README kan je alle informatie vinden over de tweede versie van de Graphics Engine, die in de /Engine v2 dir staat.

Deze versie is een doorontwikkeling van de library in /Engine. Dezelfde opzet, maar met een eigen `Shape`-hiërarchie, een `Transformatie`-class die alle matrices oplevert, en een `World` die meerdere vormen in één loop tekent.

Alle library's worden in de `.hpp` header gedefined en in de `.cpp` geïnitialiseerd.

## Bestanden

### vector.hpp / vector.cpp

De `Vector`-class voor 2D tot 4D vectoren. Slaat de waarden op in een `std::vector<float>` en houdt de dimensie bij.

Wat je ermee kan:

- `length()` en `normalized()` voor de lengte en de genormaliseerde vector
- `operator+`, `operator-`, `operator*` en `operator/` voor rekenen met vectoren en scalairen
- `dot()` en `cross()` voor het inwendig en uitwendig product
- `operator==` en `operator!=` met een tolerantie van `1e-6`, omdat floats nooit exact gelijk zijn
- `operator<<` om een vector leesbaar naar de terminal te printen

De `cross()` werkt alleen op 3D-vectoren en gooit een `std::invalid_argument` bij een andere dimensie.

### matrix.hpp / matrix.cpp

De matrix-library, opgebouwd uit een parent class en drie subclasses.

**`Matrix`** is de basis. Elke matrix is een `rij x kol`-raster van floats in een `std::vector<float>`, met `set()` en `at()` om een element te schrijven en te lezen.

Bewerkingen: `operator*` voor matrix-vermenigvuldiging en matrix-vector-vermenigvuldiging, `operator+`, `operator-`, `getransponeerd()`, `determinant()` en `inverse()`. De determinant en inverse werken via Gauss-eliminatie en gooien een fout bij een niet-vierkante of singuliere matrix.

**`MatrixN`** is de algemene n×m-matrix, voor als de afmetingen pas tijdens het draaien bekend zijn.

**`Matrix3`** en **`Matrix4`** zijn de vaste 3×3- en 4×4-varianten, met `eenheid()` als statische functie voor de identiteitsmatrix. `Matrix4` heeft daarnaast `naarGL()`, die de matrix kolom-major in een array van 16 floats schrijft, precies zoals OpenGL het verwacht.

**`Transformatie`** is de class die je in de praktijk gebruikt. Je maakt geen matrix met de hand, je vraagt een transformatie op:

```c++
Matrix4 projectie = Transformatie::perspectief(45.0f, aspect, 0.1f, 100.0f).naarMatrix();
Matrix4 translatie = Transformatie::translatie(0, 0, -4).naarMatrix();
Matrix4 rotatie    = Transformatie::rotatie(hoek, Y_AS).naarMatrix();
Matrix4 schaal     = Transformatie::schaling(1.5f, 1.5f, 1.5f).naarMatrix();
```

Intern houdt een `Transformatie` een enum bij met het type (`ROTATIE`, `TRANSLATIE`, `SCHALING`, `PERSPECTIEF`) en de bijbehorende parameters. `naarMatrix()` bouwt op basis daarvan de juiste matrix. Een rotatie gebeurt rond een as (`X_AS`, `Y_AS`, `Z_AS`) en de hoek is in radialen.

### shaders.hpp / shaders.cpp

Twee classes die het compileer- en linkwerk van GLSL regelen.

**`Shader`** vertegenwoordigt één shader, vertex of fragment. Je geeft de broncode mee aan de constructor en roept daarna `compile()` aan. Gaat er iets mis, dan gooit `compile()` een `std::runtime_error` met de volledige GLSL-foutmelding erin, zodat je in de terminal ziet op welke regel je shader stukloopt.

**`Program`** koppelt een vertex- en een fragment-shader aan elkaar tot één programma op de GPU, met `use()` om het actief te maken.

Beide classes zijn niet kopieerbaar (`= delete`), omdat het `GLuint` een handle naar GPU-geheugen is en er maar één eigenaar mag zijn. `Program` heeft daarnaast `release()`: dat geeft het id terug en zet het interne id op 0, zodat de destructor het programma niet opruimt. Dat is nodig omdat in `World` het `Program`-object tijdelijk is, maar het GPU-programma moet blijven bestaan.

### shapes.hpp / shapes.cpp

De vormen die je tekent.

**`Shape`** is de abstracte basisclass. Elke vorm heeft een eigen VAO en VBO, en kent zijn aantal vertices. De beschermde functie `upload()` doet het werk: vertices naar de GPU kopiëren en de attributen `pos` en `normaal` koppelen.

Elke vertex bestaat uit 6 floats: eerst x, y, z (de positie), dan nx, ny, nz (de normaal). De stride is dus 24 bytes, `pos` staat op offset 0 en `normaal` op offset 12.

`draw()` is puur virtueel, dus elke subclass bepaalt zelf hoe hij tekent.

**`Cube`** laadt de 36 vertices van de kubus (ribbe 1, gecentreerd op de oorsprong) en tekent met `GL_TRIANGLES`.

**`Pyramid`** laadt de 18 vertices van de piramide (grondvlak op y = -0.5, top op (0, 0.5, 0)) en tekent ook met `GL_TRIANGLES`.

De vertexdata zelf komt uit `shapes.h`, het bestand dat we van school hebben gekregen. Dat bestand bevat alleen de arrays; de classes hierboven maken er tekenbare objecten van.

### engine.hpp / engine.cpp

De `World`-class is de motor. Die regelt alles rond het venster en de render-loop.

De constructor doet het opstartwerk in deze volgorde:

1. GLFW initialiseren en een venster maken met een OpenGL 3.3 core profile context
2. GLEW initialiseren, zodat de OpenGL-functies beschikbaar zijn
3. De vertex- en fragment-shader compileren en linken tot één programma
4. De shapes aanmaken; dat moet ná `glUseProgram`, omdat de attribuut-locaties anders niet opgevraagd kunnen worden
5. Dieptetest aanzetten en de achtergrondkleur instellen

`render()` tekent één frame:

1. Het scherm leegmaken, inclusief de dieptebuffer
2. De projectiematrix ophalen via `Transformatie::perspectief`
3. De view-matrix ophalen via `Transformatie::translatie(0, 0, -4)`, zodat de camera op de oorsprong staat en de wereld naar voren schuift
4. Beide matrices naar de shader sturen
5. Voor elke shape een modelmatrix opbouwen uit schaling, rotatie en translatie, en die meesturen
6. De shape laten tekenen

De volgorde van de modelmatrix is belangrijk: `positie * rotatie * schaal`. Matrix-vermenigvuldiging werkt van rechts naar links, dus de schaal wordt als eerste toegepast, dan de rotatie, en als laatste de verplaatsing. Draai je die om, dan draait de vorm niet om zijn eigen as maar om de oorsprong van de wereld.

`run()` is de hoofdloop: viewport bijwerken, `render()` aanroepen, buffers wisselen en events verwerken. De destructor ruimt eerst de shapes op, dan het venster en GLFW.

### De shaders

De shaders staan als raw string in `engine.cpp`, niet in een apart bestand.

De vertex shader krijgt per vertex `pos` en `normaal` binnen, en drie uniform-matrices: `projection`, `view` en `model`. Hij vermenigvuldigt ze tot clip-coördinaten en deelt daarna door `clip.w`. Die deling is de perspectiefdeling: zonder die stap klapt de vorm in elkaar of verdwijnt hij buiten beeld.

De fragment shader geeft nu één effen kleur. De normaal wordt wel al doorgegeven als `fragNormaal`, zodat belichting later toe te voegen is zonder de pipeline te wijzigen.

### draw.h / draw.cpp

De tekenlibrary van school, met `point()`, `line()`, `color()` en `getTime()`. Deze wordt in de engine-versie niet gebruikt, want alles gaat via shaders. Het bestand staat er nog omdat het bij de opdrachten hoort.

## Hoe gebruiken we deze library's

Je maakt een `.cpp` file aan waarin je de engine include:

```c++
#include "engine.hpp"

int main() {
    World wereld(800, 600, "Graphics Engine");
    wereld.run();
    return 0;
}
```

Compileren vanuit de hoofdmap:

```batchfile
g++ demo.cpp "Engine v2\engine.cpp" "Engine v2\shapes.cpp" "Engine v2\shaders.cpp" "Engine v2\matrix.cpp" "Engine v2\vector.cpp" -o demo.exe -I"Engine v2" -lglfw3 -lglew32 -lopengl32 -lgdi32
```

De `-I"Engine v2"` zorgt dat de headers in /Engine v2 gevonden worden.

## Verschil met /Engine

|            | /Engine                         | /Engine v2               |
| ---------- | ------------------------------- | ------------------------ |
| Kubus      | hardgecodeerd in `engine.cpp` | eigen `Cube`-class     |
| Vormen     | één                           | lijst van shapes         |
| Matrices   | handmatig met `set()`         | via `Transformatie`    |
| Tekenen    | `GL_LINES`, draadmodel        | `GL_TRIANGLES`, gevuld |
| Attributen | alleen `pos`                  | `pos` en `normaal`   |
| Dieptetest | uit                             | aan                      |

### Eigen note Git

Om via vsc terminal te pushen:

```batchfile
cmd /c .\git-push.bat
```

Om via vsc terminal te pullen:

```batchfile
cmd /c .\git-pull.bat
```
