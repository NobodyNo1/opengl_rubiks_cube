#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
// Define the vertex positions for the box

// Define the vertex positions for the box
GLfloat vertices[] = {
        // Position             // Color
        -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,   // Bottom left back
         0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,   // Bottom right back
         0.5f,  0.5f, -0.5f,    0.0f, 0.0f, 1.0f,   // Top right back
        -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0.0f,   // Top left back

        -0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,   // Bottom left front
         0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,   // Bottom right front
         0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,   // Top right front
        -0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 0.0f    // Top left front
    
};

GLuint indices[] = {
        // Back face
        0, 1, 2,
        2, 3, 0,

        // Front face
        4, 5, 6,
        6, 7, 4,

        // Left face
        7, 3, 0,
        0, 4, 7,

        // Right face
        1, 5, 6,
        6, 2, 1,

        // Top face
        7, 6, 2,
        2, 3, 7,

        // Bottom face
        0, 1, 5,
        5, 4, 0
};