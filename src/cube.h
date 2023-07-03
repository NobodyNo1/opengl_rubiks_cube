#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION

#define CUBE_SIZE     3

// Indices of cube sides
#define BOTTOM_IDX   0
#define TOP_IDX      1
#define LEFT_IDX     2
#define RIGHT_IDX    3
#define BACK_IDX     4
#define FRONT_IDX    5


// no need to hold initial position
// if and only if draw loop matches with select and init loops
struct Side
{
    int sideIdx;
    glm::vec3 color;   // color does not change because of that we can keep its
};

struct Cube
{
    int cubeId;
    Side side[6];
};

// Description of how cube side ? should be rotated
struct RotationConfig
{
    int active;
    float angle;   // -90.0f .. 90.0f
    int axisIndex;
    int axisValue;
    float startRotationTime;
    int isClockWise;
};


// Global variables
// ----------
struct Cube cubes[27];
glm::vec3 cubeIdToPosition[27];
RotationConfig rotationConfig = {0, 0.0f, 0, 0, 0.0f, 0};

float sideColors[] = {
    1.0f, 1.0f, 0.0f,   // Bottom face (yellow)
    1.0f, 1.0f, 1.0f,   // Top face (white)
    0.0f, 1.0f, 0.0f,   // Left face (green)
    0.0f, 0.0f, 1.0f,   // Right face (blue)
    1.0f, 0.5f, 0.0f,   // Back face (orange) ??
    1.0f, 0.0f, 0.0f    // Front face (red)
};


glm::vec3 blackColor      = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 whiteColor      = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 magnetaColor    = glm::vec3(1.0000f, 0.0000f, 1.0000f);

// Functions
// ----------

// Retriving color from side index (each side have distinct color)
glm::vec3 getCubeSideColor(int sideIdx, glm::vec3 cubePosition);

// For checking is cube side matches of rubik's cube ??
int isOuterSide(int sideIdx, glm::vec3 sideOrder);

// For accessing cubes
Cube getModel(int index) { return cubes[index]; }

// Implementation
// -----
void init3_3boxes()
{
    // TODO: needs to update after custom sized RB will be implemented
    // cubes = (struct Cube *) malloc(sizeof(struct Cube)*27);
    int cubeSize = CUBE_SIZE;
    int modelIdx = 0;
    for (int i = 0; i < cubeSize; i++)
    {
        for (int j = 0; j < cubeSize; j++)
        {
            for (int k = 0; k < cubeSize; k++)
            {
                // skipping center cube
                if (k == 1 && k == j && j == i)
                    continue;
                modelIdx = 3 * (3 * i + j) + k;

                glm::vec3 cubePosition = glm::vec3(k - 1, j - 1, i - 1);
                cubes[modelIdx].cubeId = modelIdx;
                for (int sideIndex = 0; sideIndex < 6; sideIndex++)
                {
                    cubes[modelIdx].side[sideIndex].sideIdx = sideIndex;
                    cubes[modelIdx].side[sideIndex].color =
                        getCubeSideColor(sideIndex, cubePosition);
                }
                // when initing it is same as thing below
                // cubeIdToPosition[cubes[modelIdx].cubeId] = cubePosition;
                cubeIdToPosition[modelIdx] = cubePosition;
            }
        }
    }
}

int isOuterSide(int sideIdx, glm::vec3 cubePosition)
{
    int x, y, z;
    x = cubePosition.x;
    y = cubePosition.y;
    z = cubePosition.z;


    /*
    - center sides have only one side colored
        - top and bottom item z==1 or z==-1; x==y
            - top top
            - bot bot
        - sides z=0; (y == 1 or y == -1) and (x==-1 or x==1)
            - need to find both facing sizes

     corners have 3 sides colored
     others have only 2 sides


    ordering of i's
        Bottom face
        Top face
        Left face
        Right face
        Back face
        Front face
    */
    // center pices
    int frontCheck  =    sideIdx  == FRONT_IDX   &&   z ==  1;
    int backCheck   =    sideIdx  == BACK_IDX    &&   z == -1;
    int topCheck    =    sideIdx  == TOP_IDX     &&   y ==  1;
    int bottomCheck =    sideIdx  == BOTTOM_IDX  &&   y == -1;
    int rightCheck  =    sideIdx  == RIGHT_IDX   &&   x ==  1;
    int leftCheck   =    sideIdx  == LEFT_IDX    &&   x == -1;

    return  backCheck || bottomCheck || leftCheck || frontCheck || topCheck || rightCheck;
}

glm::vec3 getCubeSideColor(int sideIdx, glm::vec3 cubePosition)
{
    if (isOuterSide(sideIdx, cubePosition))
    {
        return glm::vec3(sideColors[sideIdx * 3], sideColors[sideIdx * 3 + 1],
                         sideColors[sideIdx * 3 + 2]);
    }

    return blackColor;
}


// p is outer iterator
int getModelIdxByFixedAxis(int axisValue, int fixedAxis, int p, int q)
{
    int modelIdx = 0;
    switch (fixedAxis)
    {
        case 0:
        {
            // x = axisValue
            // y = q
            // z = p
            modelIdx = 3 * (3 * p + q) + axisValue;
            break;
        }
        case 1:
        {
            // x = q
            // y = axisValue
            // z = p
            modelIdx = 3 * (3 * p + axisValue) + q;
            break;
        }
        case 2:
        {
            // x = q
            // y = p
            // z = axisValue
            modelIdx = 3 * (3 * axisValue + p) + q;
            break;
        }
    }
    return modelIdx;
}

