#!/bin/bash
g++ -Iinclude -Llib/mac/ -lglfw3 src/glad.c rubiks_cube.cpp -framework IOKit -framework Cocoa -framework OpenGL -o build/mac_rubiks_cube --std=c++11

# Get the directory path of the script
script_dir="$(cd "$(dirname "$0")" && pwd)"

# Execute the program with the directory path as an argument
./build/mac_rubiks_cube "$script_dir"