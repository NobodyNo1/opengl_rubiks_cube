#include "src/app.h"

// TODO: figure out better way of sending resourses (think how it will work on final build)
int main(int argc, char* argv[]) {
    if (argc > 1) {
        char* path = argv[1];
        std::cout << "App root: " << path << std::endl;
        root_path = path;
        // Use the path argument in your program
    } else {
        std::cout << "Unable to find app root" << std::endl;
    }

    return start();
}