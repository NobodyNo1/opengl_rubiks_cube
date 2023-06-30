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

        // in order to test model rotation
        // getModel(25).cubeId
        if (curCube.cubeId == 25)
            sideColor = whiteColor;
        if (!isOuterSide(sideIndex, cubePosition))
         continue;
            // printf("i: %i, planePoint: %f, %f, %f\n", sideIndex,
            // planePoint.x, planePoint.y, planePoint.z);
            if (curCube.cubeId != 25)
            {
                sideColor = curCube.side[sideIndex].color;
                // sideColor =
                // curCube.side[curCube.side[sideIndex].sideIdx].color;
            }
        
            float intersectionMagnitude =
                intersectRayPlane(normal, planePoint, cubePositionWithSpread);
                // intersectRayPlane(normal, planePoint, cubeIdToPosition[getModel(cubeId).cubeId]);
                
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
    float intersectionMagnitude;
    for (int sideIndex = 0; sideIndex < 6; sideIndex++)
    {
        glm::vec3 sideColor = blackColor;

        // glm::vec3 normal = glm::vec3((float)vertices[sideIndex * 24 + 3],
        //                              (float)vertices[sideIndex * 24 + 4],
        //                              (float)vertices[sideIndex * 24 + 5]);
        // glm::vec3 planePoint = glm::vec3(0.0f, 0.0f, 0.0f);
        // for (int j = 0; j < 4; j++)
        // {
        //     planePoint.x += (float)vertices[sideIndex * 24 + (j)*6 + 0];
        //     planePoint.y += (float)vertices[sideIndex * 24 + (j)*6 + 1];
        //     planePoint.z += (float)vertices[sideIndex * 24 + (j)*6 + 2];
        // }
        // planePoint.x /= 4;
        // planePoint.y /= 4;
        // planePoint.z /= 4;

        Cube curCube = getModel(cubeId);

        // in order to test model rotation
        // getModel(25).cubeId
        if (curCube.cubeId == 25)
            sideColor = magnetaColor;
        if (isOuterSide(sideIndex, cubePosition))
        {
            if (curCube.cubeId != 25)
            {
                sideColor = curCube.side[sideIndex].color;
            }
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
            // START GENERATED CODE
            float yaw = 0.0f;     // Horizontal rotation
            float pitch = 0.0f;   // Vertical rotation

            printf("cameraPositionState: %f, %f \n", cameraPositionState.x,
                   cameraPositionState.y);
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
        // TODO: write implementation
        
        // FOR TEST ONLY
        // take clicked side 
        // if front, back -> x
        // if left, right -> y
        // if top, bottom -> x
        int rotBy = 0;
        // if(collisionWith.selectedSideId == leftIdx || collisionWith.selectedSideId  == rightIdx){
        //     rotBy = 2;
        // }
        if(collisionWith.selectedSideId == LEFT_IDX || collisionWith.selectedSideId  == RIGHT_IDX){
            printf("    raw value is:left/right\n");
        } else if(collisionWith.selectedSideId == TOP_IDX || collisionWith.selectedSideId  == BOTTOM_IDX){
            printf("    raw value is:top/bottom\n");
        } else{
            printf("    raw value is:front/back\n");
        }
        if(collisionWith.cube.side[collisionWith.selectedSideId].sideIdx == LEFT_IDX || collisionWith.cube.side[collisionWith.selectedSideId].sideIdx  == RIGHT_IDX){
            printf("    actual value is:left/right\n");
        } else if(collisionWith.cube.side[collisionWith.selectedSideId].sideIdx == TOP_IDX || collisionWith.cube.side[collisionWith.selectedSideId].sideIdx  == BOTTOM_IDX){
            printf("    actual value is:top/bottom\n");
        } else{
            printf("    actual value is:front/back\n");
        }
        if(
            collisionWith.selectedSideId == LEFT_IDX || collisionWith.selectedSideId  == RIGHT_IDX
            // collisionWith.cube.side[collisionWith.selectedSideId].sideIdx == leftIdx 
            // || collisionWith.cube.side[collisionWith.selectedSideId].sideIdx == rightIdx
        ){
            rotBy = 2;
        } else if( collisionWith.selectedSideId == FRONT_IDX || collisionWith.selectedSideId  == BACK_IDX){
            rotBy = 0;
        } else{
            rotBy = 1;
        }
        float clickedXPos = cubeIdToPosition[collisionWith.modelIdx][rotBy];
        printf(
            "           rotAxis: %d, X:%f, Y:%f, Z:%f\n",
            rotBy,
            cubeIdToPosition[collisionWith.modelIdx].x,
            cubeIdToPosition[collisionWith.modelIdx].y,
            cubeIdToPosition[collisionWith.modelIdx].z
        );
        printf("        AXIS_VALUE: %f \n", clickedXPos);
        startRotation(rotBy, clickedXPos, 1);
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
        rotationConfig.angle =  (glfwGetTime()-rotationConfig.startRotationTime)* 200.0f;
        if (rotationConfig.angle >= 90.0f)
        {
            // update positon
            updateIndicies();
            
            rotationVec = {0.0f , 0.0f, 0.0f};
            rotationConfig.angle = 0.0f;
            rotationConfig.active = 0;
        }
    }
}