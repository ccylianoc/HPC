# Opdrachten TINHPC les 18

## Opdracht 1: Draadmodel kubus

In les 13 heb je een projectie van een draadmodel van een kubus getekend met de `draw`-library. In les 16 heb je 2D-vormen getekend met shaders. In deze opdracht ga je die twee dingen combineren om een projectie van een draadmodel van een kubus te tekenen met shaders.

We hebben de volgende ingrediënten nodig:

- 8 hoekpunten (vertices)
- 12 verbindingslijnen (edges)
- Vertex shader
- Fragment shader
- Rotatie, translatie en perspectiefprojectie als matrix-operaties
- Tekeninstructie
- Animatie

1. Ga uit van de [voorbeeldcode uit les 16](../week_6_les_16/glfw_shader_example.cpp) en verander de `vertices`-array in de hoekpunten van een kubus:
    ![kubus1](../week_5_les_13/img/kubus1.png){width=640px}
    ```c++
    GLfloat vertices[] = {
         1,  1,  1, // front top right
         1, -1,  1, // front bottom right
        -1, -1,  1, // front bottom left
        -1,  1,  1, // front top left
         1,  1, -1, // back top right
         1, -1, -1, // back bottom right
        -1, -1, -1, // back bottom left
        -1,  1, -1  // back top left
    };
    ```
2. Voeg een array toe die aangeeft welke hoekpunten met elkaar verbonden moeten worden:

    ![kubus2](../week_5_les_13/img/kubus2.png){width=640px}

    Deze getallen geven telkens in paren de indices van de desbetreffende vertices aan. Het zijn altijd positieve integers dus als datatype gebruiken we `GLushort` (unsigned short integer):
    ```c++
    GLushort edges[] = {
        0, 1, 1, 2, 2, 3, 3, 0, // front edges
        4, 5, 5, 6, 6, 7, 7, 4, // back edges
        0, 4, 1, 5, 2, 6, 3, 7  // front-to-back edges
    };
    ```
    Deze array van indices kopiëren we naar een zogeheten _Element Buffer Object_ op de grafische kaart, die we eerst moeten aanmaken:
    ```c++
    // Create and bind an Element Buffer Object
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    
    // Fill the EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(edges), edges, GL_STATIC_DRAW);
    ```
3. De vertex shader moet de transformaties en projectie uitvoeren op de hoekpunten. We gebruiken homogene coördinaten, dus vectoren met 4 elementen en 4×4 transformatiematrices. Deze matrices worden later ingevuld (en kunnen dus dynamisch gewijzigd worden). We noemen dit soort variabelen _uniforms_, omdat ze hetzelfde (uniform) zijn voor alle vertices. Dit in tegenstelling tot _attributes_ (zoals de positie), die voor iedere vertex anders zijn. Verander de vertex shader in:
    ```c++
    const char *vsSource =
        R"(#version 330 core
        in vec3 pos;
        uniform mat4 projection;
        uniform mat4 translation;
        uniform mat4 rotation;
        void main() {
            gl_Position = projection * translation * rotation * vec4(pos, 1.0);
        })";
    ```
    Het delen door de vierde component (_perspective divide_) wordt automatisch uitgevoerd.
    Als de shader-code wat complexer wordt, bestaat de kans dat je een foutje maakt. Om te helpen debuggen is het handig om de output van de shader compiler te kunnen zien. Dit kun je doen door na `glCompileShader()` de status op te vragen en als deze nul is, het informatielog:
    ```c++
    GLint status;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
    if (!status) {
        char log[BUFSIZE];
        glGetShaderInfoLog(vs, BUFSIZE, NULL, log);
        puts(log);
        exit(-1);
    }
    ```
    Om `puts()` te kunnen gebruiken moet je de library `cstdio` opnemen bovenaan je programma. Ook moet je de constante `BUFSIZE` definiëren:
    ```c++
    #include <cstdio>
    #define BUFSIZE 256 // size of error log buffer
    ```
