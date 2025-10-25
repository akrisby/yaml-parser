# YAML Parser with JSON Schema Validation

This project provides a C++ library for parsing YAML configuration files and validating them against JSON schemas using yaml-cpp and nlohmann/json libraries.

## Requirements

- CMake 3.15 or higher
- C++17 compiler
- yaml-cpp library
- nlohmann/json library

## Building the Project

```bash
# Create a build directory
mkdir build && cd build

# Configure the project
cmake ..

# Build
cmake --build .
```

## Usage

Here's a basic example of how to use the validator:

```cpp
#include "yaml_parser.hpp"
#include <iostream>

int main() {
    yaml_parser::YamlValidator validator;
    
    if (!validator.loadSchema("schema.json")) {
        std::cerr << "Failed to load schema" << std::endl;
        return 1;
    }
    
    if (!validator.loadConfig("config.yaml")) {
        std::cerr << "Failed to load config" << std::endl;
        return 1;
    }
    
    if (!validator.validate()) {
        std::cerr << "Validation failed:" << std::endl;
        for (const auto& error : validator.getErrors()) {
            std::cerr << "- " << error << std::endl;
        }
        return 1;
    }
    
    std::cout << "Configuration is valid!" << std::endl;
    return 0;
}
```

## Features

- Parse YAML configuration files
- Validate against JSON Schema
- Type validation
- Required property validation
- Detailed error reporting

## License

MIT License