#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "config.h"
#include <stdio.h>
#include "vertices.h"

float aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

float sideColors[] = {
    1.0f, 1.0f, 0.0f, // Bottom face (yellow)
    1.0f, 1.0f, 1.0f,  // Top face (white)
    0.0f, 1.0f, 0.0f, // Left face (green)
    0.0f, 0.0f, 1.0f, // Right face (blue)
    1.0f, 0.5f, 0.0f, // Back face (orange) ??
    1.0f, 0.0f, 0.0f // Front face (red)
};


int bottomIdx   = 0;
int topIdx      = 1;
int leftIdx     = 2;
int rightIdx    = 3;
int backIdx     = 4;
int frontIdx    = 5;


glm::vec3 blackColor = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 whiteColor = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 magnetaColor = glm::vec3(1.0000f, 0.0000f, 1.0000f);

glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 9.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 defaultView = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
glm::mat4 lastView = defaultView;

struct CubeSideId{
    int cubeID;
    int sideID;
};

int collision_isActive = 0;
int collision_found = 0;
glm::vec3 rayOrigin;
glm::vec3 rayDirection;

CubeSideId collisionWith = {-1, -1};
float minT = 100000.0;

// Perform ray-plane intersection test
float intersectRayPlane(glm::vec3 planeNormal, glm::vec3 rawPlainPoint, glm::vec3 cubePosition)
{
    // TODO: shift plane depending on position (currently center of cube is send)
    glm::vec3 planePoint = rawPlainPoint+cubePosition;
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
    
    if (planeToIntersectionMagnitude > planeSize )
    {
        // Intersection point is outside the bounds of the plane
        return -1.0f;
    }
    if(collision_found && minT == planeToIntersectionMagnitude){
        printf("!!MIN!! (M:%f) (T:%f): %f, %f, %f \n", planeToIntersectionMagnitude, t, planePoint.x,planePoint.y,planePoint.z);
    } else{
        printf("Intersect's with the (M:%f) (T:%f): %f, %f, %f \n", planeToIntersectionMagnitude, t, planePoint.x,planePoint.y,planePoint.z);
    }
    return planeToIntersectionMagnitude;
}

int isOuterSide(
    int sideIdx,
    glm::vec3 sideOrder 
){
    int x,y,z;
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


   return frontCheck 
        || backCheck 
        || topCheck 
        || bottomCheck 
        || rightCheck 
        || leftCheck;
}

glm::vec3 getVectorColor(
    int sideIdx,
    glm::vec3 sideOrder 
) {

   if(isOuterSide(sideIdx, sideOrder)) {
        return glm::vec3(sideColors[sideIdx*3], sideColors[sideIdx*3+1], sideColors[sideIdx*3+2]);
    }

   return blackColor;
}
float min(float a, float b){
    if(a<b)
        return a;
    return b;
}
float max(float a, float b){
    if(a>b)
        return a;
    return b;
}


void drawCubeOfRubiksCube(
    unsigned int shaderProgram,
    GLuint VAO,
    glm::mat4 projection,
    glm::mat4 view,
    glm::mat4 model,
    glm::vec3 sideOrder ,
    glm::vec3 cubePosition,
    int cubeId
) {
    float intersectionMagnitude;
    for(int sideIndex = 0; sideIndex < 6; sideIndex++) {
        glm::vec3 sideColor = blackColor;
        // 3,4,5 + i*24
        glm::vec3 normal =  glm::vec3(
            (float)vertices[sideIndex*24 + 3],
            (float)vertices[sideIndex*24 + 4],
            (float)vertices[sideIndex*24 + 5]
        );
        glm::vec3 planePoint =  glm::vec3(0.0f, 0.0f,0.0f);
        for(int j = 0; j<4; j++){
            planePoint.x += (float)vertices[sideIndex*24 + (j)*6 + 0];
            planePoint.y += (float)vertices[sideIndex*24 + (j)*6 + 1];
            planePoint.z += (float)vertices[sideIndex*24 + (j)*6 + 2];
        }
        planePoint.x/=4;
        planePoint.y/=4;
        planePoint.z/=4;

        if(isOuterSide(sideIndex, sideOrder)){
            //printf("i: %i, planePoint: %f, %f, %f\n", sideIndex, planePoint.x, planePoint.y, planePoint.z);
            sideColor = getVectorColor(sideIndex, sideOrder);
            // int realPos = 
            if(collision_isActive){
                intersectionMagnitude = intersectRayPlane(normal, planePoint, cubePosition);
                if(intersectionMagnitude != -1.0f) {
                    // TODO: collision is not 100% accurate
                    if(!collision_found){
                        if(minT > intersectionMagnitude){
                            minT = min(minT, intersectionMagnitude);
                            collisionWith.cubeID = cubeId;
                            collisionWith.sideID = sideIndex;
                        }
                    }
                    else{
                        //sideColor = magnetaColor;
                    } 
                    if(collision_found && collisionWith.cubeID == cubeId && collisionWith.sideID == sideIndex){
                        sideColor = whiteColor;
                    }
                }
            }
        }
        GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

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
        GLuint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
        glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);

        // Set the object color uniform
        GLuint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
        

        // glm::vec3 sideColor = getVectorColor(sideIndex, sideOrder);
        // if(collision_isActive && collision_found){
        //     if(collisionWith.x == sideOrder.x
        //     && collisionWith.y == sideOrder.y
        //     && collisionWith.z == sideOrder.z
        //     && collisionWith.w == i)
        //     {
        //     sideColor = whiteColor;
        //     }
        // }
        glUniform3f(objectColorLoc, sideColor.x, sideColor.y, sideColor.z);

        // Bind the VAO
        glBindVertexArray(VAO);

        // Draw the box
        // add logic to skip inner sides
        glDrawElements(GL_TRIANGLES, 6*(sideIndex+1), GL_UNSIGNED_INT, 0);

        // Unbind the VAO
        glBindVertexArray(0);
    }
}


