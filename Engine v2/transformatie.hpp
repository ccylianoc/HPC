#pragma once

#include "matrix.hpp"

// Transform bundelt de drie transformaties van een vorm op één plek:
// schaling, rotatie (rond alle drie de assen) en translatie.
// De volgorde van toepassen is vast: eerst schalen, dan roteren,
// dan verplaatsen. Zo draait een vorm om zijn eigen middelpunt
// in plaats van om de oorsprong van de wereld.
class Transform {
    private:
        float sx, sy, sz;          // schaling per as
        float tx, ty, tz;          // translatie per as
        float rx, ry, rz;          // rotatie per as, in radialen
        unsigned long versie_ = 0; // wordt verhoogd zodra een waarde echt verandert

    public:
        Transform();

        void setSchaal(float x, float y, float z);
        void setSchaal(float s);
        void setTranslatie(float x, float y, float z);
        void setRotatie(float x, float y, float z);

        void schaal(float x, float y, float z);
        void transleer(float x, float y, float z);
        void roteer(float x, float y, float z);

        float getSchaalX() const { return sx; }
        float getSchaalY() const { return sy; }
        float getSchaalZ() const { return sz; }
        float getTranslatieX() const { return tx; }
        float getTranslatieY() const { return ty; }
        float getTranslatieZ() const { return tz; }
        float getRotatieX() const { return rx; }
        float getRotatieY() const { return ry; }
        float getRotatieZ() const { return rz; }

        // Versieteller voor het cachen van de modelmatrix.
        unsigned long versie() const { return versie_; }

        // Bouwt de modelmatrix: translatie * rz * ry * rx * schaling.
        Matrix4 model() const;
};
