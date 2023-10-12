#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "object_config.hpp"

#define BOTTOM_IDX   0
#define TOP_IDX      1
#define LEFT_IDX     2
#define RIGHT_IDX    3
#define BACK_IDX     4
#define FRONT_IDX    5

#define TRANSPARENCY 0
#define FIX_COLOR_CUBE 0
#define FIXED_COLORED_CUBE_ID 25


glm::vec3 blackColor      = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 whiteColor      = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 magnetaColor    = glm::vec3(1.0000f, 0.0000f, 1.0000f);


float sideColors[] = {
    1.0f, 1.0f, 0.0f,   // Bottom face (yellow)
    1.0f, 1.0f, 1.0f,   // Top face (white)
    0.0f, 1.0f, 0.0f,   // Left face (green)
    0.0f, 0.0f, 1.0f,   // Right face (blue)
    1.0f, 0.5f, 0.0f,   // Back face (orange) ??
    1.0f, 0.0f, 0.0f    // Front face (red)
};

class Side{
    private:
        int sideIdx;
        glm::vec3 color;   // color does not change because of that we can keep its
        float currentTime = 0.0f;

    public:
        Side() { }
        Side(int id, glm::vec3 cubePosition) {
            sideIdx = id;
            color = getCubeSideColor(cubePosition);
        }
        int getSideIdx(){
            return sideIdx;
        }
        void updateSideIdx(int idx){
            sideIdx = idx;
        }
        glm::vec3  getColor(){
            return color;
        }
        void updateColor(glm::vec3 col){
            color = col;
        }

        void update() {

            return;
        }

        void draw(
            int cubeId,
            glm::vec3 cubePosition,
            bool collision_found,
            unsigned int shaderProgram,
            GLuint VAO,
            glm::mat4 projection,
            glm::mat4 view,
            glm::mat4 model
        ) {

            // for (int sideIndex = 0; sideIndex < 6; sideIndex++)
            // {
                glm::vec3 sideColor = blackColor;

                // in order to test model rotation
                // getModel(25).cubeId
        #if FIX_COLOR_CUBE
                if (cubeId == 25)
                    sideColor = magnetaColor;
        #endif
                if (isOuterSide(cubePosition))
                {
        #if FIX_COLOR_CUBE
                    if (cubeId != 25)
                        sideColor = color;
        #else
                    sideColor = color;
        #endif
                
                    // if (collision_found && collisionWith.modelIdx == cubeId
                    //         && collisionWith.selectedSideId == sideIndex
                    // )
                    // {
                    //     sideColor = whiteColor;
                    // }
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

                GLuint timeLoc = glGetUniformLocation(shaderProgram, "time");
                float newTime = glfwGetTime();
                float deltaTime = newTime - currentTime;
                currentTime = newTime;
                glUniform1f(timeLoc, currentTime);
                
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
                glDrawElements(GL_TRIANGLES, 6 * (sideIdx + 1), GL_UNSIGNED_INT, 0);

                // Unbind the VAO
                glBindVertexArray(0);
            // }
        }

        int isOuterSide(glm::vec3 cubePosition)
        {
            float x, y, z;
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
            int frontCheck  =    sideIdx  == FRONT_IDX   &&   z ==  CUBE_SIZE - CUBE_SHIFT - 1;
            int backCheck   =    sideIdx  == BACK_IDX    &&   z ==  0 - CUBE_SHIFT;
            int topCheck    =    sideIdx  == TOP_IDX     &&   y ==  CUBE_SIZE - CUBE_SHIFT - 1;
            int bottomCheck =    sideIdx  == BOTTOM_IDX  &&   y ==  0 - CUBE_SHIFT;
            int rightCheck  =    sideIdx  == RIGHT_IDX   &&   x ==  CUBE_SIZE - CUBE_SHIFT - 1;
            int leftCheck   =    sideIdx  == LEFT_IDX    &&   x ==  0 - CUBE_SHIFT;

            return  backCheck || bottomCheck || leftCheck || frontCheck || topCheck || rightCheck;
        }

        glm::vec3 getCubeSideColor(glm::vec3 cubePosition)
        {
            if (isOuterSide(cubePosition))
            {
                return glm::vec3(sideColors[sideIdx * 3], sideColors[sideIdx * 3 + 1],
                                sideColors[sideIdx * 3 + 2]);
            }

            return blackColor;
        }
};