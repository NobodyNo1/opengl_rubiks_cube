#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "math.h"
#include "helper.h"

#define MOVE_ON_MOVE_OF_CURSOR 1
#define MOVE_ON_DRAG_OF_CURSOR 2

struct MouseState {
    float posX;
    float posY;
    int is_rmb_hold;
    int is_lmb_hold;
};

struct CameraPositionState {
    float x;
    float y;
    float angle;
    int type;
};

struct DragAction {
    float startX;
    float startY;
    float x;
    float y;
    int startFound;
    int isActive;
};

static int object_move = MOVE_ON_DRAG_OF_CURSOR;

CameraPositionState cameraPositionState = { 0.0f, 0.0f, 0.0f, object_move};
DragAction dragAction = { 0.0f, 0.0f, 0.0f, 0.0f, 0, 0 };
// rotation can can be performed (but not necessarily applied)
MouseState mouseState;
MouseState previousMouseState;


float normalize_mouse_position(float number, int maxVal);
float flip_and_normalize_mouse_position(float number, int maxVal);
void update_mouse_state(float mouseX, float mouseY);
void handleCameraMovement(float window_width, float window_height);
void handleDragAction();

// Process mouse key events
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // note that click is single event
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        mouseState.is_rmb_hold = action == GLFW_PRESS;
        printf("Click Changed, isPress: %i!\n",  action == GLFW_PRESS);
    } else if(button == GLFW_MOUSE_BUTTON_LEFT){
        mouseState.is_lmb_hold = action == GLFW_PRESS;
        printf("Click Changed, isPress: %i!\n",  action == GLFW_PRESS);
    }
}

// Process keyboard events
void processKeyboardInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Process mouse movements
void processMouseInput(GLFWwindow *window, float window_width, float window_height)
{
    // Retriving mouse position
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    
    // Check if mouse inside the screen
    if(mouseX < 0 || mouseY < 0 || mouseX > window_width || mouseY > window_height)
        return;

    update_mouse_state(mouseX, mouseY);
}

// Handling all events
// --------------------------------------------------
void update_mouse_state(float mouseX, float mouseY) {
    previousMouseState = mouseState;
    mouseState.posX = mouseX;
    mouseState.posY = mouseY;
}

void handleActions(float window_width, float window_height) {
    handleCameraMovement(window_width, window_height);
    handleDragAction();
}

// camera movement
void handleCameraMovement(float window_width, float window_height) {
    // here origin of vector is center of screen
    if(object_move == MOVE_ON_MOVE_OF_CURSOR){
        // swapped in order to make direction of cursor match rotation of the cube
        cameraPositionState.y = normalize_mouse_position(mouseState.posX, window_width);
        cameraPositionState.x = -flip_and_normalize_mouse_position( mouseState.posY, window_height);
        cameraPositionState.angle= mod(sqrt(
            pow(cameraPositionState.x,2) + pow(cameraPositionState.y, 2)
        )*360.0f, 360.0f);
        return;
    }
    
    // here origin of vector is start hold position
    if(object_move == MOVE_ON_DRAG_OF_CURSOR) {
        if(!mouseState.is_rmb_hold) {
            cameraPositionState.x = 0.0;
            cameraPositionState.y = 0.0;
            // get last position of model add to the current mouse position
            previousMouseState.posX = mouseState.posX;
            previousMouseState.posX = mouseState.posY;
            return;
        }
        float xoffset = previousMouseState.posX - mouseState.posX ;
        float yoffset = previousMouseState.posY - mouseState.posY; // Reversed since y-coordinates range from bottom to top
        float sensitivity = 0.3f; // Adjust this value to control mouse movement speed
        xoffset *= sensitivity;
        yoffset *= sensitivity;
        previousMouseState.posX = mouseState.posX ;
        previousMouseState.posX = mouseState.posY;

        cameraPositionState.y =  yoffset;
        cameraPositionState.x =  xoffset;
    }
}

void handleDragAction() {
    // If camera moves by movement of mouse there is no point of
    // drag movement
    if(object_move != MOVE_ON_DRAG_OF_CURSOR)
        return;

    if(!dragAction.isActive && mouseState.is_lmb_hold) {
        // writing for x, because start will be set by/after raycaster        
        dragAction.x = mouseState.posX;
        dragAction.y = mouseState.posY;
    } else if(dragAction.isActive && !mouseState.is_lmb_hold) {
        dragAction.startX = 0.0f;
        dragAction.startY = 0.0f;
        dragAction.x = 0.0f;
        dragAction.y = 0.0f;
        dragAction.startFound = 0;
    }
    else
    {
        dragAction.x = mouseState.posX;
        dragAction.y = mouseState.posY;
    }
    dragAction.isActive = mouseState.is_lmb_hold;
}

// Helper functions
// ---------------------------

float normalize_mouse_position(float number, int maxVal)  {
    return 2.0f*(number/maxVal) - 1.0f;
}

float flip_and_normalize_mouse_position(float number, int maxVal) {
    //mouseState.posX/WINDOW_WIDTH - 1;   0..1
    return 2.0f*((maxVal-number)/maxVal) - 1.0f;
}