4. We gebruiken dezelfde fragment shader als in les 16. Je kunt natuurlijk wel de kleur veranderen:
    ```c++
    const char *fsSource = 
        R"(#version 330 core
        out vec4 fragColor;
        void main() {
            fragColor = vec4(1.0, 1.0, 1.0, 1.0);
        })";
    ```
    Ook bij de fragment shader is het handig om het informatielog van de compiler te printen als er iets mis is. Je kunt hiervoor dezelfde code gebruiken, waarbij je `vs` vervangt door `fs`:
    ```c++
    glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
    if (!status) {
        char log[BUFSIZE];
        glGetShaderInfoLog(fs, BUFSIZE, NULL, log);
        puts(log);
        exit(-1);
    }
    ```
5. Net als in les 16 kunnen de twee shaders nu gelinkt worden tot een programma. Je kunt een beetje geheugen besparen door de shaders daarna te verwijderen (het gelinkte shader program blijft behouden):
    ```c++
    glDeleteShader(vs);
    glDeleteShader(fs);
    ```
6. Voordat we kunnen gaan tekenen moeten we de _uniforms_ invullen. Voor de rotatie kiezen we een rotatie van 30° ($\pi/6$ in radialen) rond de _x_-as:
    ![rotatie](img/rotation.png){width=320px}

    De rotatiematrix is: $$\mathbf{R}_x=\left(\begin{matrix}1&0&0&0\\0&\cos{\vartheta}&-\sin{\vartheta}&0\\0&\sin{\vartheta}&\cos{\vartheta}&0\\0&0&0&1\end{matrix}\right)$$
    Om deze in te vullen als de uniform `rotation` moeten we de _lokatie in het geheugen_ van die uniform opvragen:
    ```c++
    GLint rotationLocation = glGetUniformLocation(program, "rotation");
    GLfloat c = cos(M_PI / 6);
    GLfloat s = sin(M_PI / 6);
    GLfloat rotation[] = {1, 0,  0, 0,
                          0, c, -s, 0,
                          0, s,  c, 0,
                          0, 0,  0, 1};
    glUniformMatrix4fv(rotationLocation, 1, GL_TRUE, rotation);
    ```
    Het derde argument van `glUniformMatrix4fv()` geeft aan of de matrix getransponeerd moet worden. We vullen hier `GL_TRUE` in omdat de getallen in _column-major_-volgorde moeten staan (eerst de getallen van de eerste kolom van boven naar beneden, dan de tweede kolom, enzovoort), maar wij gewend zijn om een matrix in _row-major_-volgorde te lezen (eerst de eerste rij van links naar rechts).

    Om `cos()`, `sin()` en de constante `M_PI` te kunnen gebruiken, moet je bovenaan je programma opnemen:
    ```c++
    #define _USE_MATH_DEFINES
    #include <cmath>
    ```
7. Om de perspectiefprojectie goed te kunnen laten werken, moeten we de figuur achter het projectievlak plaatsen. Daarvoor willen we een translatie van 3 eenheden naar achteren (in de negatieve _z_-richting):

    ![translatie](img/translation.png){width=320px}

    Stel de 4×4 translatiematrix hiervoor op en voeg code toe om die toe te wijzen aan de uniform `translation`.
