#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "object.hpp"
#include <glm/glm.hpp>
#include "cube.hpp"
#include <tools/shader_loader.hpp>
#include <iostream>
#include <stdexcept>
#include "stdbool.h"
#include "object_config.hpp"
#include <handler/actions/drag.hpp>
#include "stdbool.h"
#include <glm/gtc/matrix_transform.hpp>

#define PADDING 0.01f
#define SPREAD 1.0f + PADDING

// Indices of cube sides

const float INTERSECTION_MAGNITUDE_LIMIT = 100000.0f;
// Define the vertex positions for the box
GLfloat vertices[] = {
    // Position             // Normals
    -0.5f, -0.5f, -0.5f,    0.0f, -1.0f, 0.0f,   // Bottom left back
     0.5f, -0.5f, -0.5f,    0.0f, -1.0f, 0.0f,   // Bottom right back
     0.5f, -0.5f,  0.5f,    0.0f, -1.0f, 0.0f,   // Bottom right front
    -0.5f, -0.5f,  0.5f,    0.0f, -1.0f, 0.0f,   // Bottom left front

    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,    // Top left back
     0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,    // Top right back
     0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    // Top right front
    -0.5f,  0.5f,   0.5f,   0.0f, 1.0f, 0.0f,    // Top left front

    -0.5f, -0.5f, -0.5f,    -1.0f, 0.0f, 0.0f,   // Bottom left back
    -0.5f,  0.5f, -0.5f,    -1.0f, 0.0f, 0.0f,   // Top left back
    -0.5f,  0.5f,  0.5f,    -1.0f, 0.0f, 0.0f,   // Top left front
    -0.5f, -0.5f,  0.5f,    -1.0f, 0.0f, 0.0f,   // Bottom left front

     0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    // Bottom right back
     0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    // Top right back
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    // Top right front
     0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    // Bottom right front

    -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f,   // Bottom left back
     0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f,   // Bottom right back
     0.5f,  0.5f, -0.5f,    0.0f, 0.0f, -1.0f,   // Top right back
    -0.5f,  0.5f, -0.5f,    0.0f, 0.0f, -1.0f,   // Top left back

    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    // Bottom left front
     0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    // Bottom right front
     0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    // Top right front
    -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    // Top left front
};

GLuint indices[] = {
    // Bottom face
    0, 1, 2,
    2, 3, 0,

    // Top face
    4, 5, 6,
    6, 7, 4,

    // Left face
    8, 9, 10,
    10, 11, 8,

    // Right face
    12, 13, 14,
    14, 15, 12,

    // Back face
    16, 17, 18,
    18, 19, 16,

    // Front face
    20, 21, 22,
    22, 23, 20
};

// no need to hold initial position
// if and only if draw loop matches with select and init loops
// struct Side
// {
//     int sideIdx;
//     glm::vec3 color;   // color does not change because of that we can keep its
// };

// struct Cube
// {
//     int cubeId;
//     Side side[6];
// };


// Description of how cube side ? should be rotated
struct RotationConfig
{
    bool isActive;
    float angle;   // -90.0f .. 90.0f
    int axisIndex;
    int axisValue;
    float startRotationTime;
    bool isClockWise;
};


class RubiksCube : public Object {
private:
    GLuint RB_VAO;
    Cube* cubes;
    // real cube positions
    glm::vec3 *cubeIdToPosition;
    RotationConfig rotationConfig = { 0, 0.0f, 0, 0, 0.0f, 0 };
    const int   cubeSize     = CUBE_SIZE;
    const int   cubeElements = cubeSize * cubeSize * cubeSize;
    const float cubeShift    = CUBE_SHIFT;
    glm::mat4 defaultModelMatrix = glm::mat4(1.0f);
    Shader *rcShader;
    DragState startDrag = { 0.0f, 0.0f };
    glm::mat4 identityMat = glm::mat4(1.0f);


public:
    RubiksCube(GLuint VAO) {
        // === SETUP DATA ===
        RB_VAO = VAO;
        //RB_VAO = &VAO;
        // Create and bind the Vertex Array Object (VAO)
        glBindVertexArray(RB_VAO);

        // Create and bind the Vertex Buffer Object (VBO)
        GLuint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Create and bind the Element Buffer Object (EBO)
        GLuint EBO;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Set the vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Unbind the VAO, VBO, and EBO
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        // HARDCODED for now
        const char* vertex_shader_path = "res/shaders/box.vs";
        const char* fragment_shader_path = "res/shaders/box.fs";

        rcShader = new Shader(vertex_shader_path, fragment_shader_path);
        initBoxes(3,3,3);
    }

