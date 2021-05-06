#pragma once
#include <SDL2/SDL.h>

#include "cTexture.hpp"



class StaticObject {
    private:
        Texture* texture;

    public:
        SDL_FPoint pos;
        float angle;

        StaticObject(Texture* t);

        void setPos(float x, float y);
        void setAngle(float a);
        void render(float scale = 1);
        void setTexture(Texture* texture);
};

class AntObject: public StaticObject {
    public:
        float radius;

        AntObject(Texture* t);

        bool inRange(StaticObject &obj);
};

class Object: public StaticObject {
    public:
        float speed;

        Object(Texture* t);

        void move(float dt);
        void setSpeed(float s);
};

bool inRange(StaticObject* obj1, StaticObject* obj2, int d);
