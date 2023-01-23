## Genetic Algorithm Neural Network 
This repository holds a C++ program capable of generating spelling mazes to help children learn how to spell. This repository is paired with an article available [here](https://wfale.net).

## Configuration
At this point this repository is set up for benchmarking but will be updated in the near future for command line usage and Python script importing. System and environment configuration is outlined [here](https://wfale.net/2023/01/02/sfml-c-and-windows-quick-guide-to-awesome-graphics/).

## Compilation
    mkdir build
    cd build
    cmake -G "MinGW Makefiles" ..
    make (or 'mingw32-make' on Windows with MinGW)

## Run
From the build directory:

    ./SpellingMaze.exe