#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stdio.h>

#include "config.h"
#include "vertices.h"

#define PADDING 0.01f
#define SPREAD 1.0f + PADDING
#define CUBE_SIZE 3

// Needs refactoring

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

float aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

float sideColors[] = {
    1.0f, 1.0f, 0.0f,   // Bottom face (yellow)
    1.0f, 1.0f, 1.0f,   // Top face (white)
    0.0f, 1.0f, 0.0f,   // Left face (green)
    0.0f, 0.0f, 1.0f,   // Right face (blue)
    1.0f, 0.5f, 0.0f,   // Back face (orange) ??
    1.0f, 0.0f, 0.0f    // Front face (red)
};

int bottomIdx = 0;
int topIdx = 1;
int leftIdx = 2;
int rightIdx = 3;
int backIdx = 4;
int frontIdx = 5;

glm::vec3 blackColor = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 whiteColor = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 magnetaColor = glm::vec3(1.0000f, 0.0000f, 1.0000f);

glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 9.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 defaultView = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
glm::mat4 lastView = defaultView;

struct CubeSideId
{
    int cubeID;
    int sideID;
};

struct Cube model_arr[27];

Cube getModel(int index) { return model_arr[index]; }

glm::vec3 cubeIdToPosition[27];

int collision_isActive = 0;
int collision_found = 0;
glm::vec3 rayOrigin;
glm::vec3 rayDirection;

CubeSideId collisionWith = {-1, -1};
float minT = 100000.0;

// Perform ray-plane intersection test
float intersectRayPlane(glm::vec3 planeNormal, glm::vec3 rawPlainPoint,
                        glm::vec3 cubePosition)
{
    // TODO: shift plane depending on position (currently center of cube is
    // send)
    glm::vec3 planePoint = rawPlainPoint + cubePosition;
    float planeSize = 1.0f;
    float epsilon = 0.0001f;

    float denom = glm::dot(rayDirection, planeNormal);
    if (glm::abs(denom) < epsilon)
    {
        // Ray is parallel or almost parallel to the plane
        return -1.0f;
    }

    glm::vec3 rayToPlane = planePoint - rayOrigin;
    float t = glm::dot(rayToPlane, planeNormal) / denom;

    if (t < 0)
    {
        // Intersection is behind the ray origin
        return -1.0f;
    }

    glm::vec3 intersectionPoint = rayOrigin + t * rayDirection;

    // Check if the intersection point is within the bounds of the plane
    glm::vec3 planeToIntersection = intersectionPoint - planePoint;
    float planeToIntersectionMagnitude = glm::length(planeToIntersection);

    if (planeToIntersectionMagnitude > planeSize)
    {
        // Intersection point is outside the bounds of the plane
        return -1.0f;
    }
    if (collision_found && minT == planeToIntersectionMagnitude)
    {
        // printf("!!MIN!! (M:%f) (T:%f): %f, %f, %f \n",
        // planeToIntersectionMagnitude, t, planePoint.x,
        // planePoint.y,planePoint.z);
    }
    else
    {
        // printf("Intersect's with the (M:%f) (T:%f): %f, %f, %f \n",
        // planeToIntersectionMagnitude, t,
        // planePoint.x,planePoint.y,planePoint.z);
    }
    return planeToIntersectionMagnitude;
}

int isOuterSide(int sideIdx, glm::vec3 sideOrder)
{
    int x, y, z;
    x = sideOrder.x;
    y = sideOrder.y;
    z = sideOrder.z;

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
    int frontCheck = sideIdx == frontIdx && z == 1;
    int backCheck = sideIdx == backIdx && z == -1;
    int topCheck = sideIdx == topIdx && y == 1;
    int bottomCheck = sideIdx == bottomIdx && y == -1;
    int rightCheck = sideIdx == rightIdx && x == 1;
    int leftCheck = sideIdx == leftIdx && x == -1;

    return frontCheck || backCheck || topCheck || bottomCheck || rightCheck ||
           leftCheck;
}

