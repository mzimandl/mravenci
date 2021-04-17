#pragma once
#include <SDL2/SDL.h>
#include "cTexture.h"



class Object {
    private:
        Texture* texture;

    public:
        SDL_FPoint pos;
        float v, a;
        int type;

        Object(Texture* t, int g);

        void move(float dt);
        void randomTurn(float maxA);
        void setValues(float ix, float iy, float iv, float ia);
        void render(float scale = 1);
        void setTexture(Texture* texture);
};
