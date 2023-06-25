#define WINDOW_WIDTH    1280
#define WINDOW_HEIGHT   720

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