#pragma once
#include <string>
#include <boost/property_tree/ptree.hpp>



struct Scenario {
    int feromoneTypes;
    boost::property_tree::ptree objects;

    void load(const std::string &filename, const std::string &name);
};
