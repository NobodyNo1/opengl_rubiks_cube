#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
// Define the vertex positions for the box

// Define the vertex positions for the box
GLfloat vertices[] = {
    // Position             // Normals
    -0.5f, -0.5f, -0.5f,    0.0f, -1.0f, 0.0f,   // Bottom left back
     0.5f, -0.5f, -0.5f,    0.0f, -1.0f, 0.0f,   // Bottom right back
     0.5f, -0.5f,  0.5f,    0.0f, -1.0f, 0.0f,   // Bottom right front
    -0.5f, -0.5f,  0.5f,    0.0f, -1.0f, 0.0f,   // Bottom left front

    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,    // Top left back
     0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,    // Top right back
     0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    // Top right front
    -0.5f,  0.5f,   0.5f,   0.0f, 1.0f, 0.0f,    // Top left front

    -0.5f, -0.5f, -0.5f,    -1.0f, 0.0f, 0.0f,   // Bottom left back
    -0.5f,  0.5f, -0.5f,    -1.0f, 0.0f, 0.0f,   // Top left back
    -0.5f,  0.5f,  0.5f,    -1.0f, 0.0f, 0.0f,   // Top left front
    -0.5f, -0.5f,  0.5f,    -1.0f, 0.0f, 0.0f,   // Bottom left front

     0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    // Bottom right back
     0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    // Top right back
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    // Top right front
     0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    // Bottom right front

    -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f,   // Bottom left back
     0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f,   // Bottom right back
     0.5f,  0.5f, -0.5f,    0.0f, 0.0f, -1.0f,   // Top right back
    -0.5f,  0.5f, -0.5f,    0.0f, 0.0f, -1.0f,   // Top left back

    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    // Bottom left front
     0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    // Bottom right front
     0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    // Top right front
    -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    // Top left front
};

GLuint indices[] = {
    // Bottom face
    0, 1, 2,
    2, 3, 0,

    // Top face
    4, 5, 6,
    6, 7, 4,

    // Left face
    8, 9, 10,
    10, 11, 8,

    // Right face
    12, 13, 14,
    14, 15, 12,

    // Back face
    16, 17, 18,
    18, 19, 16,

    // Front face
    20, 21, 22,
    22, 23, 20
};