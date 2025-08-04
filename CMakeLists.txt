cmake_minimum_required(VERSION 3.10)
project(samsung_hw)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include headers
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/writing_pattern_generator.cpp
    src/configurator.cpp
    src/frame_transmitter.cpp
    src/writing_pattern_detector.cpp
    test/test_system.cpp
)

# Find YAML-CPP
find_package(yaml-cpp REQUIRED)

# Executable
add_executable(test_system.out ${SOURCES})

# Link libraries
target_link_libraries(test_system.out yaml-cpp)
