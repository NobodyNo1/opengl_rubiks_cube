#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "mouse.h"
#include "keyboard.h"
#include "math.h"
#include "actions/drag.h"
#include "actions/swipe.h"


class ActionHandler {


public:
    MouseHandler *mouseHandler;
    KeyboardHandler *keyboardHandler;
    DragAction *dragAction;
    SwipeAction *swipeAction;

    ActionHandler(GLFWwindow *window){
        mouseHandler = new MouseHandler();
        keyboardHandler = new KeyboardHandler();
        dragAction = new DragAction();
        swipeAction = new SwipeAction();
    }

    void handleActions(GLFWwindow *window, float window_width, float window_height) {
        mouseHandler->handle(window, window_width, window_height);
        keyboardHandler->handle(window);
        dragAction->handle(window_width, window_height, mouseHandler->previousMouseState, mouseHandler->mouseState);
        swipeAction->handle(mouseHandler->mouseState);
    }
};