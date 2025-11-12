#pragma once

#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace yaml_parser
{

    class YamlSchemaValidator
    {
    public:
        YamlSchemaValidator();

        // Load JSON schema from file
        bool loadSchema(const std::string &schemaFile);

        // Validate the provided YAML node against the loaded schema
        bool validate(const YAML::Node &node);

        // Get any validation errors
        std::vector<std::string> getErrors() const;

    private:
        nlohmann::json schema_;
        std::vector<std::string> errors_;

        // Helper functions for validation
        bool validateNode(const YAML::Node &node, const nlohmann::json &schema);
        void addError(const std::string &error);
    };

} // namespace yaml_parser
