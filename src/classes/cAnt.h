#include "cObject.h"



class Ant: public Object {
    public:
        bool alive;
        int type;
        int follow;

        using Object::Object;

        void modify(bool isAlive, int antType, int followType);
};
