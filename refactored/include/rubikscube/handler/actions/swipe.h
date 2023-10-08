#include <handler/mouse.h>

class SwipeAction {

private:
    float startX    = 0.0f;
    float startY    = 0.0f;
    float x         = 0.0f;
    float y         = 0.0f;
    int startFound  = 0;
    bool isActive   = false;

public:
    SwipeAction(){}

    void handle(MouseState mouseState) {
        if(!isActive && mouseState.is_lmb_hold) {
            // writing for x, because start will be set by/after raycaster        
            x = mouseState.posX;
            y = mouseState.posY;
        } else if(isActive && !mouseState.is_lmb_hold) {
            startX = 0.0f;
            startY = 0.0f;
            x = 0.0f;
            y = 0.0f;
            startFound = 0;
        }
        else
        {
            x = mouseState.posX;
            y = mouseState.posY;
        }
        isActive = mouseState.is_lmb_hold;
    }
};