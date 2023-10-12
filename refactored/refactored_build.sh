#!/bin/bash
g++ -g -Iexternal/include -Isrc -Lexternal/lib/mac/ -lglfw3 external/glad.c src/main.cpp -framework IOKit -framework Cocoa -framework OpenGL -o build/mac_rubiks_cube --std=c++11

# TODO:
# 1. Execute build from the build folder
# 2. copy resource files to the build folder

# Check the compilation result
if [ $? -eq 0 ]; then
  # Compilation successful, execute the program
  echo "Compilation successful. Running the program..."
  ./build/mac_rubiks_cube
else
  # Compilation failed, display an error message
  echo "Compilation failed."
fi