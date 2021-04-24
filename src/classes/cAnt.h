#pragma once
#include "cObject.h"



enum AntTypes {
    ANT_TYPE_EMPTY,
    ANT_TYPES_COUNT
};

class Ant: public Object {
    public:
        bool alive;
        AntTypes type;
        AntTypes follow;

        using Object::Object;

        void modify(bool isAlive, AntTypes antType, AntTypes followType);
        void randomTurn(float maxA);
};
