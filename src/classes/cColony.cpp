#include "cTexture.h"
#include "cColony.h"



Colony::Colony(Texture* t, int population) : Object(t) {
    ants.resize(population);
}

Colony::~Colony() {
    setTexture(NULL);
    for (auto& ant : ants) delete ant;
}

void Colony::renderAnts(float scale = (float)1.0) {
    for (auto& ant : ants) if (ant->alive) ant->render(scale);
}
