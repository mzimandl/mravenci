#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <omp.h>

#include "app.cpp"
#include "classes/cTimer.h"

using namespace std;



int main(int argc, char* args[]) {

    SDL_App app;

    if(!app.initSDL()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialization failed");
        return 3;
    }

    if(!app.loadMedia()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load media");
        return 3;
    }

    app.initObjects();

    bool quit = false;
    SDL_Event e;
    Timer capTimer;

    while (!quit) {
        capTimer.start();

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
        
        for (int i=0; i<EVOLUTIONS_PER_FRAME; i++) {
            app.handleAnts();
        }
        app.render();

        int frameTicks = capTimer.getTicks();
        if (frameTicks < SCREEN_TICKS_PER_FRAME) {
            SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
        }
    }

    return 0;
}
