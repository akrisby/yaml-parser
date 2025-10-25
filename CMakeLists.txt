cmake_minimum_required(VERSION 3.15)
project(yaml_parser VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable FetchContent for downloading dependencies
include(FetchContent)

# Fetch yaml-cpp
FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG yaml-cpp-0.7.0
)
FetchContent_MakeAvailable(yaml-cpp)

# Find nlohmann_json
find_package(nlohmann_json 3.11.2 REQUIRED)

# Add include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add source files
add_library(${PROJECT_NAME}
    src/yaml_parser.cpp
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    yaml-cpp
    nlohmann_json::nlohmann_json
)

# Create example executable
add_executable(${PROJECT_NAME}_example
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME}_example
    PRIVATE
    ${PROJECT_NAME}
)