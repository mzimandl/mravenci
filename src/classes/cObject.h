#pragma once
#include <SDL2/SDL.h>

#include "cTexture.h"



struct Vel {
    float v;
    float a;
};

class Object {
    private:
        Texture* texture;

    public:
        SDL_FPoint pos;
        Vel vel;

        Object(Texture* t);

        void move(float dt);
        void setPos(float x, float y);
        void setVel(float v, float a);
        void render(float scale = 1);
        void setTexture(Texture* texture);
};

bool inRange(Object* obj1, Object* obj2, int d);
