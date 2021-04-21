#include "cAnt.h"



void Ant::modify(bool isAlive, int antType, int followType) {
    alive = isAlive;
    type = antType;
    follow = followType;
}
