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
}
