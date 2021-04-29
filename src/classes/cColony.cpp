#include "cTexture.h"
#include "cColony.h"



Colony::Colony(Texture* t, int maxPopulation) :
Object(t), population(0)
{
    ants.resize(maxPopulation);
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
                ant->setValues(pos.x, pos.y, rand() % speedVariation + speed, rand() % 360);
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
