#include <omp.h>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "settings.hpp"
#include "scenario.hpp"
#include "math.hpp"
#include "classes/cTexture.hpp"
#include "classes/cAnt.hpp"
#include "classes/cColony.hpp"
#include "classes/cPheromones.hpp"
#include "classes/cSoundControl.hpp"



enum TextureNames {
    TEXTURE_ANT,
    TEXTURE_COLONY,
    TEXTURE_FOOD,
    TEXTURE_COUNT
};

const std::string TEXTURE_PATHS[TEXTURE_COUNT] = {
    "data/img/ant.png",
    "data/img/colony.png",
    "data/img/food.png",
};

class SDL_App {
    private:
        SettingsStruct &settings;

        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Point cursorPos;

        SoundControl* soundControl;

        Texture *textures[TEXTURE_COUNT];
        std::vector<Colony *> colonies;
        std::vector<AntObject *> foods;
        Pheromones *pheromones;

    public:
        SDL_App(SettingsStruct &s);
        ~SDL_App();

        bool initSDL(bool enableSoundControl);
        bool loadMedia();
        void initObjects(std::string scenarioName);
        void processData(bool enableMouse);
        void handleAnts(bool enableMouse, bool follow, FollowMode followMode, BorderMode borderMode);
        void render();
};

SDL_App::SDL_App(SettingsStruct &s) :
window(NULL), renderer(NULL), soundControl(NULL), pheromones(NULL), settings(s)
{}

SDL_App::~SDL_App() {
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;

    for (auto& colony : colonies) {
        delete colony; colony = NULL;
    }

    for (auto& food : foods) {
        delete food; food = NULL;
    }

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
    window = SDL_CreateWindow("Mravenci", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, settings.screen_width, settings.screen_height, SDL_WINDOW_SHOWN);
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

void SDL_App::initObjects(std::string scenarioName) {
    Scenario scenario;
    scenario.load("scenario.json", scenarioName);
    
    for (boost::property_tree::ptree::value_type &object : scenario.objects) {
        std::string objectType = object.second.get<std::string>("type");
        if (objectType.compare("colony") == 0) {
            colonies.push_back(new Colony(
                textures[TEXTURE_COLONY],
                textures[TEXTURE_ANT],
                object.second.get<int>("population"),
                object.second.get<float>("radius"),
                object.second.get<int>("antType"),
                object.second.get<bool>("antChange")
            ));
            colonies.back()->setPos(rand() % settings.screen_width, rand() % settings.screen_height);

            for (boost::property_tree::ptree::value_type &follow : object.second.get_child("antFollow"))
                colonies.back()->followMap[std::stoi(follow.first)] = follow.second.get_value<int>();

        } else if (objectType.compare("food") == 0) {
            foods.push_back(new AntObject(
                textures[TEXTURE_FOOD],
                object.second.get<float>("radius"),
                object.second.get<int>("antType"),
                object.second.get<bool>("antChange")
            ));
            foods.back()->setPos(rand() % settings.screen_width, rand() % settings.screen_height);
        }
    }

    pheromones = new Pheromones(renderer, settings.screen_width, settings.screen_height, settings.pheromones.screen_resolution, scenario.feromoneTypes);
}

void SDL_App::render() {
    pheromones->render(0);
    for (auto &colony : colonies) colony->renderAnts(settings.ant.render_scale);
    for (auto &colony : colonies) colony->render();
    for (auto &food : foods) food->render();
    SDL_RenderPresent(renderer);
}

void SDL_App::processData(bool enableMouse) {
    if (soundControl != NULL) soundControl->checkAudio();
    if (enableMouse) SDL_GetMouseState(&cursorPos.x, &cursorPos.y);
}

void SDL_App::handleAnts(bool enableMouse, bool follow, FollowMode followMode, BorderMode borderMode) {
    float timeStep = settings.time_step;
    float randomTurn = settings.ant.max_random_turn;

    if (soundControl != NULL) {
        int volumeIncrease = std::max(0, soundControl->avgVolumeChange());
        follow = volumeIncrease < settings.sound_control_disturb_level and follow;
        timeStep += 0.1*(float)volumeIncrease/(float)UINT8_MAX;
        randomTurn += volumeIncrease;

        //timeStep = 0.005*soundControl->lastVolume();
    }

    pheromones->decay(settings.pheromones.decay_rate);
    
    for (auto &colony : colonies) {
        if (borderMode == BORDER_KILL) colony->checkPopulation();
        colony->reviveAnts(settings.ant.revive_rate, settings.ant.speed, settings.ant.speed_variation);

        std::vector<Ant *>::iterator i;
        #pragma omp parallel default(shared) private(i)
        {
            #pragma omp for schedule(dynamic) nowait
            for (i = colony->ants.begin(); i != colony->ants.end(); i++) {
                Ant* ant = *i;
                ant->moving = rand() % 100 < settings.ant.chance_to_move;
                if (ant->alive and ant->moving) {
                    if (enableMouse) ant->deflect((float)cursorPos.x, (float)cursorPos.y, settings.cursor_danger_distance, settings.cursor_critical_distance);
                    if (follow) switch (followMode) {
                        case FOLLOW_COUNT:
                            ant->followPhCount(pheromones, settings.pheromones.follow_distance, settings.pheromones.follow_angle, settings.pheromones.follow_strength, borderMode);
                            break;

                        case FOLLOW_AVEARGE:
                            ant->followPhAverage(pheromones, settings.pheromones.follow_distance, settings.pheromones.follow_angle, settings.pheromones.follow_strength, borderMode);
                            break; 
                    }

                    ant->randomTurn(randomTurn);
                    normalizeAngle(ant->angle);
                    ant->move(timeStep);
                    ant->checkWallCollision(settings.screen_width, settings.screen_height, borderMode);

                    if (colony->antChange and ant->type != colony->antType and colony->inRange(ant)) {
                        ant->type = colony->antType;
                        colony->updateFollowType(ant);
                    }

                    for (auto &food : foods) if (food->antChange and food->inRange(ant)) {
                        ant->type = food->antType;
                        colony->updateFollowType(ant);
                    }
                }
            }
        }

        colony->producePh(pheromones, settings.pheromones.production_rate);
    }
}
