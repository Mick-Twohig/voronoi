cmake_minimum_required(VERSION 3.27)
project(voronoi C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(CMAKE_BUILD_TYPE Debug)

include_directories("/opt/homebrew/Cellar/raylib/5.0/include")
add_compile_options(-Wall -Wextra -Wpedantic -Werror)
add_executable(voronoi voronoi.c)

add_library(raylib SHARED IMPORTED)
set_target_properties(raylib PROPERTIES IMPORTED_LOCATION "/opt/homebrew/Cellar/raylib/5.0/lib/libraylib.4.5.0.dylib")
target_link_libraries(voronoi raylib)

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
