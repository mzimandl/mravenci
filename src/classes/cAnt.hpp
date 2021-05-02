#pragma once
#include "cTexture.hpp"
#include "cPheromones.hpp"
#include "cObject.hpp"



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
        void followPhCount(Pheromones* pheromones, int area, Uint8 maxA, float strength, BorderMode borderMode);
        void followPhAverage(Pheromones* pheromones, int area, Uint8 maxA, float strength, BorderMode borderMode);
        void producePh(Pheromones* pheromones, float rate);
};