8. Voor de perspectiefprojectie moeten we alle coördinaten vermenigvuldigen met de _z_-coördinaat van het projectievlak $z_p$ en delen door de _z_-coördinaat van het object $z_o$.

    ![perspectief](img/perspectiefprojectie.png)

    Zoals we in les 13 gezien hebben, zou dit kunnen met de volgende projectiematrix:
    $$\mathbf{P}=\left(\begin{matrix}z_p&0&0&0\\0&z_p&0&0\\0&0&z_p&0\\0&0&1&0\end{matrix}\right)$$
    Als je deze toepast op een punt $(x_o,y_o,z_o)$ (in homogene coördinaten) komt $z_o$ op de plek van de vierde component, waardoor gedeeld wordt:
    $$\left(\begin{matrix}z_p&0&0&0\\0&z_p&0&0\\0&0&z_p&0\\0&0&1&0\end{matrix}\right)\cdot\left(\begin{matrix}x_o\\y_o\\z_o\\1\end{matrix}\right)=\left(\begin{matrix}z_px_o\\z_py_o\\z_pz_o\\z_o\end{matrix}\right)\Rightarrow\left(\begin{matrix}z_px_o/z_o\\z_py_o/z_o\\z_p\\1\end{matrix}\right)$$
    Je ziet dat er door de vierde component gedeeld wordt. Aangezien zowel het punt als het projectievlak in de negatieve _z_-richting liggen, is $z_p/z_o$ een positief getal en worden alle coördinaten geschaald met dit getal. **Echter**, de vierde component _mag niet negatief_ zijn in OpenGL. [Dit heeft te maken met _clipping space_: alleen punten met $-w < x, y, z < w$ worden getekend, en als $w<0$ voldoet geen enkel punt hieraan.] We moeten dus een andere projectiematrix hanteren die hetzelfde effect heeft, maar waardoor de vierde component niet negatief wordt, bijvoorbeeld:
    $$\mathbf{P}=\left(\begin{matrix}1&0&0&0\\0&1&0&0\\0&0&1&0\\0&0&1/z_p&0\end{matrix}\right)$$
    Als je deze matrix toepast, krijg je hetzelfde resultaat maar zonder een negatieve vierde component:
    $$\left(\begin{matrix}1&0&0&0\\0&1&0&0\\0&0&1&0\\0&0&1/z_p&0\end{matrix}\right)\cdot\left(\begin{matrix}x_o\\y_o\\z_o\\1\end{matrix}\right)=\left(\begin{matrix}x_o\\y_o\\z_o\\z_o/z_p\end{matrix}\right)\Rightarrow\left(\begin{matrix}x_oz_p/z_o\\y_oz_p/z_o\\z_p\\1\end{matrix}\right)$$

    Kies bijvoorbeeld $z_p=-1$, stel de 4×4  perspectiefprojectiematrix op en en voeg code toe om die toe te wijzen aan de uniform `projection`.
9. Om te zorgen dat onze kubus er als een kubus uitziet in een venster dat breder is dan hoog, definiëren we een vierkante _viewport_:
    ![viewport](img/viewport.png){width=320px}

    Dit doen we door _binnen de loop_ de huidige breedte en hoogte van het venster (frame buffer) op te vragen, en de viewport zo in te stellen:
    ```c++
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport((width - height) / 2, 0, height, height);
    ```
    Door dit binnen de loop te doen wordt de viewport automatisch aangepast als het venster geresized wordt.
10. Om de lijntjes die we zo gaan tekenen wat mooier te maken, kunnen we nog instellen dat we _oversampling_ willen. Dit is een vorm van _anti-aliasing_. Dit doen we door voordat het venster aangemaakt wordt met `glfwCreateWindow()`, een _window hint_ aan te geven:
    ```c++
    glfwWindowHint(GLFW_SAMPLES, 4);
    ```
    Hiermee stellen we 4× oversampling in.
11. Nu kunnen we eindelijk gaan tekenen! Met de tekeninstructie die we geven, willen we niet driehoeken laten tekenen van telkens 3 vertices, zoals in les 16, maar willen we lijnen laten tekenen tussen de vertices waarvan de indices zijn gegeven in het element buffer object. We gebruiken daarvoor niet `glDrawArrays()`, maar:
    ```c++
    glDrawElements(GL_LINES, sizeof(edges) / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
    ```
    De constructie `sizeof(edges) / sizeof(GLushort)` geeft ons het aantal elementen in de array. Dit wordt door de C++-compiler vervangen door een constante, aangezien `edges` een statische array is.

    Pas je programma aan en test het. Als het goed is zie je nu een projectie van een kubus.
