#pragma once
#include "cTexture.hpp"
#include "cPheromones.hpp"
#include "cObject.hpp"



enum BorderMode {
    BORDER_BOUNCE,
    BORDER_THROUGH,
    BORDER_KILL
};

enum FollowMode {
    FOLLOW_COUNT,
    FOLLOW_AVEARGE
};

enum SoundMode {
    SOUND_OFF,
    SOUND_SHOCK,
    SOUND_SPEED
};

class Ant: public Object {
    public:
        bool alive, moving;
        int type, follow;

        Ant(Texture* t);

        void randomTurn(float maxA);
        void deflect(float x, float y, int dangerDist, int criticalDist);
        void checkWallCollision(int width, int height, BorderMode borderMode);
        void followPhCount(Pheromones* pheromones, int area, Uint8 maxA, float strength, BorderMode borderMode);
        void followPhAverage(Pheromones* pheromones, int area, Uint8 maxA, float strength, BorderMode borderMode);
        void producePh(Pheromones* pheromones, float rate);
};