glm::vec3 getVectorColor(int sideIdx, glm::vec3 sideOrder)
{
    if (isOuterSide(sideIdx, sideOrder))
    {
        return glm::vec3(sideColors[sideIdx * 3], sideColors[sideIdx * 3 + 1],
                         sideColors[sideIdx * 3 + 2]);
    }

    return blackColor;
}
float min(float a, float b)
{
    if (a < b)
        return a;
    return b;
}
float max(float a, float b)
{
    if (a > b)
        return a;
    return b;
}

void drawCubeOfRubiksCube(unsigned int shaderProgram, GLuint VAO,
                          glm::mat4 projection, glm::mat4 view, glm::mat4 model,
                          glm::vec3 sideOrder, glm::vec3 cubePosition,
                          int cubeId)
{
    float intersectionMagnitude;
    for (int sideIndex = 0; sideIndex < 6; sideIndex++)
    {
        glm::vec3 sideColor = blackColor;

        glm::vec3 normal = glm::vec3((float)vertices[sideIndex * 24 + 3],
                                     (float)vertices[sideIndex * 24 + 4],
                                     (float)vertices[sideIndex * 24 + 5]);
        glm::vec3 planePoint = glm::vec3(0.0f, 0.0f, 0.0f);
        for (int j = 0; j < 4; j++)
        {
            planePoint.x += (float)vertices[sideIndex * 24 + (j)*6 + 0];
            planePoint.y += (float)vertices[sideIndex * 24 + (j)*6 + 1];
            planePoint.z += (float)vertices[sideIndex * 24 + (j)*6 + 2];
        }
        planePoint.x /= 4;
        planePoint.y /= 4;
        planePoint.z /= 4;

        Cube curCube = getModel(cubeId);

        // in order to test model rotation
        // getModel(25).cubeId
        if (curCube.cubeId == 25)
            sideColor = whiteColor;
        if (isOuterSide(sideIndex, sideOrder))
        {
            // printf("i: %i, planePoint: %f, %f, %f\n", sideIndex,
            // planePoint.x, planePoint.y, planePoint.z);
            if (curCube.cubeId != 25)
            {
                sideColor = curCube.side[sideIndex].color;
                // sideColor =
                // curCube.side[curCube.side[sideIndex].sideIdx].color;
            }
            // int realPos =
            if (collision_isActive)
            {
                intersectionMagnitude =
                    intersectRayPlane(normal, planePoint, cubePosition);
                if (intersectionMagnitude != -1.0f)
                {
                    // TODO: collision is not 100% accurate
                    if (!collision_found)
                    {
                        if (minT > intersectionMagnitude)
                        {
                            minT = min(minT, intersectionMagnitude);
                            collisionWith.cubeID = curCube.cubeId;
                            collisionWith.sideID = sideIndex;
                        }
                    }
                    else
                    {
                        // sideColor = magnetaColor;
                    }
                    // TODO: test differences between checking is real here and
                    // on collision setting it as real
                    int realCubeId = curCube.cubeId;
                    int realSideId = curCube.side[sideIndex].sideIdx;
                    if (collision_found && collisionWith.cubeID == realCubeId
                        //  && collisionWith.sideID == sideIndex
                    )
                    {
                        sideColor = whiteColor;
                    }
                }
            }
        }
        GLuint projectionLoc =
            glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE,
                           glm::value_ptr(projection));

        // Set the view matrix uniform
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // Set the model matrix uniform
        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Set the light position uniform
        GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
        glUniform3f(lightPosLoc, 1.0f, 1.0f, 2.0f);

        // Set the light color uniform
        GLuint lightColorLoc =
            glGetUniformLocation(shaderProgram, "lightColor");
        glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);

        // Set the object color uniform
        GLuint objectColorLoc =
            glGetUniformLocation(shaderProgram, "objectColor");

        glUniform3f(objectColorLoc, sideColor.x, sideColor.y, sideColor.z);

        // Bind the VAO
        glBindVertexArray(VAO);

        // Draw the box
        // add logic to skip inner sides
        glDrawElements(GL_TRIANGLES, 6 * (sideIndex + 1), GL_UNSIGNED_INT, 0);

        // Unbind the VAO
        glBindVertexArray(0);
    }
}

void copy_from_mat_to_float(glm::mat4 mat, float* array, int shift)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            array[shift * 16 + i * 4 + j] = (float)mat[i][j];
        }
    }
}

