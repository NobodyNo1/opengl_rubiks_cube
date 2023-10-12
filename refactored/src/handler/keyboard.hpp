#include <glad/glad.h>
#include <GLFW/glfw3.h>


class KeyboardHandler{

public:
    KeyboardHandler(){}
    // Process keyboard events
    void handle(GLFWwindow *window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }
};