#include "cTexture.h"
#include "cColony.h"



Colony::Colony(Texture* t, int maxPopulation) : Object(t) {
    ants.resize(maxPopulation);
}

Colony::~Colony() {
    setTexture(NULL);
    for (auto& ant : ants) delete ant;
}

void Colony::renderAnts(float scale = (float)1.0) {
    for (auto& ant : ants) if (ant->alive) ant->render(scale);
}

void Colony::reviveAnts(int N) {
    if (population < ants.size()) {
        for (auto& ant : ants) {
            if (!ant->alive) {
                ant->pos = pos;
                ant->alive == true;
                population++;
                N--;
            }

            if (population == ants.size() or N == 0) {
                break;
            }
        }
    }
}