void init3_3boxes()
{
    // TODO: needs to update after custom sized RB will be implemented
    // model_arr = (struct Cube *) malloc(sizeof(struct Cube)*27);
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
                model_arr[modelIdx].cubeId = modelIdx;
                for (int sideIndex = 0; sideIndex < 6; sideIndex++)
                {
                    model_arr[modelIdx].side[sideIndex].sideIdx = sideIndex;
                    model_arr[modelIdx].side[sideIndex].color =
                        getVectorColor(sideIndex, cubePosition);
                }
                // when initing it is same as thing below
                // cubeIdToPosition[model_arr[modelIdx].cubeId] = cubePosition;
                cubeIdToPosition[modelIdx] = cubePosition;
            }
        }
    }
}

struct RotationConfig
{
    int active;
    float angle;   // -90.0f .. 90.0f
    int axisIndex;
    int axisValue;
};

RotationConfig rotationConfig = {0, 0.0f, 0, 0};

glm::mat4 identityMat = glm::mat4(1.0f);
glm::vec3 rotationVec = glm::vec3(0.0f);

void forwardRotation(const int arr[2], int outArr[2])
{
    int i, j;
    i = arr[0];
    j = arr[1];

    int p = 3 * i + j;
    int nextPos = (3 * p + 2) % 10;
    int nI = nextPos / 3;
    int nJ = nextPos % 3;

    outArr[0] = nI;
    outArr[1] = nJ;
}

void backwardRotation(const int arr[2], int outArr[2])
{
    forwardRotation(arr, outArr);
    int tmpArr[2];
    forwardRotation(outArr, tmpArr);
    forwardRotation(tmpArr, outArr);

    // int i, j;
    // i = arr[0];
    // j = arr[1];
    // int p = 3*i + j;
    // int nextPos = (6*p + 2)%10;
    // int nI = nextPos/3;
    // int nJ = nextPos%3;

    // outArr[0] = nI;
    // outArr[1] = nJ;
}

void bulkRotation(int is_forward, Cube modelIdxes[])
{
    int inArr[2] = {0};
    int outArr[2] = {0};
    for (int i = 0; i < 2; i++)
    {
        inArr[0] = (i) / 3;
        inArr[1] = i % 3;

        Cube tmpV = modelIdxes[inArr[0] * 3 + inArr[1]];
        for (int j = 0; j < 4; j++)
        {
            if (is_forward)
                forwardRotation(inArr, outArr);
            else
                backwardRotation(inArr, outArr);
            Cube tmp = modelIdxes[outArr[0] * 3 + outArr[1]];
            modelIdxes[outArr[0] * 3 + outArr[1]] = tmpV;
            tmpV = tmp;

            inArr[0] = outArr[0];
            inArr[1] = outArr[1];
        }
    }
}
int convertionIncr = 0;

void startRotation(int axisIndex, float axisValue)
{
    if (rotationConfig.active)
        return;

    rotationConfig.angle = 0.0;
    rotationConfig.active = 1;
    rotationConfig.axisIndex = axisIndex;
    rotationConfig.axisValue = axisValue;
    printf("Rotation started: \n active: %d, axisIndex:%d, axisValue:%f\n",
           rotationConfig.active, rotationConfig.axisIndex, axisValue);
    rotationVec[rotationConfig.axisIndex] = 0.5f;
    convertionIncr = 0;
}