void draw3_3by3boxes(
    unsigned int shaderProgram,
    GLuint VAO,
    CameraRotation cameraRotation,
    DragAction dragAction
) {
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    // Create the view matrix
    // TODO: explanation
    glm::mat4 view = defaultView;
    int isRotationIsNotEmpty = cameraRotation.x!=0.0f && cameraRotation.y!=0.0f ;
    if(cameraRotation.type == MOVE_ON_MOVE_OF_CURSOR) {
        if(isRotationIsNotEmpty){
           view = glm::rotate(view, glm::radians(cameraRotation.angle), glm::vec3(cameraRotation.x, cameraRotation.y, 0.0f));
        }
    } else if(cameraRotation.type == MOVE_ON_DRAG_OF_CURSOR){
        if(isRotationIsNotEmpty){    
            glm::vec3 cameraDirection = glm::normalize(cameraPosition - cameraTarget);
            glm::vec3 cameraRight = glm::normalize(glm::cross(cameraUp, cameraDirection));
            glm::vec3 cameraUpAdjusted = glm::cross(cameraDirection, cameraRight);      
        //START GENERATED CODE
            float yaw = 0.0f; // Horizontal rotation
            float pitch = 0.0f; // Vertical rotation

            printf("cameraRotation: %f, %f \n", cameraRotation.x, cameraRotation.y);
            yaw += cameraRotation.x;
            pitch +=  cameraRotation.y;

            // Clamp the pitch to avoid flipping the camera
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
                            
            // Calculate rotation matrix
            glm::mat4 rotation = glm::mat4(1.0f);
            rotation = glm::rotate(rotation, glm::radians(yaw), cameraUpAdjusted);
            rotation = glm::rotate(rotation, glm::radians(pitch), cameraRight);

            cameraPosition = glm::vec3(rotation * glm::vec4(cameraPosition, 1.0f));
            cameraTarget = glm::vec3(0.0f); // World origin
            cameraDirection = glm::normalize(cameraPosition - cameraTarget);
            cameraRight = glm::normalize(glm::cross(cameraUp, cameraDirection));
            cameraUpAdjusted = glm::cross(cameraDirection, cameraRight);
            
            view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);

            
    //END OF GENERATED CODE
             lastView = view;
        } else {
            view = lastView;
        }
    }
    if(dragAction.isActive){
        //other
        //glm::mat4 inverseProjectionMatrix = glm::inverse(projection);
        glm::mat4 inversePV = glm::inverse(projection* view);

        // Convert mouse coordinates to normalized device coordinates (NDC)
        // TODO: validate startX and startY, we need to confirm collision first then write down
        // startX and startY
        float ndcX = (2.0f * dragAction.startX) / WINDOW_WIDTH - 1.0f;
        float ndcY = 1.0f - (2.0f * dragAction.startY) / WINDOW_HEIGHT;

        // Perform ray-casting from mouse position in NDC space
        glm::vec4 rayClip(ndcX, ndcY, -1.0f, 1.0f);
        glm::vec4 rayEye = glm::inverse(projection) * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
        glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(view) * rayEye));

        rayOrigin = cameraPosition;
        rayDirection = rayWorld;

        //printf("rayDirection: %f, %f, %f\n", rayDirection.x, rayDirection.y, rayDirection.z);
        //end other
        collision_isActive = 1;
    } else {
        collisionWith = {-1, -1};
        minT = 100000;
        collision_found = 0;
        collision_isActive = 0;
    }

    glm::mat4 model = glm::mat4(1.0f);
    //float angle = static_cast<float>(glfwGetTime()) * 25.0f;  // Adjust the multiplier to change the rotation speed
    //float angle = 0.0f;

    // Scaling twice in order to fit in the screen
    glm::mat4 cleanModel = model;
   
    // Calculate the rotation angle based on time
    // Create the model matrix with rotation

    float padding = 0.01f;
    float spread = 1.0f + padding;
    float cubeSize = 3;
    for(int i = 0; i < cubeSize; i++) {
        float z = spread * (i-1);
        for(int j = 0; j < cubeSize; j++) {
            float y = spread * (j-1);
            for(int k = 0; k<cubeSize; k++) {
                // skipping center cube
                if(k==1 && k == j && j == i) continue;

                float x = spread*(k-1);
                glm::vec3 cubePosition = glm::vec3(x, y, z);
                // if (k == 0)
                //     model = glm::rotate(cleanModel, glm::radians((float)(glfwGetTime()* 25.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
                // else if (k == 2)
                //     model = glm::rotate(cleanModel, glm::radians((float)(glfwGetTime()* -25.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
                // else
                //     model = cleanModel;
                //model = glm::translate(model, sidePosition);
                model = glm::translate(cleanModel, cubePosition);

                glm::vec3 sideOrder = glm::vec3(k-1, j-1, i-1);
                int cubeId = i+j+k;
                drawCubeOfRubiksCube(
                    shaderProgram,
                    VAO,
                    projection,
                    view,
                    model,
                    sideOrder,
                    cubePosition,
                    cubeId
                    );
            }
        }
    }
    if(dragAction.isActive)
        collision_found = 1;
   

    //drawCube(shaderProgram, VAO, projection, view, model);
}