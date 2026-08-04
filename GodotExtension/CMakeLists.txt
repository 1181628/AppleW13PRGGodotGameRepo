cmake_minimum_required(VERSION 3.20)
project(my_extension)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add godot-cpp directory
add_subdirectory(external/godot-cpp)

# Include source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

# Create shared library (.dll)
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Link godot-cpp library
target_link_libraries(${PROJECT_NAME} PRIVATE godot::cpp)

# Output final DLL into game/bin/
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/game/bin"
)