#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "app.cpp"
#include "settings.hpp"
#include "classes/cAnt.hpp"
#include "classes/cTimer.hpp"



struct Args {
    bool measurePerformance;
    bool enableMouse;
    bool pause;
    std::string settingsPath;
    std::string scenarioName;
    FollowMode followMode;
    BorderMode borderMode;
    SoundMode soundMode;
};

int main(int argc, char *argv[]) {
    Args args;
    args.measurePerformance = false;
    args.enableMouse = false;
    args.pause = false;
    args.followMode = FOLLOW_COUNT;
    args.borderMode = BORDER_BOUNCE;
    args.soundMode = SOUND_OFF;
    args.settingsPath = "settings.json";
    args.scenarioName = "default";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--settings") == 0) {
            i++;
            if (i >= argc) {std::cout << "Missing settings path" << std::endl; return 1;}
            args.settingsPath = argv[i];
        }
        else if (strcmp(argv[i], "--scenario") == 0) {
            i++;
            if (i >= argc) {std::cout << "Missing scenario name" << std::endl; return 1;}
            args.scenarioName = argv[i];
        }
        else if (strcmp(argv[i], "--measure-performance") == 0) args.measurePerformance = true;
        else if (strcmp(argv[i], "--enable-mouse") == 0) args.enableMouse = true;
        else if (strcmp(argv[i], "--follow-mode") == 0) {
            i++;
            if (i >= argc) {std::cout << "Missing follow mode value" << std::endl; return 1;}
            else if (strcmp(argv[i], "count") == 0) args.followMode = FOLLOW_COUNT;
            else if (strcmp(argv[i], "average") == 0) args.followMode = FOLLOW_AVEARGE;
            else {std::cout << "Unknown follow mode value. Allowed values: 'count', 'average'" << std::endl; return 1;}
        }
        else if (strcmp(argv[i], "--border-mode") == 0) {
            i++;
            if (i >= argc) {std::cout << "Missing border mode value" << std::endl; return 1;}
            else if (strcmp(argv[i], "bounce") == 0) args.borderMode = BORDER_BOUNCE;
            else if (strcmp(argv[i], "through") == 0) args.borderMode = BORDER_THROUGH;
            else if (strcmp(argv[i], "kill") == 0) args.borderMode = BORDER_KILL;
            else {std::cout << "Unknown border mode value. Allowed values: 'bounce', 'through', 'kill'" << std::endl; return 1;}
        }
        else if (strcmp(argv[i], "--sound-mode") == 0) {
            i++;
            if (i >= argc) {std::cout << "Missing border mode value" << std::endl; return 1;}
            else if (strcmp(argv[i], "off") == 0) args.soundMode = SOUND_OFF;
            else if (strcmp(argv[i], "shock") == 0) args.soundMode = SOUND_SHOCK;
            else if (strcmp(argv[i], "speed") == 0) args.soundMode = SOUND_SPEED;
            else {std::cout << "Unknown sound mode value. Allowed values: 'off', 'shock', 'speed'" << std::endl; return 1;}
        }
        else if (strcmp(argv[i], "--pause") == 0) args.pause = true;
        else {
            if (strcmp(argv[i], "--help") != 0)
                std::cout << "Unknown flag: " << argv[i] << std::endl;

            std::cout << "Available flags:" << std::endl;
            std::cout << "   --settings [PATH]" << std::endl;
            std::cout << "   --scenario [NAME]" << std::endl;
            std::cout << "   --sound-control" << std::endl;
            std::cout << "   --measure-performance" << std::endl;
            std::cout << "   --enable-mouse" << std::endl;
            std::cout << "   --follow-mode [count, average]" << std::endl;
            std::cout << "   --border-mode [bounce, through, kill]" << std::endl;
            std::cout << "   --pause" << std::endl;
            std::cout << "   --help" << std::endl;

            return 0;
        }
    }

    SettingsStruct settings;
    settings.load(args.settingsPath);

    SDL_App app(settings);

    if(!app.initSDL(args.soundMode > 0)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialization failed");
        return 3;
    }

    if(!app.loadMedia()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load media");
        return 3;
    }

    app.initObjects(args.scenarioName);

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

        app.processData(args.enableMouse);
        if (!args.pause)
            for (int i=0; i<settings.steps_per_frame; i++) {
                app.handleAnts(args.enableMouse, follow and followCycles % settings.pheromones.follow_step == 0, args.followMode, args.borderMode, args.soundMode);
                if (followCycles == settings.pheromones.follow_step) followCycles = 1; else followCycles++;
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
        if (frameTicks < settings._screen_ticks_per_frame)
            SDL_Delay(settings._screen_ticks_per_frame - frameTicks);
    }

    if (perfTimer) delete perfTimer;

    return 0;
}
