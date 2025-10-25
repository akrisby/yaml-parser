#pragma once

#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <vector>
#include <ostream>

namespace yaml_parser
{

    class YamlValidator
    {
    public:
        YamlValidator();

        // Load JSON schema from file
        bool loadSchema(const std::string &schemaFile);

        // Load YAML configuration from file
        bool loadConfig(const std::string &configFile);

        // Validate loaded YAML against loaded schema
        bool validate();

        // Get any validation errors
        std::vector<std::string> getErrors() const;

        // Print the loaded configuration to the provided output stream
        void printConfig(std::ostream &out) const;

    private:
        YAML::Node config_;
        nlohmann::json schema_;
        std::vector<std::string> errors_;

        // Helper functions for validation
        bool validateNode(const YAML::Node &node, const nlohmann::json &schema);
        void addError(const std::string &error);
    };

} // namespace yaml_parser