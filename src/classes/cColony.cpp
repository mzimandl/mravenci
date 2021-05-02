#include "cTexture.hpp"
#include "cAnt.hpp"
#include "cColony.hpp"



Colony::Colony(Texture* colonyTexture, Texture* antTexture, int maxPopulation) :
StaticObject(colonyTexture), population(0)
{
    ants.resize(maxPopulation);
    for (auto& ant : ants) ant = new Ant(antTexture);
}

Colony::~Colony() {
    setTexture(NULL);
    for (auto& ant : ants) delete ant;
}

void Colony::renderAnts(float scale = (float)1.0) {
    for (auto& ant : ants) if (ant->alive) ant->render(scale);
}

void Colony::checkPopulation() {
    population = 0;
    for (auto& ant : ants) if (ant->alive) population++;
}

void Colony::reviveAnts(int N, int speed, int speedVariation) {
    if (population < ants.size()) {
        for (auto& ant : ants) {
            if (!ant->alive) {
                ant->setPos(pos.x, pos.y);
                ant->setAngle(rand() % 360);
                ant->setSpeed(rand() % speedVariation + speed);
                ant->alive = true;
                population++;
                N--;
            }

            if (population == ants.size() or N == 0) {
                break;
            }
        }
    }
}

void Colony::producePh(Pheromones* pheromones, float rate) {
    for (auto& ant : ants) if (ant->alive and ant->moving) ant->producePh(pheromones, rate);
}
