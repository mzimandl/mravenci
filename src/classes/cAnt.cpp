#include "cAnt.h"



void Ant::modify(bool isAlive, int antType, int followType) {
    alive = isAlive;
    type = antType;
    follow = followType;
}

void Ant::randomTurn(float maxA) {
    a += (static_cast<float>(rand())/static_cast<float>(RAND_MAX)) * maxA - maxA/2;
}
