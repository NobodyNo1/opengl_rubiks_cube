#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "config.h"

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

glm::vec3 getVectorColor(
    int sideIdx,
    glm::vec3 sideOrder 
) {
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

   if(
        frontCheck 
        || backCheck 
        || topCheck 
        || bottomCheck 
        || rightCheck 
        || leftCheck 
    ) {
        return glm::vec3(sideColors[sideIdx*3], sideColors[sideIdx*3+1], sideColors[sideIdx*3+2]);
    }

   return blackColor;
}

void drawCubeOfRubiksCube(
    unsigned int shaderProgram,
    GLuint VAO,
    glm::mat4 projection,
    glm::mat4 view,
    glm::mat4 model,
    glm::vec3 sideOrder 
) {
    for(int i = 1; i <= 6; i++) {
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
        
        int sideIndex = i - 1;

        glm::vec3 sideColor = getVectorColor(sideIndex, sideOrder);
        glUniform3f(objectColorLoc, sideColor.x, sideColor.y, sideColor.z);

        // Bind the VAO
        glBindVertexArray(VAO);

        // Draw the box
        glDrawElements(GL_TRIANGLES, 6*i, GL_UNSIGNED_INT, 0);

        // Unbind the VAO
        glBindVertexArray(0);
    }
}


void draw3_3by3boxes(
    unsigned int shaderProgram,
    GLuint VAO,
    ModelRotation modelRotation
) {
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    // Create the view matrix
    // TODO: explanation
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 model = glm::mat4(1.0f);
    //float angle = static_cast<float>(glfwGetTime()) * 25.0f;  // Adjust the multiplier to change the rotation speed
    //float angle = 0.0f;

    // Scaling twice in order to fit in the screen
    glm::mat4 scaledAndRotatedModel = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    scaledAndRotatedModel = glm::rotate(scaledAndRotatedModel, glm::radians(modelRotation.angle), glm::vec3(modelRotation.x, modelRotation.y, 0.0f));
    
    // Calculate the rotation angle based on time
    // Create the model matrix with rotation

    float padding = 0.01f;
    float spread = 1.0f + padding;
    
    for(int i = 0; i < 3; i++) {
        float z = spread * (i-1);
        for(int j = 0; j < 3; j++) {
            float y = spread * (j-1);
            for(int k = 0; k<3; k++){
                // skipping center cube
                if(k==1 && k == j && j == i) continue;

                float x = spread*(k-1);
                glm::vec3 sidePosition = glm::vec3(x, y, z);
                if (k == 0)
                    model = glm::rotate(scaledAndRotatedModel, glm::radians((float)(glfwGetTime()* 25.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
                else if (k == 2)
                    model = glm::rotate(scaledAndRotatedModel, glm::radians((float)(glfwGetTime()* -25.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
                else
                    model = scaledAndRotatedModel;
                model = glm::translate(model, sidePosition);
                glm::vec3 sideOrder = glm::vec3(k-1, j-1, i-1);
                drawCubeOfRubiksCube(shaderProgram, VAO, projection, view, model, sideOrder);
            }
        }
    }
    
    //drawCube(shaderProgram, VAO, projection, view, model);
}