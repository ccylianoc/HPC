# Opdrachten les 19
## Opdracht 1: Texture
1. Compileer en run het voorbeeldprogramma [glfw_rotating_cube_texture.cpp](code/glfw_rotating_cube_texture.cpp).

    In dit programma bestaat iedere vertex uit 3 positie-coördinaten (_x_, _y_ en _z_), 3 kleurcomponenten (_r_, _g_ en _b_) en 2 texture-coördinaten (_u_, _v_). De texture wordt door het programma gegenereerd en in de texture buffer geplaatst. Doordat de texture-coördinaten van 0 tot 5 lopen, wordt het plaatje in zowel de _u_- als de _v_-richting 5× herhaald.
2. Pas de texture-coördinaten van de vertices aan zodat het plaatje maar 1× op de zijden van de kubus geplakt wordt.

    Meestal wil je geen gegenereerde textuur, maar een textuur afkomstig van een plaatje, bijv. een PNG-bestand. Je kunt plaatjes inlezen met de [STB image loader](https://github.com/nothings/stb/blob/master/stb_image.h). Deze library bestaat alleen maar uit een header file.
3. Download de header file en zet hem op een plek waar je compiler hem kan vinden.
4. Om de library te gebruiken, zet je aan het begin van je programma:
    ```c++
    #define STB_IMAGE_IMPLEMENTATION
    #include "stb_image.h"
    ```
5. Zoek een leuk plaatje uit, bijv. van https://picsum.photos/ en zet dat in de map waar je programma staat.
6. Lees het plaatje in met deze code:
    ```c++
    GLsizei width, height;
    int nrChannels;
    GLubyte *image = stbi_load("plaatje.png", &width, &height, &nrChannels, 0);
    if (image == NULL) {
        puts("Failed to load image");
        exit(-1);
    }
    ```
7. Om te zorgen dat de data goed oplijnt met wat OpenGL verwacht, moet je bij een _oneven_ aantal pixels per rij de _unpack alignment_ aanpassen:
    ```c++
    if (width % 2) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    ```
8. Vervolgens kan je de data uit het plaatje gebruiken om de texture buffer te vullen:
    ```c++
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    ```
    Doe dit in plaats van de gegeneerde texture.
9. Test je programma. Als het goed is wordt het plaatje nu op de kubus geplakt, vermengd met de kleuren van de vertices.
10. Pas de fragment shader aan zodat alleen de kleuren van het plaatje gebruikt worden.

## Opdracht 2: Belichting

In de vorige opdracht heb je geen expliciete vorm van belichting gebruikt; dit komt overeen met _ambient_ belichting: licht dat overal aanwezig is en dat maakt dat we de kleur van de texture kunnen zien. Realistischer is om ook _diffuse_ belichting en _speculaire_ belichting toe te voegen. Bij diffuse belichting wordt de helderheid van de kleur geschaald afhankelijk van de richting van het licht.
1. Bekijk, compileer en run dit [codevoorbeeld](code/glfw_rotating_cube_positional_lighting.cpp) voor diffuse belichting. Je ziet dat de `baseColor` (de “eigen” kleur van het oppervlak) geschaald wordt met de berekende `lightness` op dat punt veroorzaakt door de lichtbron. Experimenteer met verschillende posities van de lichtbron.

2. Voeg aan je programma van de vorige opdracht belichting toe, die bestaat uit een combinatie van ambient en diffuse belichting. Bedenk daarbij dat de uiteindelijke kleur wordt bepaald door de kleur van zowel het oppervlak als van de lichtbron. Experimenteer met verschillende verhoudingen, bijv. ambient : diffuse = 0,2 : 0,8:

    ```glsl
    float ambient = 0.2f;
    float diffuse = 0.8f * max(0.0f, dot(normalizedNormal, reverseLightDirection));

    fragColor = baseColor;
    fragColor.rgb *= (ambient + diffuse) * lightColor;
    ```

    De functie `max()` wordt hier gebruikt om te zorgen dat de diffuse component nooit kleiner dan 0 wordt.

    Bij speculaire belichting wordt een “glimvlek” toegevoegd aan het oppervlak die de kleur van de lichtbron krijgt. De lokatie van de glimvlek wordt bepaald door de weerspiegeling van het licht aan het oppervlak.

3. Bekijk, compileer en run dit [codevoorbeeld](code/glfw_rotating_cube_specular_lighting.cpp) voor speculaire belichting. Je ziet dat de `lightColor` (de kleur van de lichtbron) geschaald wordt met de berekende `lightness` op dat punt. De grootte van de glimvlek wordt bepaald door de _shinyness_ (de exponent van de machtsfunctie; in dit voorbeeld 32). Experimenteer met verschillende waarden van de shinyness.
4. Voeg aan je programma van stap 2 speculaire belichting toe. Bedenk daarbij de kleur van de speculaire bijdrage alleen bepaald wordt door de kleur van de lichtbron. Kies voor de verhoudingen bijv. ambient : diffuse : specular = 0,2 : 0,5 : 0,3. De speculaire bijdrage kun je optellen bij de ambient en diffuse bijdrage:

    ```glsl
    fragColor.rgb += specular * lightColor;
    ```
5. Neem in de fragment shader zowel de kleur van de texture als de kleuren van de vertices mee, zodat de uiteindelijke kleur van het oppervlak een vermenging is van de texture, de kleur van de vertices en de kleur van de lichtbron, met een combinatie van ambient, diffuse en speculaire belichting.

## Verwerkingsopdracht
Leidt de reflectieformule $\vec{r}=\vec{l}-2(\vec{n}\cdot\vec{l})\vec{n}$ af. Gegeven de normaalvector $\vec{n}$ en de omgekeerde lichtrichting $\vec{l}$, schrijf stap voor stap op hoe je de gereflecteerde vector $\vec{r}$ bepaalt.

**Hint:** _projecteer_ $\vec{l}$ op $\vec{n}$ (zie slide “projectie van een vector op een andere vector”). Dit is afstand van het oppervlak naar de lichtbron, haaks op het oppervlak. Als je nu _tweemaal_ de omgekeerde verplaatsing neemt vanaf de lichtbron, kom je uit op het gevraagde gereflecteerde punt (maak een tekening).