#include <omp.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "settings.h"
#include "math.h"
#include "classes/cTexture.h"
#include "classes/cObject.h"
#include "classes/cAnt.h"
#include "classes/cColony.h"
#include "classes/cPheromones.h"
#include "classes/cSoundControll.h"



enum AntTypes {
    ANT_TYPE_EMPTY,
    ANT_TYPES_COUNT
};

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
        SDL_Window *window = NULL;
        SDL_Renderer *renderer = NULL;
        SDL_Point cursorPos;

        SoundControll* soundControll = NULL;

        Texture *textures[TEXTURE_COUNT];
        Colony *colony;
        Pheromones *pheromones;

        void deflectAnt(Ant* ant, float x, float y, int dangerDist, int criticalDist);
        void wallCollision(Ant* ant);

    public:
        SDL_App();
        ~SDL_App();

        bool initSDL(bool enableSoundControll);
        bool loadMedia();
        void initObjects();
        void processData(bool enableMouse);
        void handleAnts(bool enableMouse, bool followAverage, bool follow);
        void render();
};

SDL_App::SDL_App() {

}

SDL_App::~SDL_App() {
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;

    delete colony;
    delete pheromones;
    for (auto& texture : textures) delete texture;

    if (soundControll != NULL) delete soundControll;

    SDL_Quit();
    IMG_Quit();
}

bool SDL_App::initSDL(bool enableSoundControll) {
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

    if (enableSoundControll) soundControll = new SoundControll;

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
    colony = new Colony(textures[TEXTURE_COLONY], NUMBER_OF_ANTS);
    colony->setValues(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT, 0, 45);
    for (auto& ant : colony->ants) {
        ant = new Ant(textures[TEXTURE_ANT]);
        ant->setValues(colony->pos.x, colony->pos.y, rand() % ANT_RANDOM_SPEED + ANT_MIN_SPEED, rand() % 360);
        ant->modify(true, ANT_TYPE_EMPTY, ANT_TYPE_EMPTY);
    }
    pheromones = new Pheromones(renderer, ANT_TYPES_COUNT, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void SDL_App::render() {
    pheromones->render(ANT_TYPE_EMPTY);
    colony->renderAnts(ANT_SCALE_RENDER);
    colony->render();
    SDL_RenderPresent(renderer);
}

void SDL_App::deflectAnt(Ant* ant, float x, float y, int dangerDist, int criticalDist) {
    float dx = x - ant->pos.x;
    float dy = y - ant->pos.y;
    if (abs(dx) < dangerDist and abs(dy) < dangerDist) {
        float dist2 = dx*dx + dy*dy;
        if (dist2 < dangerDist*dangerDist) {
            float idist = iqsqrt(dist2);

            // cursor position and ant velocity angle
            float dA = calculateAngleI(dx, dy, idist) - ant->a;
            if (dA > 180) dA -= 360;
            else if (dA < -180) dA += 360;

            float diff = (180 - abs(dA))*std::min((float)1.0, criticalDist*idist);
            if (dA > 0) ant->a -= diff;
            else if (dA < 0) ant->a += diff;
        }
    }
}

void SDL_App::processData(bool enableMouse) {
    if (soundControll != NULL) soundControll->checkAudio();
    if (enableMouse) SDL_GetMouseState(&cursorPos.x, &cursorPos.y);
}

void SDL_App::handleAnts(bool enableMouse, bool followAverage, bool follow = true) {
    if (soundControll != NULL) follow = soundControll->isLoudSound() < 10;

    pheromones->decay(PHEROMONE_DECAY_RATE);

    int i;
    auto& ants = colony->ants;
    #pragma omp parallel default(shared) private(i)
    {
        #pragma omp for schedule(dynamic) nowait
        for (i=0; i<NUMBER_OF_ANTS; i++) {
            if (rand() % 100 < CHANCE_TO_MOVE and ants[i]->alive) {
                if (enableMouse) deflectAnt(ants[i], (float)cursorPos.x, (float)cursorPos.y, CURSOR_DANGER, CURSOR_CRITICAL);

                if (follow) {
                    if (followAverage) pheromones->followAverage(ants[i], PHEROMONES_DISTANCE, PHEROMONES_ANGLE, PHEROMONES_FOLLOW_STRENGTH);
                    else pheromones->follow(ants[i], PHEROMONES_DISTANCE, PHEROMONES_ANGLE, PHEROMONES_FOLLOW_STRENGTH);
                }

                ants[i]->randomTurn(MAX_RANDOM_TURN);
                normalizeAngle(ants[i]->a);
                ants[i]->move(STEP_SIZE);
                wallCollision(ants[i]); // ensures ants are inside screen area
            }
        }
    }

    for (auto& ant : ants) if (ant->alive) pheromones->produce(ant, PHEROMONE_PRODUCTION_RATE);
}

void SDL_App::wallCollision(Ant* ant) {
    if (ant->pos.x < 0 or ant->pos.x >= SCREEN_WIDTH) {
        ant->pos.x = ant->pos.x < 0 ? 0 : SCREEN_WIDTH-1;
        ant->a = 180 - ant->a;
    }

    if (ant->pos.y < 0 or ant->pos.y >= SCREEN_HEIGHT) {
        ant->pos.y = ant->pos.y < 0 ? 0 : SCREEN_HEIGHT-1;
        ant->a = 360 - ant->a;
    }
}
