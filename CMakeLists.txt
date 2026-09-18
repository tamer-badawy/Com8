cmake_minimum_required(VERSION 3.16)
project(Com8 VERSION 1.0.0 LANGUAGES CXX)

# Force modern C++ standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files definitions
set(SOURCE_FILES 
    ./src/main.cpp
    ./src/chip8.cpp 
    ./src/chip8.hpp
)

# Try to find standard system package mappings for Raylib
find_package(raylib QUIET)

if(NOT raylib_FOUND)
    message(STATUS "Raylib package not automatically detected, attempting native module linkage...")
endif()

# Define project target executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Link libraries based on standard compiler environments
if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib pthread GL m X11)
else()
    # Windows native architectures (e.g., linked via MSVC/vcpkg package configurations)
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
endif()
