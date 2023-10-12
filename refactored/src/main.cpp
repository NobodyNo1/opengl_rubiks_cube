// #include "tools/memtrack.hpp"
#include "app.hpp"

// TODO: figure out better way of sending resourses (think how it will work on final build)
// build script should copy resources to the build folder, after which relative path
// will work in code and for builds

int main(int argc, char* argv[]) {
    // if (argc > 1) {
    //     char* path = argv[1];
    //     std::cout << "App root: " << path << std::endl;
    //     root_path = path;
    //     // Use the path argument in your program
    // } else {
    //     std::cout << "Unable to find app root" << std::endl;
    // }

    return start();
}