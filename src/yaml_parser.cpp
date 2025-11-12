#include "yaml_parser.hpp"
#include "yaml_include_resolver.hpp"

namespace yaml_parser
{

    // YamlParser implementation
    YamlParser::YamlParser() {}

    bool YamlParser::loadConfig(const std::string &configFile)
    {
        try
        {
            config_ = yaml_utils::loadWithIncludes(configFile);
            return true;
        }
        catch (const std::exception &)
        {
            return false;
        }
    }

    YAML::Node YamlParser::getConfig() const
    {
        return config_;
    }

    void YamlParser::printConfig(std::ostream &out) const
    {
        if (!config_.IsDefined())
        {
            out << "<no config loaded>\n";
            return;
        }
        YAML::Emitter emitter;
        emitter << config_;
        out << emitter.c_str() << std::endl;
    }

} // namespace yaml_parser