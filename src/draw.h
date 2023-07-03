#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stdio.h>

#include "config.h"
#include "input_processor.h"
#include "vertices.h"
#include "cube.h"

#define PADDING 0.01f
#define SPREAD 1.0f + PADDING

// for dev testing
#define TRANSPARENCY 0
#define FIX_COLOR_CUBE 0
#define FIXED_COLORED_CUBE_ID 25

float aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

glm::vec3 cameraPosition  = glm::vec3(0.0f, 0.0f, 9.0f);
glm::vec3 cameraTarget    = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp        = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 defaultView     = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
glm::mat4 lastView        = defaultView;

struct SelectedCubeSide
{
    Cube cube;
    int selectedSideId;
    int modelIdx;
};

int collision_isActive = 0;
int collision_found = 0;
glm::vec3 rayOrigin;
glm::vec3 rayDirection;

SelectedCubeSide collisionWith;

int isVisibleForView(int sideIdx, glm::vec3 cubePosition) {
    glm::vec3 cameraDirection =
        glm::normalize(cameraPosition - cameraTarget);
    glm::vec3 cameraRight =
        glm::normalize(glm::cross(cameraUp, cameraDirection));
    glm::vec3 cameraUpAdjusted =
        glm::cross(cameraDirection, cameraRight);

    // for testing
    float curyaw = glm::degrees(atan2(cameraDirection.x, cameraDirection.z));
    float pitch = asin(cameraDirection.y);

    int x, y, z;
    x = cubePosition.x;
    y = cubePosition.y;
    z = cubePosition.z;

    int frontCheck  =    sideIdx  == FRONT_IDX   &&   z ==  1;
   
    int backCheck   =    sideIdx  == BACK_IDX    &&   z == -1;
    int topCheck    =    sideIdx  == TOP_IDX     &&   y ==  1;
    int bottomCheck =    sideIdx  == BOTTOM_IDX  &&   y == -1;
    int rightCheck  =    sideIdx  == RIGHT_IDX   &&   x ==  1;
    int leftCheck   =    sideIdx  == LEFT_IDX    &&   x == -1;
    
    // 0    90 180 -90 0
    if(curyaw < 0){
        curyaw = 360+curyaw;
    }

    float minYaw =0;
    float maxYaw = 180;
    float shift = 0;
    if(frontCheck){
        if(270 <= curyaw && curyaw<= 360)
            return 1;
        if(0 <= curyaw && curyaw<= 90)
            return 1;
    } else if(rightCheck){
        minYaw = 0;
        maxYaw = 180;
    } else if(backCheck){
        minYaw = 90;
        maxYaw = 270;
    } else if(leftCheck){
        minYaw = 180;
        maxYaw = 360;
    }
    if(rightCheck || leftCheck || backCheck){
        return minYaw <= curyaw && curyaw<= maxYaw;
    }
    if(topCheck && pitch > 0)
        return 1;
    if(bottomCheck && pitch < 0)
        return 1;
    // front = yaw (-90, 90)
    // right = yaw (0, +-180)
    // back = yaw (90, -90)
    // left = (+-180 , 0)
    return 0;
}

// Perform ray-plane intersection test
float intersectRayPlane(glm::vec3 planeNormal, glm::vec3 rawPlainPoint,
                        glm::vec3 cubePositionWithSpread)
{
    // TODO: shift plane depending on position (currently center of cube is
    // send)
    glm::vec3 planePoint = rawPlainPoint + cubePositionWithSpread;
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
    
    return planeToIntersectionMagnitude;
}

void findIntersectionForCube(
    int cubeId,
    glm::vec3 cubePosition,
    glm::vec3 cubePositionWithSpread,
    float *minimalIntersectionMagnitude,
    SelectedCubeSide *foundMinInfo
) {
    
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

        if (!isOuterSide(sideIndex, cubePosition) || !isVisibleForView(sideIndex, cubePosition))
            continue;
        
        float intersectionMagnitude =
            intersectRayPlane(normal, planePoint, cubePositionWithSpread);
            // intersectRayPlane(normal, planePoint, cubeIdToPosition[getModel(cubeId).cubeId]);
        printf("x:%f, y:%f, z:%f | M:%f\n", cubePosition.x, cubePosition.y, cubePosition.z, intersectionMagnitude);
        if (intersectionMagnitude != -1.0f)
        {
            
            if (*minimalIntersectionMagnitude > intersectionMagnitude)
            {
                *minimalIntersectionMagnitude = min(
                    *minimalIntersectionMagnitude,
                    intersectionMagnitude
                );
                foundMinInfo->cube = curCube;
                foundMinInfo->selectedSideId = sideIndex;
                foundMinInfo->modelIdx = cubeId;
            }
        }
    }
}

