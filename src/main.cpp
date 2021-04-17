#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <omp.h>

#include "app.cpp"
#include "classes/cTimer.h"



int main(int argc, char *argv[]) {

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

    bool measurePerformance = false;
    for (int i=0; i<argc; i++) {
        if (strcmp(argv[i], "--measure-performance")) {
            measurePerformance = true;
        }
    }

    int measureCycles = 0;
    Timer* perfTimer = NULL;
    if (measurePerformance) {
        perfTimer = new Timer();
        perfTimer->start();
    }

    while (!quit) {
        if (perfTimer and perfTimer->getTicks() > 1000) {
            std::cout << "performance " << perfTimer->getTicks()/measureCycles << "ms/cycle" << std::endl;
            perfTimer->start();
            measureCycles = 0;
        }
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
            if (perfTimer) { perfTimer->pause(); }
            SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
        }
        if (perfTimer) {
            perfTimer->unpause();
            measureCycles++;
        }
    }

    if (perfTimer) {
        delete perfTimer;
    }

    return 0;
}
