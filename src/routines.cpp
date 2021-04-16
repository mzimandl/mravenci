#pragma once
#include <string>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


SDL_Surface* loadSurface(std::string path, SDL_PixelFormat *format)
{
    SDL_Surface* optimizedSurface = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        std::cout << "Unable to load image: " << path << std::endl;
        std::cout << IMG_GetError() << std::endl;
    }
    else
    {
        optimizedSurface = SDL_ConvertSurface(loadedSurface, format, 0);
        if( optimizedSurface == NULL )
        {
            std::cout << "Unable to optimize image: " << path << std::endl;
            std::cout << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(loadedSurface);
    }
    
    return optimizedSurface;
}

SDL_Texture* loadTexture(std::string path, SDL_Renderer *renderer)
{
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        std::cout << "Unable to load image: " << path << std::endl;
        std::cout << IMG_GetError() << std::endl;
    }
    else
    {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if( newTexture == NULL )
        {
            std::cout << "Unable to create texture from: " << path << std::endl;
            std::cout << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(loadedSurface);
    }
    
    return newTexture;
}
