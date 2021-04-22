#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "app.cpp"
#include "classes/cTimer.h"



struct Args {
    bool measurePerformance;
    bool enableMouse;
    bool pause;
    bool followAverage;
};

int main(int argc, char *argv[]) {
    Args args;
    args.measurePerformance = false;
    args.enableMouse = false;
    args.pause = false;
    args.followAverage = false;

    for (int i=0; i<argc; i++) {
        if (strcmp(argv[i], "--measure-performance") == 0) { args.measurePerformance = true; }
        if (strcmp(argv[i], "--enable-mouse") == 0) { args.enableMouse = true; }
        if (strcmp(argv[i], "--follow-average") == 0) { args.followAverage = true; }
        if (strcmp(argv[i], "--pause") == 0) { args.pause = true; }
        if (strcmp(argv[i], "--help") == 0) {
            std::cout << "Available flags:" << std::endl;
            std::cout << "   --measure-performance" << std::endl;
            std::cout << "   --enable-mouse" << std::endl;
            std::cout << "   --follow-average" << std::endl;
            std::cout << "   --pause" << std::endl;
            std::cout << "   --help" << std::endl;

            return 0;
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

    int measureCycles = 0;
    int calculationTime = 0;
    int renderingTime = 0;
    Timer* perfTimer = NULL;
    if (args.measurePerformance) {
        perfTimer = new Timer();
        perfTimer->start();
    }

    Timer capTimer;

    bool quit = false;
    bool follow = true;
    int followCycles = 0;
    SDL_Event e;
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
            switch (e.type) {
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) {
                        case SDLK_RETURN:
                            follow = false;
                            break;

                        case SDLK_ESCAPE:
                            quit = true;
                            break;

                        case SDLK_SPACE:
                            args.pause = !args.pause;
                            break;
                    }
                    break;

                case SDL_KEYUP:
                    switch (e.key.keysym.sym) {
                        case SDLK_RETURN:
                            follow = true;
                            break;
                    }
                    break;

                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }

        if (perfTimer) perfTimer->start();

        if (!args.pause)
            for (int i=0; i<STEPS_PER_FRAME; i++) { 
                app.handleAnts(args.enableMouse, args.followAverage, follow and followCycles % PHEROMONES_FOLLOW_STEP == 0);
                if (followCycles == PHEROMONES_FOLLOW_STEP) followCycles = 1; else followCycles++;
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
        if (frameTicks < SCREEN_TICKS_PER_FRAME)
            SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
    }

    if (perfTimer) delete perfTimer;

    return 0;
}
