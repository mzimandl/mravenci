#include <cstdlib>
#include <SDL2/SDL.h>

#include "cAnt.h"
#include "../math.h"

#include "cPheromones.h"



Pheromones::Pheromones(SDL_Renderer* r, Uint8 typesCount, int w, int h) {
    renderer = r;
    levels = typesCount;
    width = w; height = h;
    
    pheromones.resize(levels);
    for (auto& p : pheromones) p = new float[width * height];

    Uint8* pixels; int pitch;

    pheromoneTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width, height);
    SDL_LockTexture(pheromoneTexture, NULL, (void**)&pixels, &pitch);
    for (int xy = 0; xy < pitch*height; xy++) { pixels[xy] = 255; }
    SDL_UnlockTexture(pheromoneTexture);
}

Pheromones::~Pheromones() {
    SDL_DestroyTexture(pheromoneTexture);
    pheromoneTexture = NULL;
    renderer = NULL;

    for (auto& p : pheromones) delete[] p;
}

void Pheromones::render(Uint8 type) {
    Uint8* pixels; int pitch;
    float* p = pheromones[type];

    SDL_LockTexture(pheromoneTexture, NULL, (void**)&pixels, &pitch);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (p[x + y*width] > 0) {
                int intensity = (int)(255*p[x + y*width]);
                pixels[4*x + pitch*y + 1] = 255 - intensity;
                pixels[4*x + pitch*y + 2] = 255 - intensity;
            }
        }
    }
    SDL_UnlockTexture(pheromoneTexture);
    SDL_RenderCopy(renderer, pheromoneTexture, NULL, NULL);
}

void Pheromones::follow(Ant* ant, int area, Uint8 maxA, float strength) {
    int x = (int)round(ant->pos.x);
    int y = (int)round(ant->pos.y);

    const int area2 = area*area;
    const float arc = 2*(float)maxA/3;
    int count1 = 0, count2 = 0, count3 = 0;

    float* p = pheromones[ant->follow];

    for (int j = std::max(0, y-area); j < std::min(height, y+area+1); j++) {
        for (int i = std::max(0, x-area); i < std::min(width, x+area+1); i++) {
            if (p[i + j*width] > 0) {
                float dx = i - ant->pos.x;
                float dy = j - ant->pos.y;

                if (dx != 0 or dy != 0) {
                    float dist2 = dx*dx + dy*dy;
                    if (dist2 < area2) {
                        float dA = calculateAngleI(dx, dy, iqsqrt(dist2)) - ant->a;

                        if (dA > 180) dA -= 360;
                        else if (dA < -180) dA += 360;

                        if (abs(dA) < maxA) {
                            if (dA < -arc/2) count1++;
                            else if (dA > arc/2) count3++;
                            else count2++;
                        }
                    }
                }
            }
        }
    }

    if (count1 or count2 or count3) {
        if (count1 > count2 and count1 > count3) ant->a -= strength*arc;
        else if (count3 > count1 and count3 > count2) ant->a += strength*arc;
        else if (count1 == count2 < count3) ant->a -= strength*arc/2;
        else if (count1 < count2 == count3) ant->a += strength*arc/2;
    }
}

void Pheromones::followAverage(Ant* ant, int area, Uint8 maxA, float strength) {
    int x = (int)round(ant->pos.x);
    int y = (int)round(ant->pos.y);

    const int area2 = area*area;

    float diffA = 0;
    float total = 0;

    float* p = pheromones[ant->follow];

    for (int j = std::max(0, y-area); j < std::min(height, y+area+1); j++) {
        for (int i = std::max(0, x-area); i < std::min(width, x+area+1); i++) {
            const int intensity = p[i + j*width];
            if (intensity > 0) {
                float dx = i - ant->pos.x;
                float dy = j - ant->pos.y;

                if (dx != 0 or dy != 0) {
                    float dist2 = dx*dx + dy*dy;
                    if (dist2 < area2) {
                        float dA = calculateAngleI(dx, dy, iqsqrt(dist2)) - ant->a;

                        if (dA > 180) dA -= 360;
                        else if (dA < -180) dA += 360;

                        if (abs(dA) < maxA) {
                            diffA += intensity;
                            total += intensity;
                        }
                    }
                }
            }
        }
    }

    if (total > 0) ant->a += strength * diffA / total;
}

void Pheromones::decay(float rate) {
    for (auto& type : pheromones)
        for (int y = 0; y < width*height; y+=width) {
            for (int x = 0; x < width; x++) {
                float& p = type[x + y];
                if (p > 0) p = std::max(p - rate, (float)0);
            }
        }
}

void Pheromones::produce(Ant* ant, float rate) {
    int x = (int)round(ant->pos.x);
    int y = (int)round(ant->pos.y);
    if (x >= 0 and x < width and y >= 0 and y < height) {
        float& p = pheromones[ant->type][x + y*width];
        p = std::min(p + rate, (float)1);
    }
}
