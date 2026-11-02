# Opdrachten les 22
## Opdracht 1: Meerdere vormen
In deze opdracht ga je een programma maken dat meerdere vormen rendert, ieder met zijn eigenschappen zoals kleur en transformatie.

1. Compileer en run de gegeven [voorbeeldcode](glfw_multiple_shapes.cpp). In dit voorbeeld worden twee vormen gerenderd. Iedere vorm heeft zijn eigen VAO en VBO. Deze worden in dit voorbeeld hard-coded geïndexeerd voor de twee vormen; in je eigen programma doe je dit natuurlijk in een loop.
2. Haal de data voor de vertices van de kubus en de piramide weg. Gebruik in plaats daarvan deze [include file](shapes.h). Deze bevat vertices (posities en normaalvectoren) voor een kubus, een piramide, een bol en een cilinder.
3. We gaan nu het programma aanpassen zodat alle 4 de vormen gerenderd worden. Maak hiervoor een abstract class `Shape` met subclasses `Cube`, `Pyramid`, `Sphere` en `Cylinder`.
4. Zorg dat de constructor van deze classes de VBO vult met de juiste serie vertices uit `shapes.h`.  Bedenk welke functionaleit thuishoort in de parent class (`Shape`) en welke in de subclasses.
5. Zorg dat iedere vorm zijn eigen transformatie kan hebben (een combinatie van schaling, rotatie en translatie) en zijn eigen eigenschappen zoals kleur.
6. Voeg een `draw()`-method toe, die de VAO _bindt_, de uniforms instelt voor deze vorm en de vertices tekent met `glDrawArrays()`.
7. Maak instanties van de verschillende vormen aan en zet die in een lijst, bijv. een `std::vector<std::reference_wrapper<Shape>>`.
8. Ken aan iedere instantie een eigen kleur en translatie toe, zodat de 4 verschillende vormen met verschillende kleuren op verschillende plekken (bijv. in een 2 × 2 grid) gerenderd worden.
9. Doorloop in de main loop de lijst met vormen en roep van iedere vorm de `draw()`-method aan.
10. Test je programma. Als het goed is heb je nu een structuur waarin je een willekeurig aantal vormen kunt renderen, elk met eigen transformatie en eigenschappen. 

## Opdracht 2: Eigenschappen
In deze opdracht ga je de eigenschappen van de vormen, zoals kleur, transparantie, factoren voor ambient, diffuse en speculaire belichting, enz., onderbrengen in aparte classes.
1. Maak een abstracte class `Property` met concrete subclasses als `Color`, `Texture`, `DiffuseReflection`, enz. Bedenk hoe de constructors van deze subclasses eruit moeten zien.
2. Zorg dat je meerdere eigenschappen aan een vorm kan toevoegen, door aan de `Shape`-class een method `addProperty(Property p)` toe te voegen. Sla in deze method de eigenschappen op in een datastructuur, bijv. van type `std::vector<std::reference_wrapper<Property>>`.
3. Zorg dat bij het tekenen van de vorm (in de `draw()`-method) alle ingestelde eigenschappen van de vorm toegepast worden door de _attributes_ van de shader van de juiste data te voorzien.
4. Test je programma door verschillende vormen met verschillende eigenschappen te renderen.

## opdracht 3: Camera-class

In deze opdracht ga je je programma uit de vorige opdracht uitbreiden met een instelbare `Camera`-class.

De camera-transformatie bestaat uit de volgende onderdelen:
1. Rotaties rond de _x_-, _y_-, en _z_-as.
2. Translatie met een vector die tegengesteld is aan de positievector van de camera.
3. Perspectiefprojectie met een gegeven _field of view_.

In de vertex shader moeten alledrie deze onderdelen toegepast worden op de vertices. Echter, voor het berekenen van de 3D-posities die gebruikt worden voor de diffuse en speculaire belichting (`fragPos`) moeten wel de rotaties en translatie worden toegepast, maar _niet_ de projectie. En op de normaalvectoren moeten wel de rotaties worden toegepast, maar _niet_ de translatie en projectie.