void rotateSideIdx(int axis, int isClockwise, Side* side)
{
    /*
    clockWise
        by x -> (top -> front -> bot -> back)
        1 => 5
        5 => 0
        0 => 4
        4 => 1
    */
    int rotateByX[6];
    if(isClockwise){
        rotateByX[5] = 1;
        rotateByX[0] = 5;
        rotateByX[4] = 0;
        rotateByX[1] = 4;
    } else{
        rotateByX[1] = 5;
        rotateByX[5] = 0;
        rotateByX[0] = 4;
        rotateByX[4] = 1;
    }
    
    rotateByX[2] = 2;
    rotateByX[3] = 3;
    /*
    clockWise
        by y -> (left -> front -> right-> back)
        2 => 5
        5 => 3
        3 => 4
        4 => 2
    */
    int rotateByY[6];
    if(isClockwise){
        rotateByY[5] =2;
        rotateByY[3] =5;
        rotateByY[4] =3;
        rotateByY[2] =4;
    } else{
        rotateByY[2] = 5;
        rotateByY[5] = 3;
        rotateByY[3] = 4;
        rotateByY[4] = 2;
    }
    rotateByY[1] = 1;
    rotateByY[0] = 0;
    /*
    clockWise
        by z -> (left-> top-> right -> bot)
        2 => 1
        1 => 3
        3 => 0
        0 => 3
    */
    int rotateByZ[6];
    if(isClockwise){
        rotateByZ[2] = 1;
        rotateByZ[1] = 3;
        rotateByZ[3] = 0;
        rotateByZ[0] = 2;
    } else {
        rotateByZ[1] = 2;
        rotateByZ[3] = 1;
        rotateByZ[0] = 3;
        rotateByZ[2] = 0;
    }
    rotateByZ[5] = 5;
    rotateByZ[4] = 4;

    int* currentRot;
    switch (axis)
    {
        case 0:
            currentRot = rotateByX;
            break;
        case 1:
            currentRot = rotateByY;
            break;
        case 2:
            currentRot = rotateByZ;
            break;
    };
    Side updateTo[6];
    for (int sideIndex = 0; sideIndex < 6; sideIndex++)
    {   // need to rotate sides
        Side curSide = side[currentRot[sideIndex]];
        updateTo[sideIndex].color = curSide.color;
        updateTo[sideIndex].sideIdx = curSide.sideIdx;
    }

    for (int sideIndex = 0; sideIndex < 6; sideIndex++)
    {   // need to rotate sides
        side[sideIndex] = updateTo[sideIndex];
    }
}

void updateIndicies()
{
    int modelIdx = 0;
    
    // collection all the indices that need to be swapped
    Cube cubesToUpdate[9];

    for (int p = 0; p < CUBE_SIZE; p++)
    {
        for (int q = 0; q < CUBE_SIZE; q++)
        {
            // getting cubeId (position) that needs to be rotated
            // TODO: EXPAIN THIS MORE AND MAKE CODE MORE CLEAR
            // + 1 and cube position and sideOrder is so confusing
            modelIdx = getModelIdxByFixedAxis(rotationConfig.axisValue + 1,
                                              rotationConfig.axisIndex, p, q);
            // getting real cubeId
            Cube cube = getModel(modelIdx);
            // getting position of the real cube
            glm::vec3 pos = cubeIdToPosition[cube.cubeId];
           // printf("%f, %f, %f \n", pos.x, pos.y, pos.z);

            // collecting
            cubesToUpdate[3 * p + q].cubeId = cube.cubeId;

            for (int sideIndex = 0; sideIndex < 6; sideIndex++)
            {   // need to rotate sides
                cubesToUpdate[3 * p + q].side[sideIndex].color =
                    cube.side[sideIndex].color;
                cubesToUpdate[3 * p + q].side[sideIndex].sideIdx =
                    cube.side[sideIndex].sideIdx;
            }
            rotateSideIdx(rotationConfig.axisIndex, rotationConfig.isClockWise,
                          cubesToUpdate[3 * p + q].side);
        }
    }
    // Rotating the matrix, in other words setting current values to values they
    // need to be
    // for y rotation it should be reversed
    if(rotationConfig.axisIndex == 1)
        bulkMatrixRotation(!rotationConfig.isClockWise, cubesToUpdate);
     else
        bulkMatrixRotation(rotationConfig.isClockWise, cubesToUpdate);
    for (int p = 0; p < CUBE_SIZE; p++)
    {
        for (int q = 0; q < CUBE_SIZE; q++)
        {
            // skipping center cube
            if (q == 1 && q == p)
                continue;

            // same logic as collection
            int from = getModelIdxByFixedAxis(rotationConfig.axisValue + 1,
                                              rotationConfig.axisIndex, p, q);
            Cube& toUpdate = cubes[from];

            Cube valueOfUpdate = cubesToUpdate[3 * p + q];

            toUpdate.cubeId = valueOfUpdate.cubeId;
            for (int sideIndex = 0; sideIndex < 6; sideIndex++)
            {
                toUpdate.side[sideIndex].color =
                    valueOfUpdate.side[sideIndex].color;
                // side indexes are the same for now
                toUpdate.side[sideIndex].sideIdx =
                    valueOfUpdate.side[sideIndex].sideIdx;
            }
        }
    }
}
