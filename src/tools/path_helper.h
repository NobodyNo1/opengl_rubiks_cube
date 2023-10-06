#include <iostream>

#ifndef PATH_HELPER_H
#define PATH_HELPER_H

static char* root_path;

// for now path shouldn't start with slash "/ or \"
char* get_path(const char* local_path) {
    size_t root_path_size = strlen(root_path);
    size_t local_path_size = strlen(local_path);

    char* real_path = (char*) malloc(root_path_size + local_path_size + 2);  // +2 for the extra '/' and null-terminator

    strcpy(real_path, root_path);
    strcat(real_path, "/");
    strcat(real_path, local_path);

    return real_path;
}
#endif