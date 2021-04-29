#pragma once
#include <vector>
#include <SDL2/SDL.h>



class Pheromones {
    private:
        SDL_Renderer* renderer;
        SDL_Texture* texture;

    public:
        int width, height;
        std::vector<float* > value;

        Pheromones(SDL_Renderer* renderer, int width, int height, int typesCount);
        ~Pheromones();

        void render(int type);
        void decay(float rate);
};