    // make it generic
    // private
    void initBoxes(int x, int y, int z)
    {
        // TODO: needs to update after custom sized RB will be implemented
        // cubes = (struct Cube *) malloc(sizeof(struct Cube)*27);
        cubes = new Cube[cubeElements];
        // why we need this?
        cubeIdToPosition = new glm::vec3[cubeElements];
        int modelIdx = 0;

        for (int i = 0; i < cubeSize; i++)
        {
            bool isInnerZ = i>0 && i< cubeSize-1;

            for (int j = 0; j < cubeSize; j++)
            {
                bool isInnerY = j>0 && j< cubeSize-1;

                for (int k = 0; k < cubeSize; k++)
                {
                    bool isInnerX = k>0 && k< cubeSize-1;
                    if (isInnerZ && isInnerY && isInnerX)
                        continue;
                    // skipping center cube
                    // START : works only for the 3x3x3 cube
                    // if (k == 1 && k == j && j == i)
                    //     continue;
                    
                    modelIdx = cubeSize * (cubeSize * i + j) + k;
                    
                    glm::vec3 cubePosition = glm::vec3(k - cubeShift, j - cubeShift, i - cubeShift);
                    cubes[modelIdx] = Cube(modelIdx, cubePosition);
                    // END : works only for the 3x3x3 cube
                    
                    // when initing it is same as thing below
                    // cubeIdToPosition[cubes[modelIdx].cubeId] = cubePosition;
                    cubeIdToPosition[modelIdx] = cubePosition;
                }
            }
        }
    }

    ~RubiksCube() {
        RB_VAO = 0;
        delete cubes;
        delete cubeIdToPosition;
    }

    void update(
        glm::mat4 projection,
        glm::mat4 view,
        glm::vec3 cameraPosition,
        DragAction* dragAction
    ) {
        handleRotation(projection, view, cameraPosition, dragAction);
    }

    void handleRotation(
        glm::mat4 projection,
        glm::mat4 view,
        glm::vec3 cameraPosition,
        DragAction* dragAction
    ) {
        if (rotationConfig.isActive) {
           rotationAnimation();
        } else {
            handleRotateAction(projection, view, cameraPosition, dragAction);
        }
    }

    void rotationAnimation() {
        if (!rotationConfig.isActive) return;
        
    }

    void handleRotateAction(
        glm::mat4 projection,
        glm::mat4 view,
        glm::vec3 cameraPosition,
        DragAction* dragAction
    ) {
        if (rotationConfig.isActive) return;

        if(!dragAction->dragState.isActive()) return;
        if (startDrag.isActive())
        {
            //calculate swipe
            /*
                if start - cur > delta
                     start rotation
                else
                    return 
            */
        } 
        // check if collides
        startDrag = dragAction->dragState;
    }

    bool isRotActive = true;
    float angle = 0.0f;
    float angularSpeed = 90.0f/180;
    int axisIndex = 1;
    bool direction = true;
    float axisValue = 1.5f;
    glm::vec3 rotationVec = glm::vec3(0.0f, 0.5f, 0.0f);

