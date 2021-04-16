#pragma once
#include <string>
#include <SDL2/SDL.h>
#include <cmath>
#include <cstdlib>

#include "routines.cpp"

using namespace std;

// ------------------------ Texture --------------------------

class Texture {
    private:
        SDL_Rect sRect;
        SDL_Rect dRect;
        SDL_Texture* texture;
        SDL_Renderer* renderer;
    public:
        Texture(SDL_Renderer* r, string path);
        ~Texture();

        void render(int x, int y, float a, float scale = 1);
};


Texture::Texture(SDL_Renderer* r, string path) {
    renderer = r;
    texture = loadTexture(path, renderer);
    SDL_QueryTexture(texture, NULL, NULL, &sRect.w, &sRect.h);
}

Texture::~Texture() {
    SDL_DestroyTexture(texture);
}

void Texture::render(int x, int y, float a, float scale) {
    dRect.w = sRect.w * scale;
    dRect.h = sRect.h * scale;
    dRect.x = x - (int)round((float)dRect.w/2);
    dRect.y = y - (int)round((float)dRect.h/2);
    SDL_RenderCopyEx(renderer, texture, NULL, &dRect, a, NULL, SDL_FLIP_NONE);
}

// ------------------------ Object --------------------------

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

Object::Object(Texture* t, int g = 0) {
    texture = t;
    type = g;
}

void Object::move(float dt) {
    pos.x += v*cos(a*M_PI/180)*dt;
    pos.y += v*sin(a*M_PI/180)*dt;
}

void Object::randomTurn(float maxA) {
    a += (static_cast<float>(rand())/static_cast<float>(RAND_MAX)) * maxA - maxA/2;
}

void Object::setValues(float ix, float iy, float iv, float ia) {
    pos.x = ix;
    pos.y = iy;
    v = iv;
    a = ia;
}

void Object::render(float scale) {
    texture->render((int)round(pos.x), (int)round(pos.y), a, scale);
}

void Object::setTexture(Texture* newTexture) {
    texture = newTexture;
}
