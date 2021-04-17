#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <iostream>

#include "settings.h"
#include "math.h"
#include "classes/cObject.h"
#include "classes/cTexture.h"

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

enum AntTypes {
    ANT_EMPTY,
    ANT_TYPES_COUNT
};

class SDL_App {
    private:
        SDL_Window *window = NULL;
        SDL_Renderer *renderer = NULL;

        Texture *textures[TEXTURE_COUNT];
        vector<Object *> ants;
        Object *colony;
        Uint8 feromones[ANT_TYPES_COUNT][SCREEN_WIDTH][SCREEN_HEIGHT];

        void renderAnts();
        void renderFeromones();

        void deflectAnt(Object* ant, float x, float y, int dangerDist, int criticalDist);
        void followFeromones(Object* ant, int area, int maxA);
        void produceFeromones(Object* ant);
        void decayFeromones();
        void wallCollision(Object* ant);

        void destroy();

    public:
        SDL_App();
        ~SDL_App();

        bool initSDL();
        bool loadMedia();
        void initObjects();
        void handleAnts();
        void render();
};

SDL_App::SDL_App() {
    // for some reason class member array has to be initilized manually
    for (auto& f_type: feromones) {
        for (auto& f_x : f_type) {
            for (auto& f_y : f_x) {
                f_y = 0;
            }
        }
    }
}

SDL_App::~SDL_App() {
    destroy();
}

bool SDL_App::initSDL() {
    srand(time(NULL));

    //Initialize SDL
    if(SDL_Init( SDL_INIT_VIDEO ) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize: %s", SDL_GetError());
        return false;
    }

    //Create window
    window = SDL_CreateWindow("Mravenci", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
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
    colony = new Object(textures[TEXTURE_COLONY], 0);
    colony->setValues(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT, 0, 45);

    ants.resize(NUMBER_OF_ANTS);
    for (auto& ant : ants) {
        ant = new Object(textures[TEXTURE_ANT], ANT_EMPTY);
        ant->setValues(colony->pos.x, colony->pos.y, rand() % ANT_RANDOM_SPEED + ANT_MIN_SPEED, rand() % 360);
    }
}

void SDL_App::destroy() {
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;

    for (auto& ant : ants) {
        delete ant;
    }

    for (auto& texture : textures) {
        delete texture;
    }

    SDL_Quit();
    IMG_Quit();
}

void SDL_App::renderFeromones() {
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        for (int j = 0; j < SCREEN_HEIGHT; j++) {
            if (feromones[ANT_EMPTY][i][j] > 0) {
                SDL_SetRenderDrawColor(renderer, 255 - feromones[ANT_EMPTY][i][j], 255 - feromones[ANT_EMPTY][i][j], 255, 255);
                SDL_RenderDrawPoint(renderer, i, j);
            }
        }
    }
}

void SDL_App::renderAnts() {
    for (auto& ant : ants) {
        ant->render(ANT_SCALE_RENDER);
    }
}

void SDL_App::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    renderFeromones();
    renderAnts();
    colony->render();
    SDL_RenderPresent(renderer);
}

void SDL_App::deflectAnt(Object* ant, float x, float y, int dangerDist, int criticalDist) {
    float dx = x - ant->pos.x;
    float dy = y - ant->pos.y;
    if (abs(dx) < dangerDist and abs(dy) < dangerDist) {        
        float dist2 = dx*dx + dy*dy;
        if (dist2 < dangerDist*dangerDist) {   
            float dist = qsqrt(dist2);     

            // cursor position and ant velocity angle
            float dA = calculateAngle(dx, dy, dist) - ant->a;
            if (dA > 180) {
                dA -= 360;
            } else if (dA < -180) {
                dA += 360;
            }

            float diff = (180 - abs(dA))*min((float)1.0, criticalDist/dist);
            if (dA > 0) {
                ant->a -= diff;
            } else if (dA < 0) {
                ant->a += diff;
            }
        }
    }
}

