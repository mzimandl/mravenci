#pragma once
#include <vector>

#include "cObject.h"
#include "cAnt.h"
#include "cTexture.h"



enum AntTypes {
    ANT_TYPE_EMPTY,
    ANT_TYPES_COUNT
};

class Colony: public Object {
    public:
        Colony(Texture* t, int population);
        ~Colony();

        std::vector<Ant *> ants;

        void renderAnts(float scale);
};
