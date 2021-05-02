#include "settings.hpp"

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>



void SettingsStruct::load(const std::string &filename) {
    boost::property_tree::ptree root;
    boost::property_tree::read_json(filename, root);

    screen_width = root.get<int>("screen_width");
    screen_height = root.get<int>("screen_height");
    fps = root.get<int>("fps");
    _screen_ticks_per_frame = 1000/fps;

    time_step = root.get<float>("time_step");
    steps_per_frame = root.get<int>("steps_per_frame");

    cursor_danger_distance = root.get<int>("cursor_danger_distance");
    cursor_critical_distance = root.get<int>("cursor_critical_distance");

    sound_control_disturb_level = root.get<int>("sound_control_disturb_level");

    // ants settings
    ant.render_scale = root.get<float>("ant.render_scale");
    ant.max_population = root.get<int>("ant.max_population");
    ant.speed = root.get<int>("ant.speed");
    ant.speed_variation = root.get<int>("ant.speed_variation");
    ant.max_random_turn = root.get<int>("ant.max_random_turn");
    ant.chance_to_move = root.get<int>("ant.chance_to_move");
    ant.revive_rate = root.get<int>("ant.revive_rate");

    // pheromones settings
    pheromones.screen_resolution = root.get<float>("pheromones.screen_resolution");
    pheromones.production_rate = root.get<float>("pheromones.production_rate");
    pheromones.decay_rate = root.get<float>("pheromones.decay_rate");
    pheromones.follow_distance = root.get<int>("pheromones.follow_distance");
    pheromones.follow_angle = root.get<int>("pheromones.follow_angle");
    pheromones.follow_strength = root.get<float>("pheromones.follow_strength");
    pheromones.follow_step = root.get<int>("pheromones.follow_step");
}
