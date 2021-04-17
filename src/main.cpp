#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <omp.h>

#include "app.cpp"
#include "classes/cTimer.h"



int main(int argc, char *argv[]) {
    bool measurePerformance = false;
    for (int i=0; i<argc; i++) {
        if (strcmp(argv[i], "--measure-performance")) {
            measurePerformance = true;
        }
    }

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

    int measureCycles = 0;
    int calculationTime = 0;
    int renderingTime = 0;
    Timer* perfTimer = NULL;
    if (measurePerformance) {
        perfTimer = new Timer();
        perfTimer->start();
    }

    while (!quit) {
        capTimer.start();
        if (perfTimer and measureCycles > 10) {
            std::cout << "(ms) calculation: " << calculationTime/measureCycles;
            std::cout << " rendering: " << renderingTime/measureCycles;
            std::cout << " total: " << (calculationTime + renderingTime)/measureCycles << std::endl;
            calculationTime = 0;
            renderingTime = 0;
            measureCycles = 0;
        }

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
        
        if (perfTimer) { perfTimer->start(); }
        for (int i=0; i<STEPS_PER_FRAME; i++) {
            app.handleAnts();
        }
        if (perfTimer) {
            calculationTime += perfTimer->getTicks();
            perfTimer->start();
        }
        app.render();
        if (perfTimer) {
            renderingTime += perfTimer->getTicks();
            measureCycles ++;
        }

        int frameTicks = capTimer.getTicks();
        if (frameTicks < SCREEN_TICKS_PER_FRAME) {
            SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
        }
    }

    if (perfTimer) {
        delete perfTimer;
    }

    return 0;
}
