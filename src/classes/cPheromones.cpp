#include <cstdlib>
#include <SDL2/SDL.h>

#include "../math.h"

#include "cPheromones.h"



Pheromones::Pheromones(SDL_Renderer* r, int screenW, int screenH, float s, int typesCount) :
renderer(r), width((int)(screenW*s)), height((int)(screenH*s)), size(s)
{
    value.resize(typesCount);
    for (auto& p : value) p = new float[width * height];

    Uint8* pixels; int pitch;

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width, height);
    SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);
    for (int xy = 0; xy < pitch*height; xy++) { pixels[xy] = 255; }
    SDL_UnlockTexture(texture);
}

Pheromones::~Pheromones() {
    SDL_DestroyTexture(texture);
    texture = NULL;
    renderer = NULL;

    for (auto& p : value) delete[] p;
}

void Pheromones::render(int type) {
    Uint8* pixels; int pitch;
    float* p = value[type];

    SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (p[x + y*width] > 0) {
                int intensity = (int)(255*p[x + y*width]);
                pixels[4*x + pitch*y + 1] = 255 - intensity;
                pixels[4*x + pitch*y + 2] = 255 - intensity;
            }
        }
    }
    SDL_UnlockTexture(texture);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
}

void Pheromones::decay(float rate) {
    for (auto& type : value)
        for (int y = 0; y < width*height; y+=width) {
            for (int x = 0; x < width; x++) {
                float& p = type[x + y];
                if (p > 0) p = std::max(p - rate, (float)0);
            }
        }
}
