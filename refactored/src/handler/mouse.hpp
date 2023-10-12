#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifndef MOUSE_H
#define MOUSE_H
typedef struct MouseState {
    float posX;
    float posY;
    int is_rmb_hold;
    int is_lmb_hold;
} MouseState;

class MouseHandler {


private:

    // Handling all events
    // --------------------------------------------------
    void update_mouse_state(float mouseX, float mouseY) {
        previousMouseState = mouseState;
        mouseState.posX = mouseX;
        mouseState.posY = mouseY;
    }
    
public:
    MouseState mouseState;
    MouseState previousMouseState;
    MouseHandler() {}

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        // note that click is single event
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            log("RMB");
            mouseState.is_rmb_hold = action == GLFW_PRESS;
        } else if(button == GLFW_MOUSE_BUTTON_LEFT){
            log("LMB");
            mouseState.is_lmb_hold = action == GLFW_PRESS;
        }
    }
    // Process mouse movements
    void handle(GLFWwindow *window, float window_width, float window_height)
    {
        // Retriving mouse position
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        
        // Check if mouse inside the screen
        if(mouseX < 0 || mouseY < 0 || mouseX > window_width || mouseY > window_height)
            return;

        update_mouse_state(mouseX, mouseY);
    }


};

#endif