Je moet je class dus zo maken dat je de complete transformatie (onderdeel 1+2+3) eruit kan krijgen, maar ook de transformatie zonder projectie (onderdeel 1+2) en de transformatie zonder translatie en projectie (onderdeel 1).

1. Voeg een `Camera`-class (subclass van `Transform`) toe aan je programma uit de vorige opdracht met een constructor waar je de (_x_, _y_, _z_)-positie, rotatiehoeken rond de  _x_-, _y_-, en _z_-as, en een _field of view_ aan kan meegeven.
2. Zorg dat er transformatiematrices worden berekend voor de complete transformatie, de transformatie zonder projectie en de transformatie zonder translatie en projectie. Je kunt hiervoor je code uit de opdracht bij les 15 gebruiken. Voor de perspectiefprojectie kun je deze matrix gebruiken:
    $$\mathbf{P}_\mathrm{persp}=\left(\begin{matrix}
    1 & 0 & 0 & 0\\
    0 & 1 & 0 & 0\\
    0 & 0 & -\mathrm{fov} & -1 \\
    0 & 0 & -\mathrm{fov} & 0
    \end{matrix}\right)$$
   Waarbij fov het _field of view_ is. Hiervoor kun je als default-waarde 1 hanteren.
3. Voeg methods toe voor het aanpassen van de positie en de rotaties, zodat een bewegende camera mogelijk wordt. 
4. Pas de vertex shader aan zodat de camera-transformatie wordt toegepast op de vertices voor het berekenen van `gl_Position`, de transformatie zonder projectie voor het berekenen van `fragPos`, en de transformatie zonder projectie en translatie voor de normaalvectoren.
5. Pas de fragment shader aan zodat de camera-transformatie zonder projectie wordt toegepast op de positie van de lichtbron.
6. Pas de translaties van de verschillende vormen aan; deze hoeven nu niet meer “naar achteren” gezet te worden voor de projectie. Maak in plaats daarvan een instantie aan van je `Camera`-class waarbij je de camera een eindje in de positieve _z_-richting zet. (Dit komt uiteindelijk op hetzelfde neer: de camera-transformatie zal alle vormen in de negatieve _z_-richting verplaatsen, en de camera blijft in de oorsprong).
7. Test je programma met verschillende camera-standpunten en -oriëntaties.

# Opdracht 4: Lichtbron-class

In deze opdracht ga je je programma uit de vorige opdracht uitbreiden met een instelbare lichtbron-class.

1. Voeg een class `Lightsource` toe aan je programma met een constructor waaraann je de positie en de kleur van de lichtbron kunt meegeven.
2. Pas de fragment shader aan zodat de data uit de class gebruikt worden in de shader. Let op dat de  lichtbron ook getransformeerd wordt door de camera-transformatie (zonder projectie).
3. Test je programma met verschillende instellingen van de lichtbron.
4. **Extra**: kun je meerdere lichtbronnen toevoegen aan je programma?

# Opdracht 5: Muis-input toevoegen

In deze opdracht ga je je programma uit de vorige opdracht uitbreiden met muis-input.

1. Gebruik de functie `glfwGetCursorPos()` om de _x_-en _y_-coördinaten van de muiscursor op te vragen voordat de main loop begint en sla ze op.
2. Vraag de _x_-en _y_-coördinaten iedere iteratie van de main loop op
3. Gebruik het verschil voor de rotatie van de camera rond de _y_- en _x_-as.
4. Voeg een callback-functie toe voor het scrollwiel (zie https://www.glfw.org/docs/3.3/input_guide.html#scrolling) waarin je de offset opslaat in een globale variabele of field.
5. Gebruik de scroll-offset in de main loop om de _z_-coördinaat van de camera aan te passen, zodat je kunt in- en uitzoomen.
6. Zorg dat de uniforms in de shaders geüpdated worden.
7. Test je programma. Als het goed is kun je nu rondkijken en in- en uitzoomen in de 3D-wereld van je graphics engine.
8. **Extra**: pas het programma aan zodat je echt kunt “rondvliegen” door de 3D-wereld.