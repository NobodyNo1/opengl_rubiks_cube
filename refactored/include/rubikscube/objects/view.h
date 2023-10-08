#include <objects/object.h>
#include <objects/rubiks_cube.h>
#include <config.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <objects/triangle.h>
#include <handler/actions/drag.h>
// #include <glm/gtc/type_ptr.hpp>

class View: public Object{
    private:
        // ideally should be just list of objects
        RubiksCube *rubiksCube;
        float aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
        glm::vec3 cameraPosition   = glm::vec3(0.0f, 0.0f, 9.0f);
        glm::vec3 cameraTarget     = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraUp         = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::mat4 defaultView      = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
        glm::mat4 viewMatrix       = defaultView;
        Triangle *triangle;

public:
    View() {
        // number depends on number of objects
        GLuint VAO[1];
        glGenVertexArrays(1, VAO);
        // //rubicsCube = new RubicsCube(&VAO[0]);
        rubiksCube = new RubiksCube(VAO[0]);
        // triangle =  new Triangle();
    }
    ~View(){
        delete rubiksCube;
    }
    void update(DragAction* drag){
        handleCameraPosition(drag->dragState);
    }

    void draw(){
        rubiksCube->draw(
            projectionMatrix,
            viewMatrix
        );
        // triangle->draw();
    }



    void handleCameraPosition(
        DragState dragState
    ) {

        int isDragActive = dragState.x != 0.0f && dragState.y != 0.0f;
    
        if (!isDragActive) return;

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

        float yaw = 0.0f;     // Horizontal rotation
        float pitch = 0.0f;   // Vertical rotation

        yaw += dragState.x;
        pitch += dragState.y;

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
    }
};