# yaml-parser

A small C++ library and example CLI that parses YAML configuration files and validates them against a JSON Schema.

Key points
- Language: C++17
- Build: CMake (3.15+)
- Dependencies: yaml-cpp, nlohmann/json

Repository layout
- `include/` — public headers
- `src/` — library and example
- `test/` — example `config.yaml`/`schema.json`
- `CMakeLists.txt` — project build

Prerequisites
- CMake 3.15 or later
- A C++17-capable compiler (GCC, Clang, MSVC)
- If you don't have system packages for dependencies, CMake will download `yaml-cpp` using FetchContent. `nlohmann/json` is required as a packaged CMake config on the system; on Debian/Ubuntu install `nlohmann-json3-dev` or use a package manager of your choice.

Build

```bash
# create a build directory and configure
mkdir -p build && cd build
cmake ..

# build the library and example
cmake --build .

# example binary will be `yaml_parser_example`
./yaml_parser_example
```

CLI usage

The example program accepts optional flags or positional arguments:

- `-s, --schema <file>` — path to schema file (default: `schema.json`)
- `-c, --config <file>` — path to config file (default: `config.yaml`)
- `-h, --help` — show usage

Examples

Run with the example files in the repository root:

```bash
./yaml_parser_example
```

Run with explicit files:

```bash
# named flags
./yaml_parser_example -s test/schema.json -c test/config.yaml

# positional args (schema then config)
./yaml_parser_example test/schema.json test/config.yaml
```

Library usage (minimal)

Include the header and call the load/validate functions:

```cpp
#include "yaml_parser.hpp"

yaml_parser::YamlValidator v;
v.loadSchema("schema.json");
v.loadConfig("config.yaml");
if (!v.validate()) {
  for (auto &e : v.getErrors()) std::cerr << e << std::endl;
}
```

Notes and next steps
- The JSON Schema validation implemented here is intentionally minimal (basic type checks, required fields, and nested property validation). For full JSON Schema support consider integrating a proven JSON Schema validator library and converting YAML -> JSON before validation.
- Tests: add unit tests to verify edge cases (missing files, type mismatches, arrays, nested objects).

Contributing

PRs welcome. Please describe tests and add small, focused changes.

License

MIT
