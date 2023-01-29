## Spelling Maze C++ Python Module 
This repository holds a C++ program capable of generating spelling mazes to help children learn how to spell. This repository is paired with an article available [here](https://wfale.net).

## Configuration
At this point this repository is set up for benchmarking but will be updated in the near future for command line usage and Python script importing. System and environment configuration is outlined [here](https://wfale.net/2023/01/02/sfml-c-and-windows-quick-guide-to-awesome-graphics/).

## Pre-Requisits
- SFML Compiled and Installed
- Pybind11 Installed
- Python Developer Tools Installed

## Compilation
    mkdir build
    cd build
    cmake -G "MinGW Makefiles" ..
    make (or 'mingw32-make' on Windows with MinGW)

## Import
The built file will be a .so(Linux) or a .pyd(Windows) in the build directory, as long as this file is in your PATH variable importing the library should be as simple as:
    
    import SpellingMaze

## Generating a Maze
Generating a maze can be done with the following call:

    SpellingMaze.generate_maze(<word>, <grid_width>, <grid_height>, <block_width>, <block_height>)

The maze will be saved to the directory you executed from witht he filename '\<word\>.png'