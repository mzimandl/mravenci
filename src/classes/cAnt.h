#pragma once
#include "cTexture.h"
#include "cObject.h"



enum AntTypes {
    ANT_TYPE_EMPTY,
    ANT_TYPES_COUNT
};

class Ant: public Object {
    public:
        bool alive, moving;
        AntTypes type, follow;

        Ant(Texture* t);

        void randomTurn(float maxA);
        void deflect(float x, float y, int dangerDist, int criticalDist);
        void checkWallCollision(int width, int height, bool periodic);
};
