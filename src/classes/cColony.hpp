#pragma once
#include <vector>
#include <map>

#include "cObject.hpp"
#include "cAnt.hpp"
#include "cTexture.hpp"



class Colony: public AntObject {
    private:
        int population;

    public:
        std::map<int, int> followMap;

        Colony(Texture* colonyTexture, Texture* antTexture, int maxPopulation, float r, int type, bool change);
        ~Colony();

        std::vector<Ant *> ants;

        void renderAnts(float scale);
        void checkPopulation();
        void reviveAnts(int N, int speed, int speedVariation);
        void producePh(Pheromones* pheromones, float rate);
        void updateFollowType(Ant* ant);
};
