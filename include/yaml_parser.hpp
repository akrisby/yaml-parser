#pragma once

#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <vector>
#include <ostream>

namespace yaml_parser
{

    // Responsible for loading YAML files (with include support) and providing access to the parsed node.
    class YamlParser
    {
    public:
        YamlParser();

        // Load YAML configuration from file (resolves !include)
        bool loadConfig(const std::string &configFile);

        // Return the loaded YAML root node
        YAML::Node getConfig() const;

        // Print the loaded configuration to the provided output stream
        void printConfig(std::ostream &out) const;

    private:
        YAML::Node config_;
    };

} // namespace yaml_parser