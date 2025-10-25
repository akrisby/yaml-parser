#include "yaml_parser.hpp"

namespace yaml_parser
{

    YamlValidator::YamlValidator() {}

    bool YamlValidator::loadSchema(const std::string &schemaFile)
    {
        try
        {
            std::ifstream file(schemaFile);
            if (!file.is_open())
            {
                addError("Cannot open schema file: " + schemaFile);
                return false;
            }
            schema_ = nlohmann::json::parse(file);
            return true;
        }
        catch (const std::exception &e)
        {
            addError("Error parsing schema file: " + std::string(e.what()));
            return false;
        }
    }

    bool YamlValidator::loadConfig(const std::string &configFile)
    {
        try
        {
            config_ = YAML::LoadFile(configFile);
            return true;
        }
        catch (const YAML::Exception &e)
        {
            addError("Error parsing YAML file: " + std::string(e.what()));
            return false;
        }
    }

    bool YamlValidator::validate()
    {
        if (schema_.is_null())
        {
            addError("No schema loaded");
            return false;
        }
        if (!config_.IsDefined())
        {
            addError("No configuration loaded");
            return false;
        }

        errors_.clear();
        return validateNode(config_, schema_);
    }

    std::vector<std::string> YamlValidator::getErrors() const
    {
        return errors_;
    }

    bool YamlValidator::validateNode(const YAML::Node &node, const nlohmann::json &schema)
    {
        // Basic type validation
        if (schema.contains("type"))
        {
            std::string type = schema["type"];
            if (type == "object" && !node.IsMap())
            {
                addError("Expected object type");
                return false;
            }
            if (type == "array" && !node.IsSequence())
            {
                addError("Expected array type");
                return false;
            }
            if (type == "string" && !node.IsScalar())
            {
                addError("Expected string type");
                return false;
            }
            if (type == "number" && !node.IsScalar())
            {
                addError("Expected number type");
                return false;
            }
        }

        // Object property validation
        if (node.IsMap() && schema.contains("properties"))
        {
            for (const auto &property : schema["properties"].items())
            {
                const std::string &propertyName = property.key();
                if (node[propertyName])
                {
                    if (!validateNode(node[propertyName], property.value()))
                    {
                        return false;
                    }
                }
                else if (schema.contains("required") &&
                         std::find(schema["required"].begin(), schema["required"].end(), propertyName) != schema["required"].end())
                {
                    addError("Missing required property: " + propertyName);
                    return false;
                }
            }
        }

        return true;
    }

    void YamlValidator::addError(const std::string &error)
    {
        errors_.push_back(error);
    }

} // namespace yaml_parser