void SDL_App::followFeromones(Object* ant, int area, int maxA) {
    int x = (int)round(ant->pos.x);
    int y = (int)round(ant->pos.y);
    
    float diffA = 0;
    int total = 0;
    
    for (int i = max(0, x-area); i < min(SCREEN_WIDTH, x+area+1); i++) {
        for (int j = max(0, y-area); j < min(SCREEN_HEIGHT, y+area+1); j++) {
            if (feromones[ant->type][i][j] > 0) {
                float dx = i - ant->pos.x;            
                float dy = j - ant->pos.y;

                if (dx != 0 or dy != 0) {
                    float dist = qsqrt(dx*dx + dy*dy);
                    
                    if (dist < area) {
                        float dA = calculateAngle(dx, dy, dist) - ant->a;
                        if (dA > 180) {
                            dA -= 360;
                        } else if (dA < -180) {
                            dA += 360;
                        }

                        if (abs(dA) < maxA) {
                            diffA += dA*feromones[ant->type][i][j];
                            total += feromones[ant->type][i][j];
                        }
                    }
                }
            }
        }
    }

    if (total > 0) {
        ant->a += diffA / total;
    }
}

void SDL_App::decayFeromones() {
    for (int t = 0; t < ANT_TYPES_COUNT; t++) {
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            for (int j = 0; j < SCREEN_HEIGHT; j++) {
                if (feromones[t][i][j] > 0) {
                    if (feromones[t][i][j] < FEROMONE_DECAY) {
                        feromones[t][i][j] = 0;
                    } else {
                        feromones[t][i][j] -= FEROMONE_DECAY;
                    }
                }
            }
        }
    }
}

void SDL_App::produceFeromones(Object* ant) {
    int x = (int)round(ant->pos.x);
    int y = (int)round(ant->pos.y);
    if (x < 0) {x = 0;} else if (x >= SCREEN_WIDTH) {x = SCREEN_WIDTH - 1;}
    if (y < 0) {y = 0;} else if (y >= SCREEN_HEIGHT) {y = SCREEN_HEIGHT - 1;}
    if (feromones[ant->type][x][y] > 255 - FEROMONE_PRODUCTION) {
        feromones[ant->type][x][y] = 255;
    } else {
        feromones[ant->type][x][y] += FEROMONE_PRODUCTION;
    }
}

void SDL_App::handleAnts() {
    // SDL_Point cursor;
    // SDL_GetMouseState(&cursor.x, &cursor.y);

    decayFeromones();
    
    int i;
    #pragma omp parallel default(shared) private(i)
    {
        #pragma omp for schedule(dynamic) nowait
        for (i=0; i<NUMBER_OF_ANTS; i++) {
            // deflectAnt(ants[i], (float)cursor.x, (float)cursor.y, CURSOR_DANGER, CURSOR_CRITICAL);
            wallCollision(ants[i]);
            followFeromones(ants[i], FEROMONES_AREA, FEROMONES_ANGLE);
            ants[i]->randomTurn(MAX_RANDOM_TURN);
            normalizeAngle(ants[i]->a);

            ants[i]->move(STEP_SIZE);
        }
    }

    for (auto ant : ants) {
        produceFeromones(ant);
    }
}

void SDL_App::wallCollision(Object* ant) {
    if (ant->pos.x < 0 or ant->pos.x >= SCREEN_WIDTH) {
        if (ant->pos.x < 0) {ant->pos.x = 0;} else {ant->pos.x = SCREEN_WIDTH-1;}
        ant->a = 180 - ant->a;
    }
    
    if (ant->pos.y < 0 or ant->pos.y >= SCREEN_HEIGHT) {
        if (ant->pos.y < 0) {ant->pos.y = 0;} else {ant->pos.y = SCREEN_HEIGHT-1;}
        ant->a = 360 - ant->a;
    }
}

bool inRange(Object* obj1, Object* obj2, int d) {
    int dx = obj1->pos.x - obj2->pos.x; 
    int dy = obj1->pos.y - obj2->pos.y;
    if (abs(dx) < 25 and abs(dy) < 25) {
        float dist = qsqrt(dx*dx + dy*dy);
        if (dist < 25) {
            return true;
        }
    }
    return false;
}
