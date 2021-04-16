#pragma once
#include <string>
#include <SDL2/SDL.h>



SDL_Texture* loadTexture(std::string path, SDL_Renderer *renderer);

class Texture {
    private:
        SDL_Rect sRect;
        SDL_Rect dRect;
        SDL_Texture* texture;
        SDL_Renderer* renderer;
    public:
        Texture(SDL_Renderer* r, std::string path);
        ~Texture();

        void render(int x, int y, float a, float scale = 1);
};
