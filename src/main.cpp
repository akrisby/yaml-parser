#include "yaml_parser.hpp"
#include "yaml_schema_validator.hpp"
#include <iostream>
#include <string>

static void print_usage(const char *prog)
{
    std::cout << "Usage: " << prog << " [options] [schema_file config_file]\n"
              << "Options:\n"
              << "  -s, --schema <file>   Path to schema file (default: schema.json)\n"
              << "  -c, --config <file>   Path to config file (default: config.yaml)\n"
              << "  -h, --help            Show this help message\n";
}

int main(int argc, char *argv[])
{
    std::string schema_file = "test.schema.json";
    std::string config_file = "test.yaml";
    bool do_print = false;
    std::string prog = argc > 0 ? argv[0] : "yaml-validator";

    // Simple command-line parsing: options and up to two positional args.
    int positional_count = 0;
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help")
        {
            print_usage(prog.c_str());
            return 0;
        }
        else if (arg == "-s" || arg == "--schema")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "Error: expected filename after " << arg << std::endl;
                print_usage(prog.c_str());
                return 1;
            }
            schema_file = argv[++i];
        }
        else if (arg == "-c" || arg == "--config")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "Error: expected filename after " << arg << std::endl;
                print_usage(prog.c_str());
                return 1;
            }
            config_file = argv[++i];
        }
        else if (!arg.empty() && arg[0] == '-')
        {
            if (arg == "-p" || arg == "--print")
            {
                do_print = true;
                continue;
            }
            std::cerr << "Unknown option: " << arg << std::endl;
            print_usage(prog.c_str());
            return 1;
        }
        else
        {
            // Positional args: first -> schema, second -> config
            if (positional_count == 0)
                schema_file = arg;
            else if (positional_count == 1)
                config_file = arg;
            else
            {
                std::cerr << "Too many positional arguments\n";
                print_usage(prog.c_str());
                return 1;
            }
            ++positional_count;
        }
    }

    // Separate parser and validator
    yaml_parser::YamlParser parser;
    yaml_parser::YamlSchemaValidator validator;

    // Load schema first (so we can report schema errors early)
    if (!validator.loadSchema(schema_file))
    {
        std::cerr << "Failed to load schema: " << schema_file << std::endl;
        for (const auto &e : validator.getErrors())
            std::cerr << "- " << e << std::endl;
        return 1;
    }

    // Load config (with includes)
    if (!parser.loadConfig(config_file))
    {
        std::cerr << "Failed to load config: " << config_file << std::endl;
        return 1;
    }

    // Validate parsed config
    if (!validator.validate(parser.getConfig()))
    {
        std::cerr << "Validation failed:" << std::endl;
        for (const auto &error : validator.getErrors())
        {
            std::cerr << "- " << error << std::endl;
        }
        return 1;
    }

    std::cout << "Configuration is valid!" << std::endl;

    if (do_print)
    {
        parser.printConfig(std::cout);
    }
    return 0;
}