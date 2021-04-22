#include <string>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "cTexture.h"



SDL_Texture* loadTexture(std::string path, SDL_Renderer *renderer) {
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL ) {
        std::cout << "Unable to load image: " << path << std::endl;
        std::cout << IMG_GetError() << std::endl;

    } else {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if( newTexture == NULL ) {
            std::cout << "Unable to create texture from: " << path << std::endl;
            std::cout << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(loadedSurface);
    }

    return newTexture;
}

Texture::Texture(SDL_Renderer* r, std::string path) {
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
