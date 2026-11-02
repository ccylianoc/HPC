# Opdrachten les 20
## Opdracht 1: Materiaaleigenschappen
In deze opdracht ga je materiaaleigenschappen toevoegen aan je shader.

1. Ga uit van je programma van de vorige les, maar verwijder de texture sampler uit de shader. Definieer in je fragment shader de lichtbron (positie en kleur), bijv.:
    ```glsl
    vec3 lightPos = vec3(0.2f, -2.0f, 0.0f);
    vec3 lightColor = vec3(0.5f, 0.5f, 0.5f);
    ```
2. Definieer een struct voor het materiaal:
    ```glsl
    struct Material {
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shininess;
    };
    ```
3. Maak hier een uniform mee:
    ```glsl
    uniform Material material;
    ```
4. Bereken binnen de `main()`-functie van je fragment shader de ambient-component:
    ```glsl
    vec3 ambient = lightColor * material.ambient;
    ```
5. Bereken ook de diffuse component:
    ```glsl
    vec3 normalizedNormal = normalize(normal);
    vec3 reverseLightDirection = normalize(lightPos - fragPos);
    float diffuseFactor = max(0.0f, dot(normalizedNormal, reverseLightDirection));
    vec3 diffuse = diffuseFactor * lightColor * material.diffuse;
    ```
6. Bereken ook de speculaire component:
    ```glsl
    vec3 reflectedDirection = reflect(reverseLightDirection, normalizedNormal);
    vec3 viewingDirection = normalize(fragPos);
    float specularFactor = pow(max(0.0f, dot(viewingDirection, reflectedDirection)), 128.0f * material.shininess);
    vec3 specular = specularFactor * lightColor * material.specular;
    ```
    De factor 128 wordt hier gebruikt om shinyness-waarden tussen 0 en 1 te kunnen gebruiken.
7. Tel tenslotte de drie componenten bij elkaar op:
    ```glsl
    vec3 combined = ambient + diffuse + specular;
    fragColor = vec4(combined, 1.0);
    ```
6. Voorzie in je C++-programma de Material-uniform van data. Kies daarvoor een materiaal uit [deze lijst](http://devernay.free.fr/cours/opengl/materials.html). Je moet voor ieder field in de struct apart de locatie opvragen:
    ```c++
    GLint ambientLocation = glGetUniformLocation(program, "material.ambient");
    glUniform3f(ambientLocation, r, g, b};
    ```
    enzovoort. N.B.: gebruik voor de shinyness `glUniform1f` in plaats van `glUniform3f`.

7. Experimenteer met verschillende materialen. Kun je een materiaal nabootsen dat niet in de lijst staat, bijv. porcelein?

## Opdracht 2: Transparency

In deze opdracht ga je transparency toevoegen aan je programma uit de vorige opdracht.

1. Om in OpenGL te werken met transparency, moet je _blending_ aanzetten (buiten de main loop):
    ```c++
    glEnable(GL_BLEND);
    ```
2. Je moet ook de manier aangeven waarop twee kleuren gecombineerd moeten worden, dus de factoren voor $C_s$ en $C_d$ in
    $$
    C=\ \alpha_sC_s+\left(1-\alpha_s\right)C_d
    $$
    Dit doe je met de functie `glBlendFunc()`.
    ```c++
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ```
3. Vervolgens kun je α-waarden toevoegen aan de vertices:
    ```c++
    GLfloat vertices[] = {
         1,  1,  1, ..., 1.0, // front top right
         1, -1,  1, ..., 0.5, // front bottom right
        -1, -1,  1, ..., 0.0, // front bottom left
            ⋮
    ```
    Geef bijv. alle vertices in het voorvlak de α-waarde 1.0 en in het achtervlak 0.0.
4. In je vertex shader moet je een nieuw attribute toevoegen voor de alpha-waarde van type `float`, die doorgegeven moet worden aan de fragment shader:
    ```glsl
    in float alp;
    out float alpha = alp;
    ```
5. In de fragment shader moet hij binnenkomen:
    ```glsl
    in float alpha;
    ```
    en ingevuld worden als vierde compnent van `fragColor` (in plaats van 1.0).
6. Bij het invullen van de attributes in je C++-programma moet je hier ook rekening mee houden: de _stride_ voor alle attributes is nu 4 bytes meer.
7. Voorzie de alpha-attribute van data met het juiste datatype (1 `GL_FLOAT`), de juiste stride en offset.
8. Compileer en test je programma. Als het goed is krijg je nu een draaiende kubus te zien die aan één kant semi-transparant is. Experimenteer met verschillende achtergrondkleuren (`glClearColor()`).

## Opdracht 3: Normaalvector manipuleren

In deze opdracht ga je kijken wat het effect is van kleine verstoringen in de normaalvector. Ga uit van je programma uit Opdracht 1 (dus zonder transparency).
1. Om de verstoring consistent te laten zijn tussen verschillende frames van de draaiende kubus, laten we de verstoring afhangen van de ongetransformeerde 3D-positie. Die moeten we vanuit de vertex shader doorgeven aan de fragment shader. Voeg toe aan de vertex shader (buiten de `main()`-functie):
    ```glsl
    out vec3 position;
    ```
    Voeg aan de `main()`-functie van de vertex shader toe:
    ```glsl
    postion = pos;
    ```
    Voeg aan de fragment shader een attribute toe:
    ```glsl
    in vec3 position;
    ```
2. In de `main()`-functie van de fragment shader kun je vervolgens een verstoring aanbrengen aan de nornaalvector, bijv.
    ```glsl
    vec3 disturbedNormal = normal + 0.1 * sin(10.0 * position);
    ```
    Gebruik deze `disturbedNormal` vervolgens voor de berekeningen:
    ```glsl
    vec3 normalizedNormal = normalize(disturbedNormal);
    ```
3. Compileer en test je programma. Wat valt op?
4. Probeer ook andere verstoringen, bijv.
    ```glsl
    vec3 disturbedNormal = normal + 0.1 * fract(100000.0 * sin(position));
    ```
    `fract()` geef het fractionele deel van een getal (alleen de cijfers achter de komma). Dit geeft een soort hoogfrequente ruis die het oppervlak ruw doet lijken.