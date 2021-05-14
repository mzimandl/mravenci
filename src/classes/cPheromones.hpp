#pragma once
#include <vector>
#include <SDL2/SDL.h>



class Pheromones {
    private:
        SDL_Renderer* renderer;
        std::vector<SDL_Texture* > textures;

    public:
        int width, height;
        float size;
        std::vector<float* > value;

        Pheromones(SDL_Renderer* r, int screenW, int screenH, float s, int typesCount);
        ~Pheromones();

        void render(int type);
        void renderAll();
        void decay(float rate);
};
