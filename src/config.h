#define DEFAULT_WINDOW_WIDTH    1280
#define DEFAULT_WINDOW_HEIGHT   720

float WINDOW_WIDTH  =  DEFAULT_WINDOW_WIDTH;
float WINDOW_HEIGHT =  DEFAULT_WINDOW_HEIGHT;


#define MOVE_ON_MOVE_OF_CURSOR 1
#define MOVE_ON_DRAG_OF_CURSOR 2

struct CameraRotation {
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
    int isActive;
};