float INTERSECTION_MAGNITUDE_LIMIT = 100000.0f;

int findIntersection(
    float *minimalIntersectionMagnitude,
    SelectedCubeSide *foundMinInfo
) {
    float spread = SPREAD;

    for (int i = 0; i < CUBE_SIZE; i++)
    {
        for (int j = 0; j < CUBE_SIZE; j++)
        {
            for (int k = 0; k < CUBE_SIZE; k++)
            {
                // skipping center cube
                if (k == 1 && k == j && j == i)
                    continue;
                
                int modelIdx = 3 * (3 * i + j) + k;
                // Cube cube = getModel(modelIdx);

                glm::vec3 cubePosition = glm::vec3(
                    k - 1, j - 1, i - 1);   // cubeIdToPosition[cube.cubeId];

                glm::vec3 cubePositionWithSpread = spread * cubePosition;
                findIntersectionForCube(
                    modelIdx, cubePosition, cubePositionWithSpread,
                    minimalIntersectionMagnitude, foundMinInfo 
                );
            }
        }
    }
    if(*minimalIntersectionMagnitude!= INTERSECTION_MAGNITUDE_LIMIT){
        //something happened
        return 1;
    }
    return 0;
}

void drawCubeOfRubiksCube(unsigned int shaderProgram, GLuint VAO,
                          glm::mat4 projection, glm::mat4 view, glm::mat4 model,
                          glm::vec3 cubePosition, glm::vec3 cubePositionWithSpread,
                          int cubeId)
{
    for (int sideIndex = 0; sideIndex < 6; sideIndex++)
    {
        glm::vec3 sideColor = blackColor;

        Cube curCube = getModel(cubeId);

        // in order to test model rotation
        // getModel(25).cubeId
#if FIX_COLOR_CUBE
        if (curCube.cubeId == 25)
            sideColor = magnetaColor;
#endif
        if (isOuterSide(sideIndex, cubePosition))
        {
#if FIX_COLOR_CUBE
            if (curCube.cubeId != 25)
                sideColor = curCube.side[sideIndex].color;
#else
            sideColor = curCube.side[sideIndex].color;
#endif
           
            if (collision_found && collisionWith.modelIdx == cubeId
                    && collisionWith.selectedSideId == sideIndex
            )
            {
                sideColor = whiteColor;
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

        GLuint alphaValLoc =
            glGetUniformLocation(shaderProgram, "alphaVal");
        float alphaVal =  1.0f;
#if TRANSPARENCY
        if(isOuterSide(sideIndex, cubePosition) && isVisibleForView(sideIndex, cubePosition)){
            alphaVal = 0.2f;
        } else
            alphaVal = 0.0f;
        if(collision_found){
             if(collisionWith.modelIdx == cubeId
                    && collisionWith.selectedSideId == sideIndex)
            {
                alphaVal = 1.0f;
            } else{
                if(isOuterSide(sideIndex, cubePosition)){
                    alphaVal = 0.2f;
                } else
                    alphaVal = 0.0f;
            }
        }
#endif
        glUniform1f(alphaValLoc, alphaVal);

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


glm::mat4 identityMat = glm::mat4(1.0f);
glm::vec3 rotationVec = glm::vec3(0.0f);


int convertionIncr = 0;

void startRotation(int axisIndex, float axisValue, int isClockWise)
{
    if (rotationConfig.active)
        return;


    rotationConfig.startRotationTime = glfwGetTime();
    rotationConfig.angle = 0.0;
    rotationConfig.active = 1;
    rotationConfig.axisIndex = axisIndex;
    rotationConfig.axisValue = axisValue;
    rotationConfig.isClockWise = isClockWise;
    printf("Rotation started: \n active: %d, axisIndex:%d, axisValue:%f\n",
           rotationConfig.active, rotationConfig.axisIndex, axisValue);
    rotationVec[axisIndex] = 0.5f;
    convertionIncr = 0;
}

void rotateObject(glm::mat4& model, glm::vec3 cubePosition)
{
    if (!rotationConfig.active)
        return;
    if (rotationConfig.angle >= 90.0f)
    {
        return;
    }
    
    int posToUpdate = (int)cubePosition[rotationConfig.axisIndex];
    if (posToUpdate == rotationConfig.axisValue)
    {
        if(convertionIncr<9) {
            //printf("~~~~~ found items: z:%f, y:%f, x:%f\n", cubePosition.x, cubePosition.y, cubePosition.z);
            convertionIncr++;
        }
        model = glm::rotate(identityMat, glm::radians(rotationConfig.angle),
                            rotationVec);
    }
}

glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
glm::mat4 viewMatrix;

void handleCameraPosition(
    int isRotationIsNotEmpty,
    CameraPositionState cameraPositionState
) {
    if (cameraPositionState.type == MOVE_ON_MOVE_OF_CURSOR)
    {
        if (isRotationIsNotEmpty)
        {
            viewMatrix = glm::rotate(
                viewMatrix, glm::radians(cameraPositionState.angle),
                glm::vec3(cameraPositionState.x, cameraPositionState.y, 0.0f));
        }
    }
    else if (cameraPositionState.type == MOVE_ON_DRAG_OF_CURSOR)
    {
        if (isRotationIsNotEmpty)
        {

            glm::vec3 cameraDirection =
                glm::normalize(cameraPosition - cameraTarget);
            glm::vec3 cameraRight =
                glm::normalize(glm::cross(cameraUp, cameraDirection));
            glm::vec3 cameraUpAdjusted =
                glm::cross(cameraDirection, cameraRight);

            // for testing
            float curyaw = glm::degrees(atan2(cameraDirection.x, cameraDirection.z));
            float curpitch = asin(cameraDirection.y);
            float updatedYaw = curyaw < 0 ? 360+curyaw : curyaw;
            printf("yaw: %f -> %f \n", curyaw, updatedYaw);
            //printf("pitch: %f\n", curpitch);

            // START GENERATED CODE
            float yaw = 0.0f;     // Horizontal rotation
            float pitch = 0.0f;   // Vertical rotation

            // printf("cameraPositionState: %f, %f \n", cameraPositionState.x,
                //    cameraPositionState.y);
            yaw += cameraPositionState.x;
            pitch += cameraPositionState.y;

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

            // viewMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUpAdjusted);
            viewMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);

            // END OF GENERATED CODE
            lastView = viewMatrix;
        }
        else
        {
            viewMatrix = lastView;
        }
    }
    
}

void handleSelection(){
    if (dragAction.isActive && !collision_found)
    {
        // other
        // glm::mat4 inverseProjectionMatrix = glm::inverse(projection);
        glm::mat4 inversePV = glm::inverse(projectionMatrix * viewMatrix);

        // Convert mouse coordinates to normalized device coordinates (NDC)
        // TODO: validate startX and startY, we need to confirm collision first
        // then write down startX and startY
        float ndcX = (2.0f * dragAction.x) / WINDOW_WIDTH - 1.0f;
        float ndcY = 1.0f - (2.0f * dragAction.y) / WINDOW_HEIGHT;

        // Perform ray-casting from mouse position in NDC space
        glm::vec4 rayClip(ndcX, ndcY, -1.0f, 1.0f);
        glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
        glm::vec3 rayWorld =
            glm::normalize(glm::vec3(glm::inverse(viewMatrix) * rayEye));

        rayOrigin = cameraPosition;
        rayDirection = rayWorld;

        // printf("rayDirection: %f, %f, %f\n", rayDirection.x, rayDirection.y,
        // rayDirection.z); end other
        collision_isActive = 1;

        float minimalIntersectionMagnitude = INTERSECTION_MAGNITUDE_LIMIT;
        SelectedCubeSide foundMinInfo;
        int intersectionFound = findIntersection(&minimalIntersectionMagnitude, &foundMinInfo);
        if(intersectionFound) {
            dragAction.startX = dragAction.x;
            dragAction.startY = dragAction.y;
            collisionWith = foundMinInfo;
            // collisionWith.cube = curCube;
            // collisionWith.selectedSideId = sideIndex;
            // collisionWith.modelIdx = cubeId;
            collision_found = 1;

            // Need to store pos as start
            // next update need to compare them and do something ;)
        }
    }
    else if(!dragAction.isActive)
    {
        // needs to cleared
        //collisionWith;
        collision_found = 0;
        collision_isActive = 0;
    }
    if (collision_found && !rotationConfig.active)
    {
        float delta = 7.0f;
        float length = sqrt(pow(dragAction.startX - dragAction.x, 2) + pow(dragAction.startY - dragAction.y, 2));
        float xDiff = abs(dragAction.startX - dragAction.x);
        float yDiff = abs(dragAction.startY - dragAction.y);
        int rotAxis = 0;
        int isClockWise = 1;
        if(length < delta){
            return;
        } else {
            if(xDiff < yDiff){
                rotAxis = 0;
                isClockWise =  dragAction.y - dragAction.startY > 0;
            }
            else{
                rotAxis = 1;
                isClockWise =  dragAction.x - dragAction.startX > 0;
            }
        }
        // FOR TEST ONLY
        // take clicked side 
        // if front, back -> x
        // if left, right -> y
        // if top, bottom -> x
        int rotBy = 0;
        
        if(collisionWith.selectedSideId == LEFT_IDX || collisionWith.selectedSideId  == RIGHT_IDX){
            if(rotAxis == 0){
               rotBy = 2;
            } else{
                rotBy = 1;
            }
            if(collisionWith.selectedSideId  == RIGHT_IDX && rotBy == 2)
                isClockWise = !isClockWise;
        } else if(collisionWith.selectedSideId == FRONT_IDX || collisionWith.selectedSideId  == BACK_IDX){
            // front: x,y -> x,y
            // left: x,y -> z,y
            // top (depends on the orientation): x,y -> x,z or z, x
            rotBy = rotAxis;
            if(collisionWith.selectedSideId  == BACK_IDX && rotBy == 0)
                isClockWise = !isClockWise;
        } else if(collisionWith.selectedSideId == TOP_IDX || collisionWith.selectedSideId  == BOTTOM_IDX){
            glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPosition);
            float yaw = glm::degrees(atan2(cameraDirection.x, cameraDirection.z));
            if(BOTTOM_IDX == collisionWith.selectedSideId ){
                yaw = -yaw;
            }
    
            //dragAction.startX - dragAction.x
            float xStart = dragAction.startX* cosf(convert_to_rad(yaw)) + dragAction.startY *sinf(convert_to_rad(yaw));
            float yStart = -dragAction.startX* sinf(convert_to_rad(yaw)) + dragAction.startY *cosf(convert_to_rad(yaw));
            float x = dragAction.x* cosf(convert_to_rad(yaw)) + dragAction.y *sinf(convert_to_rad(yaw));
            float y = -dragAction.x* sinf(convert_to_rad(yaw)) + dragAction.y *cosf(convert_to_rad(yaw));
            xDiff = abs(xStart - x);
            yDiff = abs(yStart - y);
            float rawDiffY = y - yStart;
            float rawDiffX =  x - xStart ;

            //xˆ = x cos θ + y sin θ and ˆy = −x sin θ + y cos θ
            //x = ˆx cos θ − yˆsin θ and y = ˆx sin θ + ˆy cos θ.
             if(xDiff < yDiff){
                rotBy = 0;
                isClockWise = rawDiffY < 0;
            }
            else{
                rotBy = 2;
                isClockWise = rawDiffX > 0;
                if(BOTTOM_IDX == collisionWith.selectedSideId ){
                    isClockWise = !isClockWise;
                }
            }
           // TODO: define view angle
            //rotBy = 1;
        }
        float clickedXPos = cubeIdToPosition[collisionWith.modelIdx][rotBy];
    
        startRotation(rotBy, clickedXPos, isClockWise);
    }
    
}

void handleSideRotation();

void draw3_3by3boxes(unsigned int shaderProgram, GLuint VAO,
                     CameraPositionState cameraPositionState,
                     DragAction dragAction)
{
    // Create the view matrix
    // TODO: explanation
    viewMatrix = defaultView;
    int isRotationIsNotEmpty =
        cameraPositionState.x != 0.0f && cameraPositionState.y != 0.0f;
    handleCameraPosition(isRotationIsNotEmpty, cameraPositionState);
    handleSelection();

    glm::mat4 cleanModelMatrix = glm::mat4(1.0f);
    glm::mat4 modelMatrix = cleanModelMatrix;

    // Calculate the rotation angle based on time
    // Create the model matrix with rotation
    float spread = SPREAD;

    int modelIdx = 0;

    handleSideRotation();

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

                glm::vec3 cubePosition = glm::vec3(
                    k - 1, j - 1, i - 1);   // cubeIdToPosition[cube.cubeId];
                

                modelMatrix = cleanModelMatrix;

                // testing rotation
                if (rotationConfig.active)
                {
                    rotateObject(modelMatrix, cubePosition); //cubeIdToPosition[getModel(modelIdx).cubeId]);
                }
                glm::vec3 cubePositionWithSpread = spread * cubePosition;
                modelMatrix = glm::translate(modelMatrix, cubePositionWithSpread);

                drawCubeOfRubiksCube(
                    shaderProgram,
                    VAO,
                    projectionMatrix,
                    viewMatrix,
                    modelMatrix, 
                    cubePosition,
                    cubePositionWithSpread,
                    modelIdx
                );
            }
        }
    }

    // drawCube(shaderProgram, VAO, projection, view, model);
}

void handleSideRotation() {
    if (rotationConfig.active)
    {
        int direction = 1;
        if(!rotationConfig.isClockWise){
            direction = -1;
        }
        rotationConfig.angle =  direction*(glfwGetTime()-rotationConfig.startRotationTime)* 200.0f;
        if (abs(rotationConfig.angle) >= 90.0f)
        {
            // update positon
            updateIndicies();
            
            rotationVec = {0.0f , 0.0f, 0.0f};
            rotationConfig.angle = 0.0f;
            rotationConfig.active = 0;
        }
    }
}