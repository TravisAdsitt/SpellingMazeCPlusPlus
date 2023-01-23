#include <math.h>
#include <iostream>
#include <random>
#include <ctime>
#include <vector>
#include <SFML/Graphics.hpp>
#include <chrono>
#include "../include/map.hpp"


int main()
{
    std::string words[5] = {"Hello", "Random", "Testing", "Maze", "Generation"};
    int grid_widths[4] = {20, 40, 80, 160};
    generator.seed(time(0));
    
    std::cout << "Word,Width,Height,Generation Time,Image Save Time," << std::endl;

    for(std::string word: words){
        for(int grid_width: grid_widths){
                std::cout << word << "," << grid_width << "," << grid_width << ",";
                
                
                auto start = std::chrono::high_resolution_clock::now();
                WordMaze m(word, grid_width, grid_width, 20, 20);
                auto stop = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
                std::cout << duration.count()  / 1000.f << ",";


                start = std::chrono::high_resolution_clock::now();
                m.save_to_png("Test.png");
                stop = std::chrono::high_resolution_clock::now();
                duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
                std::cout << duration.count()  / 1000.f << "," << std::endl;
        }
    }

    return 0;
}


