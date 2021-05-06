#pragma once
#include <vector>

#include "cObject.hpp"
#include "cAnt.hpp"
#include "cTexture.hpp"



class Colony: public AntObject {
    private:
        int population;

    public:
        Colony(Texture* colonyTexture, Texture* antTexture, int maxPopulation);
        ~Colony();

        std::vector<Ant *> ants;

        void renderAnts(float scale);
        void checkPopulation();
        void reviveAnts(int N, int speed, int speedVariation);
        void producePh(Pheromones* pheromones, float rate);
};
