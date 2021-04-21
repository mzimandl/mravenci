#pragma once
#include <vector>

#include "cObject.h"
#include "cAnt.h"
#include "cTexture.h"



class Colony: public Object {
    public:
        Colony(Texture* t, int population);
        ~Colony();

        std::vector<Ant *> ants;

        void renderAnts(float scale);
};