    void updateCubePosition(){
         for (int i = 0; i < cubeSize; i++)
        {
            bool isInnerZ = i>0 && i< cubeSize-1;

            for (int j = 0; j < cubeSize; j++)
            {
                bool isInnerY = j>0 && j< cubeSize-1;

                for (int k = 0; k < cubeSize; k++)
                {
                    bool isInnerX = k>0 && k< cubeSize-1;
                    if (isInnerZ && isInnerY && isInnerX)
                        continue;
                    int modelIdx = cubeSize * (cubeSize * i + j) + k;
                    //  glm::vec3 cubePosition = glm::vec3(
                    //     k - cubeShift, j - cubeShift, i - cubeShift);   // cubeIdToPosition[cube.cubeId];
                    glm::vec3 cubePosition = cubes[modelIdx].position;
                    float posToUpdate = cubePosition[axisIndex];
                    if (posToUpdate == axisValue)
                    {
                        cubes[modelIdx].rotate(posToUpdate>0.0f,direction,axisIndex);
                    }
                }
            }
        }
    }

    void rotationUpdate(){
        if(!isRotActive) return;
        angle+=angularSpeed;
        if(angle > 90.0f){
            updateCubePosition();
            isRotActive = false;
            return;
        }
        if (angle >= 360.0f){
            angle = 0.0f;
        }
    }

    void rotateObject(glm::mat4& model, glm::vec3 cubePosition)
    {
        // if (!rotationConfig.active)
        //     return;
        // if (rotationConfig.angle >= 90.0f)
        // {
        //     return;
        // }
        
        float posToUpdate = cubePosition[axisIndex];
        if (posToUpdate == axisValue)
        {
            // if(convertionIncr<9) {
            //     //printf("~~~~~ found items: z:%f, y:%f, x:%f\n", cubePosition.x, cubePosition.y, cubePosition.z);
            //     convertionIncr++;
            // }
            model = glm::rotate(identityMat, glm::radians(angle),
                                rotationVec);
        }
    }

    void draw(
        glm::mat4 projection,
        glm::mat4 view
    ) {
        
        rcShader->use();
            // start with shader use?

        // int isRotationIsNotEmpty =
        //     cameraPositionState.x != 0.0f && cameraPositionState.y != 0.0f;
        // handleCameraPosition(isRotationIsNotEmpty, cameraPositionState);
        // handleSelection();


        // Calculate the rotation angle based on time
        // Create the model matrix with rotation
        float spread = SPREAD;

        int modelIdx = 0;
        glm::mat4 modelMatrix = defaultModelMatrix;
        glm::vec3 cubePosition = glm::vec3(0,0,0);  
                         // cubeIdToPosition[cube.cubeId];
                    
        rotationUpdate();
        //handleSideRotation();
        for (int i = 0; i < cubeSize; i++)
        {
            bool isInnerZ = i>0 && i< cubeSize-1;

            for (int j = 0; j < cubeSize; j++)
            {
                bool isInnerY = j>0 && j< cubeSize-1;

                for (int k = 0; k < cubeSize; k++)
                {
                    bool isInnerX = k>0 && k< cubeSize-1;
                    if (isInnerZ && isInnerY && isInnerX)
                        continue;

                    modelIdx = cubeSize * (cubeSize * i + j) + k;
                    // Cube cube = getModel(modelIdx);

                    glm::vec3 cubePosition = glm::vec3(
                        k - cubeShift, j - cubeShift, i - cubeShift);   // cubeIdToPosition[cube.cubeId];
                    

                    modelMatrix = defaultModelMatrix;

                    // testing rotation
                    // if (rotationConfig.active)
                    // {
                        if(isRotActive)
                           rotateObject(modelMatrix, cubePosition); //cubeIdToPosition[getModel(modelIdx).cubeId]);
                    // }
                    glm::vec3 cubePositionWithSpread = spread * cubePosition;
                    modelMatrix = glm::translate(modelMatrix, cubePositionWithSpread);

                    cubes[modelIdx].draw(
                        rcShader->ID,
                        RB_VAO,
                        projection,
                        view,
                        modelMatrix
                    );
                }
            }
        }
    }
};