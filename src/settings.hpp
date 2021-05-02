#pragma once
#include <string>



struct AntSettingsStruct {
    float render_scale; // size of rendered ant (of original image size)
    int max_population; // ant population
    int speed; // minimum speed
    int speed_variation; // random speed increase
    int max_random_turn; // max angle of ants random turn
    char chance_to_move; // 0-100 percent
    int revive_rate; // max number of ants revived in step
};

struct PheromonesSettingsStruct {
    float screen_resolution;
    float production_rate; // pheromone amount ant produces every step
    float decay_rate; // amount of decay of pheromones every step
    int follow_distance; // distance to which ant smells pheromones
    int follow_angle; // 0-180 +/- angle in front of ant where he smels pheromones
    float follow_strength; // .0-1.
    int follow_step; // follow pheromones every Nth step
};

struct SettingsStruct {
    int screen_width;
    int screen_height;
    int fps;
    int _screen_ticks_per_frame;
    
    float time_step;
    int steps_per_frame;

    int cursor_danger_distance;
    int cursor_critical_distance;

    int sound_control_disturb_level;

    AntSettingsStruct ant;
    PheromonesSettingsStruct pheromones;

    void load(const std::string &filename);
};
