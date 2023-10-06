#include "../mouse.h"

#ifndef DRAG_H
#define DRAG_H
#define SENSITIVITY   0.3f

typedef struct DragState {
    float x;
    float y;
} DragState;

class DragAction{
public:
    DragState dragState = {0.0f, 0.0f};
    DragAction() {}    

    // camera movement
    void handle(
        float window_width,
        float window_height,
        MouseState previousMouseState,
        MouseState mouseState
    ) {
            if(!mouseState.is_rmb_hold) {
                dragState.x = 0.0;
                dragState.y = 0.0;
                // get last position of model add to the current mouse position
                previousMouseState.posX = mouseState.posX;
                previousMouseState.posX = mouseState.posY;
                return;
            }
            float xoffset = previousMouseState.posX - mouseState.posX ;
            float yoffset = previousMouseState.posY - mouseState.posY; // Reversed since y-coordinates range from bottom to top
            float sensitivity = SENSITIVITY; // Adjust this value to control mouse movement speed
            xoffset *= sensitivity;
            yoffset *= sensitivity;
            previousMouseState.posX = mouseState.posX ;
            previousMouseState.posX = mouseState.posY;

            dragState.y =  yoffset;
            dragState.x =  xoffset;
    }
};


#endif 