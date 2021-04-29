#pragma once
#include <vector>
#include <SDL2/SDL.h>

#include "cAnt.h"



class Pheromones {
    private:
        int width, height;
        std::vector<float* > pheromones;

        SDL_Renderer* renderer;
        SDL_Texture* pheromoneTexture;

    public:
        Pheromones(SDL_Renderer* renderer, int width, int height);
        ~Pheromones();

        void render(AntTypes type);
        void follow(Ant* ant, int area, Uint8 maxA, float strength, BorderMode borderMode);
        void followAverage(Ant* ant, int area, Uint8 maxA, float strength, BorderMode borderMode);
        void decay(float rate);
        void produce(Ant* ant, float rate);
};
