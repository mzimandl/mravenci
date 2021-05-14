#include <cstdlib>
#include <SDL2/SDL.h>

#include "../math.hpp"

#include "cPheromones.hpp"



Pheromones::Pheromones(SDL_Renderer* r, int screenW, int screenH, float s, int typesCount) :
renderer(r), width((int)(screenW*s)), height((int)(screenH*s)), size(s)
{
    value.resize(typesCount);
    for (auto& p : value) p = new float[width * height];

    Uint8* pixels; int pitch;

    textures.resize(typesCount);
    for (int t = 0; t < typesCount; t++) {
        auto& texture = textures[t] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, width, height);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if ((t - 2) % 3 == 0)
                    pixels[4*x + pitch*y] = 255; // red
                else
                    pixels[4*x + pitch*y] = 0;
                
                if ((t - 1) % 3 == 0)
                    pixels[4*x + pitch*y + 1] = 255; // green
                else
                    pixels[4*x + pitch*y + 1] = 0;

                if (t % 3 == 0)
                    pixels[4*x + pitch*y + 2] = 255; // blue
                else
                    pixels[4*x + pitch*y + 2] = 0;

                pixels[4*x + pitch*y + 3] = SDL_ALPHA_TRANSPARENT; // alpha
            }
        }
        SDL_UnlockTexture(texture);
    }
}

Pheromones::~Pheromones() {
    for (auto& texture : textures) { SDL_DestroyTexture(texture); texture = NULL; }
    renderer = NULL;

    for (auto& p : value) delete[] p;
}

void Pheromones::render(int type) {
    Uint8* pixels; int pitch;
    float* p = value[type];
    auto& texture = textures[type];
    Uint8 intensity;

    SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            intensity = (int)(255*p[x + y*width]);
            if (intensity > 0) pixels[4*x + pitch*y + 3] = intensity;
        }
    }
    SDL_UnlockTexture(texture);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
}

void Pheromones::renderAll() {
    for (int t = 0; t < value.size(); t++) {
        render(t);
    }
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
