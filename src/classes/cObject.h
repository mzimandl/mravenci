#pragma once
#include <SDL2/SDL.h>

#include "cTexture.h"



class Object {
    private:
        Texture* texture;

    public:
        SDL_FPoint pos;
        float v, a;

        Object(Texture* t);

        void move(float dt);
        void setValues(float ix, float iy, float iv, float ia);
        void render(float scale = 1);
        void setTexture(Texture* texture);
};

bool inRange(Object* obj1, Object* obj2, int d);
