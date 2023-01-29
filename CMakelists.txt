cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(SpellingMaze VERSION 0.1)

find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
find_package(pybind11 REQUIRED)

pybind11_add_module(SpellingMaze src/spelling_maze.cpp)

target_link_libraries(SpellingMaze PRIVATE sfml-graphics)