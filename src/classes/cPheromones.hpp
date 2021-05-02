#pragma once
#include <vector>
#include <SDL2/SDL.h>



class Pheromones {
    private:
        SDL_Renderer* renderer;
        SDL_Texture* texture;

    public:
        int width, height;
        float size;
        std::vector<float* > value;

        Pheromones(SDL_Renderer* r, int screenW, int screenH, float s, int typesCount);
        ~Pheromones();

        void render(int type);
        void decay(float rate);
};