void rotateObject(glm::mat4& model, glm::vec3 cubePosition)
{
    if (!rotationConfig.active)
        return;
    if (rotationConfig.angle >= 90.0f
        //|| rotationConfig.angle<= 90.0f
    )
    {
        return;
    }
    if (cubePosition[rotationConfig.axisIndex] == rotationConfig.axisValue)
    {
        model = glm::rotate(identityMat, glm::radians(rotationConfig.angle),
                            rotationVec);
    }
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
        by x -> (top -> front -> bot -> back)
        1 => 5
        5 => 0
        0 => 4
        4 => 1
    */
    int rotateByX[6];
    rotateByX[5] = 1;
    rotateByX[0] = 5;
    rotateByX[4] = 0;
    rotateByX[1] = 4;

    rotateByX[2] = 2;
    rotateByX[3] = 3;

    /*
        by y -> (left -> front -> right-> back)
        2 => 5
        5 => 3
        3 => 4
        4 => 2
    */
    int rotateByY[6];
    rotateByY[2] = 5;
    rotateByY[5] = 3;
    rotateByY[3] = 4;
    rotateByY[4] = 2;

    rotateByY[1] = 1;
    rotateByY[0] = 0;
    /*
        by z -> (left-> top-> right -> bot)
        2 => 1
        1 => 3
        3 => 0
        0 => 3
    */
    int rotateByZ[6];
    rotateByZ[2] = 1;
    rotateByZ[1] = 3;
    rotateByZ[3] = 0;
    rotateByZ[0] = 2;

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
    printf("Update 0 side to: id:%d\n", updateTo[0].sideIdx);

    for (int sideIndex = 0; sideIndex < 6; sideIndex++)
    {   // need to rotate sides
        side[sideIndex] = updateTo[sideIndex];
    }
    printf("Result: id:%d\n", side[0].sideIdx);
}