12. Om animatie toe te voegen, bijv. een rotatie rond de _y_-as, kunnen we nog een extra rotatiematrix toevoegen aan de vertex shader:
    $$\mathbf{R}_y=\left(\begin{matrix}\cos{\vartheta}&0&\sin{\vartheta}&0\\0&1&0&0\\-\sin{\vartheta}&0&\cos{\vartheta}&0\\0&0&0&1\end{matrix}\right)$$
    We kunnen deze rotatiematrix in de shader laten uitrekenen, en alleen de rotatiehoek als _uniform_ van type `float` doorgeven (in plaats van de hele matrix). We moeten dan wel de elementen van de matrix in _column-major_-volgorde zetten:
    ```glsl
    #version 330 core
    in vec3 pos;
    uniform mat4 projection;
    uniform mat4 translation;
    uniform mat4 rotation;
    uniform float rotationAngle;
    void main() {
        float s = sin(rotationAngle);
        float c = cos(rotationAngle);
        mat4 rotationY = mat4(c, 0, -s, 0,
                              0, 1,  0, 0,
                              s, 0,  c, 0,
                              0, 0,  0, 1); // Rotation matrix in column-major order
        gl_Position = projection * translation * rotation * rotationY * vec4(pos, 1.0);
    }
    ```
    We moeten vervolgens de lokatie in het geheugen van deze `float` opvragen:
    ```c++
    GLint rotationAngleLocation = glGetUniformLocation(program, "rotationAngle");
    ```
    en hier _binnen de loop_ een waarde aan toekennen, zodat die ieder frame geüpdated wordt:
    ```c++
    GLfloat angle = ROTATION_SPEED * glfwGetTime();
    glUniform1f(rotationAngleLocation, angle);
    ```
    Definieer bovenaan je programma de constante `ROTATION_SPEED`, bijv. met de waarde 0,5 radialen per seconde.

    Tenslotte moeten we het plaatje voortdurend opnieuw laten tekenen, dus niet alleen als er een ‘event’ optreedt. Hiervoor moeten we in de loop niet de functie `glfwWaitEvents();` gebruiken, die blokkeert totdat er een ‘event’ is, maar de functie `glfwPollEvents()` die alleen _checkt_ of er ‘events’ opgetreden zijn, maar niet blokkeert. Zie https://www.glfw.org/docs/latest/input_guide.html#events voor meer informtie.

    Pas je programma aan en test het. Als het goed is zie je nu een projectie van een draaiende kubus.

## Opdracht 2: kubus met dichte oppervlakken

In de vorige opdracht heb je een draadmodel van een kubus getekend. Om de kubus beter weer te geven, willen we eigenlijk dichte oppervlakken in plaats van een draadmodel. Het probleem daarmee is dat de zijden van de kubus vierhoeken zijn, maar een OpenGL shader kan alleen maar punten, lijnen of driehoeken tekenen. De reden dat OpenGL shaders geen ingevulde figuren met meer dan 3 hoeken kunnen tekenen, is dat je van een driehoek zeker weet dat de hoekpunten altijd in één vlak liggen. Met vier of meer hoekpunten kan het zijn dat de punten niet in één vlak liggen, en dan is het oppervlak niet goed gedefiniëerd.

Daarnaast is een driehoek altijd convex; concave vormen zijn lastiger in te kleuren. Van onderstaande figuur bijvoorbeeld valt het lichtgkleurde deel wel binnen de omhullende van alle hoekpunten, maar moet toch niet ingekleurd worden. Dit maakt dat voor het inkleuren van concave figuren een complexer algoritme nodig is.

![concaaf figuur](img/concave.png){width=320px}

