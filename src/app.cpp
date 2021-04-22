#include <iostream>
#include <vector>
#include <omp.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "settings.h"
#include "math.h"
#include "classes/cTexture.h"
#include "classes/cObject.h"
#include "classes/cAnt.h"
#include "classes/cColony.h"

using namespace std;



enum TextureNames {
    TEXTURE_ANT,
    TEXTURE_COLONY,
    TEXTURE_COUNT
};

const string TEXTURE_PATHS[TEXTURE_COUNT] = {
    "data/img/ant.png",
    "data/img/colony.png"
};

class SDL_App {
    private:
        SDL_Window *window = NULL;
        SDL_Renderer *renderer = NULL;
        SDL_Point cursorPos;

        Texture *textures[TEXTURE_COUNT];
        Colony *colony;

        float *pheromones[ANT_TYPES_COUNT];
        SDL_Texture* pheromoneTexture;

        void renderAnts();
        void renderPheromones();

        void deflectAnt(Ant* ant, float x, float y, int dangerDist, int criticalDist);
        void followPheromones(Ant* ant, int area, int maxA);
        void followPheromonesAverage(Ant* ant, int area, int maxA);
        void producePheromones(Ant* ant);
        void decayPheromones();
        void wallCollision(Ant* ant);

        void destroy();

    public:
        SDL_App();
        ~SDL_App();

        bool initSDL();
        bool loadMedia();
        void initObjects();
        void handleAnts(bool enableMouse, bool followAverage, bool follow);
        void render();
};

SDL_App::SDL_App() {
    for (auto& f : pheromones) { f = new float[SCREEN_WIDTH*SCREEN_HEIGHT]; }
}

SDL_App::~SDL_App() {
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;

    SDL_DestroyTexture(pheromoneTexture);
    pheromoneTexture = NULL;

    delete colony;
    for (auto& texture : textures) { delete texture; }
    for (auto& f : pheromones) { delete [] f; }

    SDL_Quit();
    IMG_Quit();
}

bool SDL_App::initSDL() {
    srand(time(NULL));

    //Initialize SDL
    if(SDL_Init( SDL_INIT_VIDEO ) < 0) {
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
    int flags = IMG_INIT_PNG|IMG_INIT_JPG;
    int initted = IMG_Init(flags);
    if((initted&flags) != flags) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "IMG_Init: Failed to init required jpg and png support: %s", IMG_GetError());
        return false;
    }

    return true;
}

bool SDL_App::loadMedia() {
    for (int i = 0; i < TEXTURE_COUNT; i++) {
        textures[i] = new Texture(renderer, TEXTURE_PATHS[i]);
        if (textures[i] == NULL) {return false;}
    }

    return true;
}

void SDL_App::initObjects() {
    pheromoneTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    Uint8* pixels;
    int pitch;

    SDL_LockTexture(pheromoneTexture, NULL, (void**)&pixels, &pitch);
    for (int i = 0; i < pitch*SCREEN_HEIGHT; i++) {
        pixels[i] = 255;
    }
    SDL_UnlockTexture(pheromoneTexture);

    colony = new Colony(textures[TEXTURE_COLONY], NUMBER_OF_ANTS);
    colony->setValues(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT, 0, 45);
    for (auto& ant : colony->ants) {
        ant = new Ant(textures[TEXTURE_ANT]);
        ant->setValues(colony->pos.x, colony->pos.y, rand() % ANT_RANDOM_SPEED + ANT_MIN_SPEED, rand() % 360);
        ant->modify(true, ANT_TYPE_EMPTY, ANT_TYPE_EMPTY);
    }
}

void SDL_App::renderPheromones() {
    Uint8* pixels;
    int pitch;

    SDL_LockTexture(pheromoneTexture, NULL, (void**)&pixels, &pitch);

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            if (pheromones[ANT_TYPE_EMPTY][x + y*SCREEN_WIDTH] > 0) {
                int intensity = (int)(255*pheromones[ANT_TYPE_EMPTY][x + y*SCREEN_WIDTH]);
                pixels[4*x + pitch*y + 1] = 255 - intensity;
                pixels[4*x + pitch*y + 2] = 255 - intensity;
            }
        }
    }
    SDL_UnlockTexture(pheromoneTexture);
    SDL_RenderCopy(renderer, pheromoneTexture, NULL, NULL);
}

void SDL_App::render() {
    renderPheromones();
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
            if (dA > 180) {
                dA -= 360;
            } else if (dA < -180) {
                dA += 360;
            }

            float diff = (180 - abs(dA))*min((float)1.0, criticalDist*idist);
            if (dA > 0) {
                ant->a -= diff;
            } else if (dA < 0) {
                ant->a += diff;
            }
        }
    }
}

