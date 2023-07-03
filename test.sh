g++ -Iinclude -Llib/mac/ -lglfw3 src/glad.c test.cpp -framework IOKit -framework Cocoa -framework OpenGL -o build/test --std=c++11

./build/test