Daarom willen we eigenlijk alles als driehoeken tekenen, en moeten we iedere vierhoek van onze kubus omzetten naar twee driehoeken:

![vierhoek](img/vierhoek.png){width=320px}

1. Sla je programma op met een nieuwe naam, zodat je altijd nog terug kan naar de werkende code van opdracht 1.
2. Vervang de array `edges` door een array die de indices bevat van de hoekpunten van de 12 driehoeken in plaats van de zes vierhoeken:

    ![driehoeken](img/kubus3.png){width=640px}

    ```c++
    GLushort edges[] = {
        0, 1, 2, 2, 3, 0, // front
        4, 5, 6, 6, 7, 4, // back
        0, 4, 1, 1, 4, 5, // bottom
        2, 6, 3, 3, 6, 7, // top
        0, 3, 4, 4, 7, 3, // left
        1, 2, 5, 5, 6, 2  // right
    };
    ```
3. Vervang in de tekeninstructie `GL_LINES` door `GL_TRIANGLES`. Test je programma. Als het goed is zie je nu een draaiende kubus met dichte oppervlakken. Echter, doordat alles wit is, zie je weinig structuur. Om te checken of de driehoeken goed getekend worden, kun je `glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)` gebruiken. Hiermee worden de vlakken niet ingevuld. Haal `glPolygonMode()` weer weg.
4. Om meer structuur aan de kubus te geven, voegen we kleur toe. We geven iedere vertex een eigen kleur, zodat een vertex nu niet alleen een (_x_, _y_, _z_)-positie heeft maar ook een (_r_, _g_, _b_)-kleur. Voeg deze toe aan de `vertices`-array:
    ```c++
    GLfloat vertices[] = {
         1,  1,  1, 1, 1, 1, // front top right, white
         1, -1,  1, 1, 0, 1, // front bottom right, purple
        -1, -1,  1, 0, 0, 1, // front bottom left, blue
        -1,  1,  1, 0, 1, 1, // front top left, cyan
         1,  1, -1, 1, 1, 0, // back top right, yellow
         1, -1, -1, 1, 0, 0, // back bottom right, red
        -1, -1, -1, 0, 0, 0, // back bottom left, black
        -1,  1, -1, 0, 1, 0  // back top left, green
    };
    ```
5. Pas de vertex shader aan zodat deze de kleur binnenkrijgt als _attribute_ en doorgeeft aan de fragment shader met een `out` variable:
    ```glsl
    #version 330 core
    in vec3 pos;
    in vec3 col;
    out vec3 color;
        ⋮
    void main() {
        color = col;
        ⋮
    ```
6. Pas de fragment shader aan zodat deze de kleur binnenkrijgt als _attribute_ met dezelfde naam als de `out` variable in de vertex shader, en gebruikt als kleur van het fragment.
    ```glsl
    #version 330 core
    in vec3 color;
    out vec4 fragColor;
    void main() {
        fragColor = vec4(color, 1.0);
    }
    ```
7. De _attributes_ `pos` en `col` moeten gevuld worden vanuit de vertex buffer. Iedere vertex bestaat nu uit zes 32-bits (4 bytes) floating point getallen. De vertex buffer heeft daarmee een _stride_ van 6 × 4 = 24 bytes. De positie heeft geen _offset_ en de kleur heeft een _offset_ van 12 bytes in de vertex data:
    ![vertex](img/vertex.png)
    We moeten deze _stride_  en _offset_ meegeven als vijfde en zesde argument in de aanroep van `glVertexAttribPointer()`. Als de offset niet 0 is, moeten we deze _casten_ naar type pointer-to-void (`void *`):
    ```c++
    // Positions using 3 floats per vertex
    GLint posLocation = glGetAttribLocation(program, "pos");
    glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 24, 0);
    glEnableVertexAttribArray(posLocation);

    // Colors using 3 floats per vertex with an offset of 12 bytes
    GLint colLocation = glGetAttribLocation(program, "col");
    glVertexAttribPointer(colLocation, 3, GL_FLOAT, GL_FALSE, 24, (void *)12);
    glEnableVertexAttribArray(colLocation);
    ```
    Test je programma. Als het goed is zie je nu een draaiende kubus met gekleurde oppervlakken. De kleuren worden geïnterpoleerd tussen de hoekpunten, zodat je verlopende kleuren krijgt. Echter, de kubus ziet er raar uit omdat sommige oppervlakken over andere heen getekend worden, terwijl ze eigenlijk niet zichtbaar zouden moeten zijn. Dit gaan we in de volgende opdracht oplossen.
