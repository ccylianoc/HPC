# Engine v2

Een kleine 3D-graphics engine in C++ met OpenGL, gebouwd voor het vak High Performance Computing (TINHPC). De engine gebruikt geen externe wiskundebibliotheek zoals GLM: alle vector- en matrixberekeningen zitten in een eigen `Vector`- en `Matrix`-library.

## Bestandsstructuur

| Bestand | Inhoud |
|---|---|
| `engine.hpp` / `engine.cpp` | `World`-class: venster, shaders, objecten, tekenloop |
| `shapes.hpp` / `shapes.cpp` | `Shape`-parent class met subclasses `Cube`, `Pyramid`, `Sphere` |
| `shaders.hpp` / `shaders.cpp` | Vertex- en fragment-shaderbronnen, compileren en linken |
| `matrix.hpp` / `matrix.cpp` | `Matrix`, `MatrixN`, `Matrix3`, `Matrix4`, `Transformatie` |
| `vector.hpp` / `vector.cpp` | `Vector` (n-dimensionaal) |
| `demo.cpp` | `main()`: maakt een `World` aan |
| `stb_image.h` | Texture-loading (header-only) |

## Bouwen

Vanuit de map waar `demo.cpp` staat:

```bash
g++ demo.cpp "Engine v2\engine.cpp" "Engine v2\shapes.cpp" "Engine v2\shaders.cpp" \
    "Engine v2\matrix.cpp" "Engine v2\vector.cpp" -o demo.exe -I"Engine v2" \
    -lglfw3 -lglew32 -lopengl32 -lgdi32
```

Vereist: GLFW3, GLEW en een OpenGL-context van minimaal 3.3 core.

## Texture

De sphere gebruikt een textuur. Zet `texture.png` in de map waar je `demo.exe` start — dus niet in `Engine v2`, maar in de map waar je het programma uitvoert. Ontbreekt het bestand, dan gooit het programma een `runtime_error` met de melding dat inladen mislukt is.

De textuur wordt via `stb_image` ingeladen met `GL_RGB`, voorzien van mipmaps, en gebonden aan texture unit 0. Per object bepaalt `Materiaal::gebruiktTextuur` of de textuur meegerekend wordt in de belichting.

## Objecten en materialen

Elk object heeft een eigen `Materiaal`:

```cpp
struct Materiaal {
    float ambient[3];
    float diffuse[3];
    float specular[3];
    float shininess;
    float alpha;
    bool gebruiktTextuur;
};
```

- `ambient` — kleur in de schaduw, houd hem donker (10–20% van diffuse)
- `diffuse` — de hoofdkleur onder direct licht
- `specular` — glans; `shininess` bepaalt hoe klein de glansvlek is (8 = groot en zacht, 128 = klein en scherp)
- `alpha` — transparantie, 1.0 is ondoorzichtig

## Features

**Eigen wiskunde.** `Vector` ondersteunt optellen, aftrekken, scalair vermenigvuldigen, inproduct, uitproduct en normaliseren. `Matrix` ondersteunt vermenigvuldigen met een matrix en met een vector, transponeren, determinant en inverse. `Matrix4` en `Transformatie` leveren translatie-, schaal-, rotatie- en perspectiefmatrices in homogene coördinaten.

**Per-object transformatie.** Elk object heeft een eigen modelmatrix, opgebouwd uit translatie, schaling en rotatie.

**Belichting (Phong).** Ambient, diffuse en specular met een instelbare shininess, berekend per fragment.

**Transparantie.** Objecten met `alpha < 1.0` worden in twee passes getekend: eerst alle ondoorzichtige objecten, daarna de transparante met blending aan en diepteschrijven uit. Zo blijft de achterkant van een transparant object zichtbaar.

**Normal perturbation.** De normaal wordt in de fragment-shader verstoord met een sinuspatroon in objectruimte, waardoor het oppervlak reliëf lijkt te hebben zonder dat de geometrie verandert.

**Texture mapping.** De sphere heeft uv-coördinaten en een textuur; de andere objecten gebruiken hun materiaalkleur.

## Wat nog niet af is

De volgende onderdelen uit les 22 staan nog open:

- `Camera`-class met eigen view- en projectiematrices (nu staat de view hardgecodeerd)
- `Lightsource`-class (nu staat de lichtpositie hardgecodeerd in de shader)
- Muisinput voor roteren en zoomen
- `Property`-classes als alternatief voor de `Materiaal`-struct
- `Cylinder` als vierde vorm
