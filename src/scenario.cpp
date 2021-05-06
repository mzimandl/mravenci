#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "scenario.hpp"



void Scenario::load(const std::string &filename, const std::string &name) {
    boost::property_tree::ptree root;
    boost::property_tree::read_json(filename, root);
    
    feromoneTypes = root.get<int>(name + ".pheromoneTypes");
    objects = root.get_child(name + ".objects");
}