## Opdracht 3: kubus met diepte-informatie
Het probleem is dat de pixels (fragments) van vlakken die dichter bij het ‘oog’ liggen, soms overschreven worden door fragments die verder weg liggen, en dus eigenlijk niet zichtbaar zouden moeten zijn. Een fragment zou alleen getekend moeten worden als het dichter bij het ‘oog’ ligt dan een al eerder getekend fragment:

![z-buffer](img/z-buffer.png)

Om dit te kunnen doen moeten we de diepte-informatie (_z_-coördinaat) van elk fragment bijhouden, zodat we kunnen checken of het getekend moet worden of niet. Dit kan in de zogeheten _z-buffer_ of _depth buffer_. Het probleem is dat bij de perspectieftransformatie die wij gebruiken, de _z_-coördinaat verloren gaat; alle _z_-coördinaten worden naar het projectievlak $z_p$ getransformeerd:
$$\left(\begin{matrix}1&0&0&0\\0&1&0&0\\0&0&1&0\\0&0&1/z_p&0\end{matrix}\right)\cdot\left(\begin{matrix}x\\y\\z\\1\end{matrix}\right)=\left(\begin{matrix}x\\y\\z\\z/z_p\end{matrix}\right)\Rightarrow\left(\begin{matrix}xz_p/z\\yz_p/z\\z_p\\1\end{matrix}\right)$$
Om dit op te lossen passen we de  perspectiefprojectiematrix aan zodat de diepte-informatie behouden blijft:
$$\left(\begin{matrix}1&0&0&0\\0&1&0&0\\0&0&1/z_p&-1\\0&0&1/z_p&0\end{matrix}\right)\cdot\left(\begin{matrix}x\\y\\z\\1\end{matrix}\right)=\left(\begin{matrix}x\\y\\z/z_p-1\\z/z_p\end{matrix}\right)\Rightarrow\left(\begin{matrix}xz_p/z\\yz_p/z\\1-z_p/z\\1\end{matrix}\right)$$
De _z_-coördinaat wordt hiermee getransformeerd naar $1-z_p/z$, waar de originele _z_-coördinaat in verwerkt zit, en dat altijd een waarde heeft tussen 0 en 1 voor alle waarden van $z<z_p$, dus alle _z_-waarden die achter het projectievlak liggen (bedenk dat $z$ en $z_p$ beide negatief zijn).
1. Sla je programma op met een nieuwe naam, zodat je altijd nog terug kan naar de werkende code van opdracht 2.
2. Pas de projectiematrix aan naar de hierboven gegeven variant, waarbij je voor $z_p$ bijv. –1 kiest.
3. Zet buiten de loop, voordat je gaat tekenen, de _depth test_ aan, die checkt of een pixel overschreven mag worden:
    ```c++
    glEnable(GL_DEPTH_TEST);
    ```
4. Zorg dat binnen de loop, bij het wissen van het frame voordat er getekend wordt, niet alleen de kleur van alle pixels maar ook de diepte gereset wordt:
    ```c++
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ```
5. Test je programma. Als het goed is zie je nu een draaiende kubus met gekleurde oppervlakken, waarbij de ‘verborgen’ oppervlakken niet getekend worden.

