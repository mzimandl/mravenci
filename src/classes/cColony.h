#pragma once
#include <vector>

#include "cObject.h"
#include "cAnt.h"
#include "cTexture.h"



class Colony: public StaticObject {
    private:
        int population;

    public:
        Colony(Texture* t, int maxPopulation);
        ~Colony();

        std::vector<Ant *> ants;

        void renderAnts(float scale);
        void checkPopulation();
        void reviveAnts(int N, int speed, int speedVariation);
};