void SDL_App::followPheromones(Ant* ant, int area, int maxA) {
    int x = (int)round(ant->pos.x);
    int y = (int)round(ant->pos.y);

    const int area2 = area*area;
    const float arc = 2*(float)maxA/3;
    int count1 = 0, count2 = 0, count3 = 0;

    for (int j = max(0, y-area); j < min(SCREEN_HEIGHT, y+area+1); j++) {
        for (int i = max(0, x-area); i < min(SCREEN_WIDTH, x+area+1); i++) {
            if (pheromones[ant->follow][i + j*SCREEN_WIDTH] > 0) {
                float dx = i - ant->pos.x;
                float dy = j - ant->pos.y;

                if (dx != 0 or dy != 0) {
                    float dist2 = dx*dx + dy*dy;
                    if (dist2 < area2) {
                        float dA = calculateAngleI(dx, dy, iqsqrt(dist2)) - ant->a;
                        if (dA > 180) {
                            dA -= 360;
                        } else if (dA < -180) {
                            dA += 360;
                        }

                        if (abs(dA) < maxA) {
                            if (dA < -arc/2) { count1++; }
                            else if (dA > arc/2) { count3++; }
                            else { count2++; }
                        }
                    }
                }
            }
        }
    }

    if (count1 or count2 or count3) {
        if (count1 > count2 and count1 > count3) { ant->a -= PHEROMONES_STRENGTH*arc; }
        else if (count3 > count1 and count3 > count2) { ant->a += PHEROMONES_STRENGTH*arc; }
        else if (count1 == count2 < count3) { ant->a -= PHEROMONES_STRENGTH*arc/2; }
        else if (count1 < count2 == count3) { ant->a += PHEROMONES_STRENGTH*arc/2; }
    }
}

void SDL_App::followPheromonesAverage(Ant* ant, int area, int maxA) {
    int x = (int)round(ant->pos.x);
    int y = (int)round(ant->pos.y);

    const int area2 = area*area;

    float diffA = 0;
    float total = 0;

    for (int j = max(0, y-area); j < min(SCREEN_HEIGHT, y+area+1); j++) {
        for (int i = max(0, x-area); i < min(SCREEN_WIDTH, x+area+1); i++) {
            const int index = i + j*SCREEN_WIDTH;
            if (pheromones[ant->follow][index] > 0) {
                float dx = i - ant->pos.x;
                float dy = j - ant->pos.y;

                if (dx != 0 or dy != 0) {
                    float dist2 = dx*dx + dy*dy;
                    if (dist2 < area2) {
                        float dA = calculateAngleI(dx, dy, iqsqrt(dist2)) - ant->a;
                        if (dA > 180) {
                            dA -= 360;
                        } else if (dA < -180) {
                            dA += 360;
                        }

                        if (abs(dA) < maxA) {
                            diffA += dA*pheromones[ant->follow][index];
                            total += pheromones[ant->follow][index];
                        }
                    }
                }
            }
        }
    }

    if (total > 0) { ant->a += PHEROMONES_STRENGTH * diffA / total; }
}

void SDL_App::decayPheromones() {
    for (int t = 0; t < ANT_TYPES_COUNT; t++) {
        for (int y = 0; y < SCREEN_WIDTH*SCREEN_HEIGHT; y+=SCREEN_WIDTH) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                if (pheromones[t][x + y] > 0) {
                    pheromones[t][x + y] -= PHEROMONE_DECAY;
                    if (pheromones[t][x + y] < 0) { pheromones[t][x + y] = 0; }
                }
            }
        }
    }
}

void SDL_App::producePheromones(Ant* ant) {
    int x = (int)round(ant->pos.x);
    int y = (int)round(ant->pos.y);
    if (x >= 0 and x < SCREEN_WIDTH and y >= 0 and y < SCREEN_HEIGHT) {
        const int index = x + y*SCREEN_WIDTH;
        pheromones[ant->type][index] += PHEROMONE_PRODUCTION;
        if (pheromones[ant->type][index] > 1) { pheromones[ant->type][index] = 1; }
    }
}

void SDL_App::handleAnts(bool enableMouse, bool followAverage, bool follow = true) {
    if (enableMouse) {
        SDL_GetMouseState(&cursorPos.x, &cursorPos.y);
    }

    decayPheromones();

    int i;
    auto& ants = colony->ants;
    #pragma omp parallel default(shared) private(i)
    {
        #pragma omp for schedule(dynamic) nowait
        for (i=0; i<NUMBER_OF_ANTS; i++) {
            if (rand() % 100 < CHANCE_TO_MOVE and ants[i]->alive) {
                if (enableMouse) {
                    deflectAnt(ants[i], (float)cursorPos.x, (float)cursorPos.y, CURSOR_DANGER, CURSOR_CRITICAL);
                }

                if (follow) {
                    if (followAverage) { followPheromonesAverage(ants[i], PHEROMONES_DISTANCE, PHEROMONES_ANGLE); }
                    else { followPheromones(ants[i], PHEROMONES_DISTANCE, PHEROMONES_ANGLE); }
                }

                ants[i]->randomTurn(MAX_RANDOM_TURN);
                normalizeAngle(ants[i]->a);
                ants[i]->move(STEP_SIZE);
                wallCollision(ants[i]); // ensures ants are inside screen area
            }
        }
    }

    for (auto& ant : ants) if (ant->alive) producePheromones(ant);
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
