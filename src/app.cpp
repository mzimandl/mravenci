#include <omp.h>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "settings.h"
#include "math.h"
#include "classes/cTexture.h"
#include "classes/cAnt.h"
#include "classes/cColony.h"
#include "classes/cPheromones.h"
#include "classes/cSoundControl.h"



enum TextureNames {
    TEXTURE_ANT,
    TEXTURE_COLONY,
    TEXTURE_COUNT
};

const std::string TEXTURE_PATHS[TEXTURE_COUNT] = {
    "data/img/ant.png",
    "data/img/colony.png"
};

class SDL_App {
    private:
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Point cursorPos;

        SoundControl* soundControl;

        Texture *textures[TEXTURE_COUNT];
        Colony *colony;
        Pheromones *pheromones;

    public:
        SDL_App();
        ~SDL_App();

        bool initSDL(bool enableSoundControl);
        bool loadMedia();
        void initObjects();
        void processData(bool enableMouse);
        void handleAnts(bool enableMouse, bool follow, FollowMode followMode, BorderMode borderMode);
        void render();
};

SDL_App::SDL_App() :
window(NULL), renderer(NULL), soundControl(NULL), colony(NULL), pheromones(NULL)
{}

SDL_App::~SDL_App() {
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;

    delete colony; colony = NULL;
    delete pheromones; pheromones = NULL;
    for (auto& texture : textures) {
        delete texture; texture = NULL;
    }

    if (soundControl != NULL) {
        delete soundControl; soundControl = NULL;
    }

    SDL_Quit();
    IMG_Quit();
}

bool SDL_App::initSDL(bool enableSoundControl) {
    srand(time(NULL));

    //Initialize SDL
    if(SDL_Init( SDL_INIT_VIDEO|SDL_INIT_AUDIO ) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize: %s", SDL_GetError());
        return false;
    }

    //Create window
    window = SDL_CreateWindow("Mravenci", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window: %s", SDL_GetError());
        return false;
    }

    //Create renderer
    renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create renderer: %s", SDL_GetError());
        return false;
    }

    //Init SDL image
    int imgFlags = IMG_INIT_PNG|IMG_INIT_JPG;
    int initted = IMG_Init(imgFlags);
    if((initted&imgFlags) != imgFlags) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "IMG_Init: Failed to init required jpg and png support: %s", IMG_GetError());
        return false;
    }

    if (enableSoundControl) soundControl = new SoundControl;

    return true;
}

bool SDL_App::loadMedia() {
    for (int i = 0; i < TEXTURE_COUNT; i++) {
        textures[i] = new Texture(renderer, TEXTURE_PATHS[i]);
        if (textures[i] == NULL) return false;
    }

    return true;
}

void SDL_App::initObjects() {
    colony = new Colony(textures[TEXTURE_COLONY], ANT_MAX_POPULATION);
    colony->setPos(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
    for (auto& ant : colony->ants) ant = new Ant(textures[TEXTURE_ANT]);
    pheromones = new Pheromones(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, ANT_TYPES_COUNT);
}

void SDL_App::render() {
    pheromones->render(ANT_TYPE_EMPTY);
    colony->renderAnts(ANT_SCALE_RENDER);
    colony->render();
    SDL_RenderPresent(renderer);
}

void SDL_App::processData(bool enableMouse) {
    if (soundControl != NULL) soundControl->checkAudio();
    if (enableMouse) SDL_GetMouseState(&cursorPos.x, &cursorPos.y);
}

void SDL_App::handleAnts(bool enableMouse, bool follow, FollowMode followMode, BorderMode borderMode) {
    int soundCorrection = 0;
    if (soundControl != NULL) {
        soundCorrection = std::max(0, soundControl->avgLevelVariation());
        follow = soundCorrection < SOUND_CONTROL_DISTURB_LEVEL and follow;
    }

    pheromones->decay(PHEROMONE_DECAY_RATE);
    
    if (borderMode == BORDER_KILL) colony->checkPopulation();
    colony->reviveAnts(ANT_REVIVE_RATE, ANT_SPEED, ANT_SPEED_VARIATION);

    std::vector<Ant *>::iterator i;
    #pragma omp parallel default(shared) private(i)
    {
        #pragma omp for schedule(dynamic) nowait
        for (i = colony->ants.begin(); i != colony->ants.end(); i++) {
            Ant* ant = *i;
            ant->moving = rand() % 100 < ANT_CHANCE_TO_MOVE;
            if (ant->alive and ant->moving) {
                if (enableMouse) ant->deflect((float)cursorPos.x, (float)cursorPos.y, CURSOR_DANGER, CURSOR_CRITICAL);
                if (follow) switch (followMode) {
                    case FOLLOW_COUNT:
                        ant->followPhCount(pheromones, PHEROMONES_DISTANCE, PHEROMONES_ANGLE, PHEROMONES_FOLLOW_STRENGTH, borderMode);
                        break;

                    case FOLLOW_AVEARGE:
                        ant->followPhAverage(pheromones, PHEROMONES_DISTANCE, PHEROMONES_ANGLE, PHEROMONES_FOLLOW_STRENGTH, borderMode);
                        break; 
                }

                ant->randomTurn(ANT_RANDOM_TURN + soundCorrection);
                normalizeAngle(ant->vel.a);
                ant->move(STEP_SIZE + 0.1*(float)soundCorrection/(float)UINT8_MAX);
                ant->checkWallCollision(SCREEN_WIDTH, SCREEN_HEIGHT, borderMode);
            }
        }
    }

    for (auto& ant : colony->ants) if (ant->alive and ant->moving) ant->producePh(pheromones, PHEROMONE_PRODUCTION_RATE);
}
