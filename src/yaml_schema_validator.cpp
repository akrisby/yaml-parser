#include "yaml_schema_validator.hpp"
#include <fstream>

// include nlohmann already in header but include fstream here for file reading

namespace yaml_parser
{

    YamlSchemaValidator::YamlSchemaValidator() {}

    bool YamlSchemaValidator::loadSchema(const std::string &schemaFile)
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
            addError(std::string("Error parsing schema file: ") + e.what());
            return false;
        }
    }

    bool YamlSchemaValidator::validate(const YAML::Node &node)
    {
        if (schema_.is_null())
        {
            addError("No schema loaded");
            return false;
        }
        if (!node.IsDefined())
        {
            addError("No configuration provided");
            return false;
        }

        errors_.clear();
        return validateNode(node, schema_);
    }

    std::vector<std::string> YamlSchemaValidator::getErrors() const
    {
        return errors_;
    }

    bool YamlSchemaValidator::validateNode(const YAML::Node &node, const nlohmann::json &schema)
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

    void YamlSchemaValidator::addError(const std::string &error)
    {
        errors_.push_back(error);
    }

} // namespace yaml_parser
