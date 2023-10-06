#!/bin/bash
g++ -Iinclude -Llib/mac/ -lglfw3 src/refactored/glad.c src/refactored/rubiks_cube.cpp -framework IOKit -framework Cocoa -framework OpenGL -o src/refactored/build/mac_rubiks_cube --std=c++11

# Get the directory path of the script

# Execute the program with the directory path as an argument
./src/refactored/build/mac_rubiks_cube