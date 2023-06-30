g++ -Iinclude -Llib/mac/ -lglfw3 src/glad.c rubiks_cube.cpp -framework IOKit -framework Cocoa -framework OpenGL -o build/rubiks_cube --std=c++11

./build/test