void updateIndicies()
{
    int modelIdx = 0;
    Cube modelIdxes[9];
    // center piece

    // collection all the indecies that need to be swapped
    for (int p = 0; p < CUBE_SIZE; p++)
    {
        for (int q = 0; q < CUBE_SIZE; q++)
        {
            // getting cubeId (position) that needs to be rotated
            modelIdx = getModelIdxByFixedAxis(rotationConfig.axisValue + 1,
                                              rotationConfig.axisIndex, p, q);
            // getting real cubeId
            Cube cube = getModel(modelIdx);
            // getting position of the real cube
            glm::vec3 pos = cubeIdToPosition[cube.cubeId];
            printf("%f, %f, %f \n", pos.x, pos.y, pos.z);

            // collecting
            modelIdxes[3 * p + q].cubeId = cube.cubeId;

            for (int sideIndex = 0; sideIndex < 6; sideIndex++)
            {   // need to rotate sides
                modelIdxes[3 * p + q].side[sideIndex].color =
                    cube.side[sideIndex].color;
                modelIdxes[3 * p + q].side[sideIndex].sideIdx =
                    cube.side[sideIndex].sideIdx;
            }
            rotateSideIdx(rotationConfig.axisIndex, 1,
                          modelIdxes[3 * p + q].side);
        }
    }
    // Rotating the matrix, in other words setting current values to values they
    // need to be
    bulkRotation(1, modelIdxes);
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
            Cube& toUpdate = model_arr[from];

            Cube valueOfUpdate = modelIdxes[3 * p + q];

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

void draw3_3by3boxes(unsigned int shaderProgram, GLuint VAO,
                     CameraRotation cameraRotation, DragAction dragAction)
{
    glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    // Create the view matrix
    // TODO: explanation
    glm::mat4 view = defaultView;
    int isRotationIsNotEmpty =
        cameraRotation.x != 0.0f && cameraRotation.y != 0.0f;
    if (cameraRotation.type == MOVE_ON_MOVE_OF_CURSOR)
    {
        if (isRotationIsNotEmpty)
        {
            view = glm::rotate(
                view, glm::radians(cameraRotation.angle),
                glm::vec3(cameraRotation.x, cameraRotation.y, 0.0f));
        }
    }
    else if (cameraRotation.type == MOVE_ON_DRAG_OF_CURSOR)
    {
        if (isRotationIsNotEmpty)
        {
            glm::vec3 cameraDirection =
                glm::normalize(cameraPosition - cameraTarget);
            glm::vec3 cameraRight =
                glm::normalize(glm::cross(cameraUp, cameraDirection));
            glm::vec3 cameraUpAdjusted =
                glm::cross(cameraDirection, cameraRight);
            // START GENERATED CODE
            float yaw = 0.0f;     // Horizontal rotation
            float pitch = 0.0f;   // Vertical rotation

            printf("cameraRotation: %f, %f \n", cameraRotation.x,
                   cameraRotation.y);
            yaw += cameraRotation.x;
            pitch += cameraRotation.y;

            // Clamp the pitch to avoid flipping the camera
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;

            // Calculate rotation matrix
            glm::mat4 rotation = glm::mat4(1.0f);
            rotation =
                glm::rotate(rotation, glm::radians(yaw), cameraUpAdjusted);
            rotation = glm::rotate(rotation, glm::radians(pitch), cameraRight);

            cameraPosition =
                glm::vec3(rotation * glm::vec4(cameraPosition, 1.0f));
            cameraTarget = glm::vec3(0.0f);   // World origin
            cameraDirection = glm::normalize(cameraPosition - cameraTarget);
            cameraRight = glm::normalize(glm::cross(cameraUp, cameraDirection));
            cameraUpAdjusted = glm::cross(cameraDirection, cameraRight);

            view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);

            // END OF GENERATED CODE
            lastView = view;
        }
        else
        {
            view = lastView;
        }
    }
    if (dragAction.isActive)
    {
        // other
        // glm::mat4 inverseProjectionMatrix = glm::inverse(projection);
        glm::mat4 inversePV = glm::inverse(projection * view);

        // Convert mouse coordinates to normalized device coordinates (NDC)
        // TODO: validate startX and startY, we need to confirm collision first
        // then write down startX and startY
        float ndcX = (2.0f * dragAction.startX) / WINDOW_WIDTH - 1.0f;
        float ndcY = 1.0f - (2.0f * dragAction.startY) / WINDOW_HEIGHT;

        // Perform ray-casting from mouse position in NDC space
        glm::vec4 rayClip(ndcX, ndcY, -1.0f, 1.0f);
        glm::vec4 rayEye = glm::inverse(projection) * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
        glm::vec3 rayWorld =
            glm::normalize(glm::vec3(glm::inverse(view) * rayEye));

        rayOrigin = cameraPosition;
        rayDirection = rayWorld;

        // printf("rayDirection: %f, %f, %f\n", rayDirection.x, rayDirection.y,
        // rayDirection.z); end other
        collision_isActive = 1;
    }
    else
    {
        collisionWith = {-1, -1};
        minT = 100000;
        collision_found = 0;
        collision_isActive = 0;
    }
    if (collision_found && !rotationConfig.active)
    {
        // TODO: write implementation
        // FOR TEST ONLY
        float clickedXPos = cubeIdToPosition[collisionWith.cubeID][0];
        startRotation(0, clickedXPos);
    }
    // TODO: cache this value
    glm::mat4 model = glm::mat4(1.0f);
    // float angle = static_cast<float>(glfwGetTime()) * 25.0f;  // Adjust the
    // multiplier to change the rotation speed float angle = 0.0f;

    // Scaling twice in order to fit in the screen
    glm::mat4 cleanModel = model;

    // Calculate the rotation angle based on time
    // Create the model matrix with rotation
    float spread = SPREAD;

    int modelIdx = 0;

    if (rotationConfig.active)
    {
        rotationConfig.angle += 1;
        if (rotationConfig.angle >= 90.0f)
        {
            // update positon
            updateIndicies();
            rotationConfig.angle = 0.0f;
            rotationConfig.active = 0;
        }
    }

    for (int i = 0; i < CUBE_SIZE; i++)
    {
        for (int j = 0; j < CUBE_SIZE; j++)
        {
            for (int k = 0; k < CUBE_SIZE; k++)
            {
                // skipping center cube
                if (k == 1 && k == j && j == i)
                    continue;
                modelIdx = 3 * (3 * i + j) + k;
                // Cube cube = getModel(modelIdx);

                glm::vec3 sideOrder = glm::vec3(
                    k - 1, j - 1, i - 1);   // cubeIdToPosition[cube.cubeId];
                glm::vec3 cubePosition = spread * sideOrder;

                cleanModel = glm::mat4(1.0f);

                // testing rotation
                if (rotationConfig.active)
                {
                    model = cleanModel;
                    rotateObject(model, sideOrder);
                }
                else
                    model = cleanModel;
                model = glm::translate(model, cubePosition);

                drawCubeOfRubiksCube(shaderProgram, VAO, projection, view,
                                     model, sideOrder, cubePosition, modelIdx);
            }
        }
    }
    if (dragAction.isActive)
        collision_found = 1;

    // drawCube(shaderProgram, VAO, projection, view, model);
}