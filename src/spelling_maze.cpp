#include "../include/map.hpp"
#include <pybind11/pybind11.h>


void generate_maze(std::string word, int grid_width, int grid_height, int block_width = 20, int block_height = 20){
    generator.seed(time(0));
    WordMaze m(word, grid_width, grid_height, block_width, block_height);
    m.save_to_png(word + ".png");
}

PYBIND11_MODULE(SpellingMaze, m) {
    m.def("generate_maze", &generate_maze, "A function to generate a maze.");
}


