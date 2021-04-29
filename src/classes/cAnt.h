#pragma once
#include "cTexture.h"
#include "cObject.h"



enum AntTypes {
    ANT_TYPE_EMPTY,
    ANT_TYPES_COUNT
};

enum BorderMode {
    BORDER_BOUNCE,
    BORDER_THROUGH,
    BORDER_KILL
};

enum FollowMode {
    FOLLOW_COUNT,
    FOLLOW_AVEARGE
};

class Ant: public Object {
    public:
        bool alive, moving;
        AntTypes type, follow;

        Ant(Texture* t);

        void randomTurn(float maxA);
        void deflect(float x, float y, int dangerDist, int criticalDist);
        void checkWallCollision(int width, int height, BorderMode borderMode);
};
