#include <glm/glm.hpp>
#include "side.hpp"
#include <iostream>
#include "object_config.hpp"

class Cube {

    private:
        int cubeId;
        Side* sides;

    public:
        glm::vec3 position;

        Cube(){

        }

        Cube(int id, glm::vec3 pos) {
            cubeId = id;
            position = pos;
            sides = new Side[6];
            for (int sideIndex = 0; sideIndex < 6; sideIndex++)
            {
                sides[sideIndex] = Side(sideIndex, position);
            }
        }

        void update() {
            for (int i = 0; i<6; i++){
                sides[i].update();
            }
        }

        void updatePosition(glm::vec3 pos) {
            position = pos;
        }

        void draw(
            unsigned int shaderProgram,
            GLuint VAO,
            glm::mat4 projection,
            glm::mat4 view,
            glm::mat4 model
        ) {

            bool collision_found = false;
            for (int i = 0; i<6; i++) {
                sides[i].draw(
                    cubeId,
                    position,
                    collision_found,
                    shaderProgram,
                    VAO,
                    projection,
                    view,
                    model
                );
            }
        }

        void rotate(bool isPositiveAxis, bool isClockwise, int axis) {
            bool direction = isPositiveAxis && isClockwise;
            if(direction){
                rotateCube(CUBE_SIZE, axis);
                //clockwise rotation
            } else{
                unRotateCube(CUBE_SIZE, axis);
                //anti clockwise
            }
            //rotateSideIdx(axis, direction, sides);
        }
       
        
private:
    void rotateCube(int cubeSize, int axis) {
        // z  x y
        // y z x
        // x z y
        int k = cubeSize - 1;
        // float x2 = y1;
        // float y2 = k - x1 - 2*CUBE_SHIFT;
        //rotate by x
        switch (axis)
        {
        case 0: {//by x
            float a = position[1];
            float b = k - position[2] - 2*CUBE_SHIFT;
            position[2] = a; 
            position[1] = b; 
            /* code */
            break;
        }
        case 1: {// by y
            float a = position[0];
            float b = k - position[2] - 2*CUBE_SHIFT;
            position[2] = a; 
            position[0] = b; 
            
            break;
        }
        case 2: {//by z
            float a = position[1];
            float b = k - position[0] - 2*CUBE_SHIFT;
            position[0] = a; 
            position[1] = b; 
            break; 
        }
        default:
            break;
        }
        // TODO: rotate sides
    }

    void unRotateCube(int cubeSize, int axis) {
        // z  x y
        // y z x
        // x z y
        int k = cubeSize - 1;
        // float x2 = y1;
        // float y2 = k - x1 - 2*CUBE_SHIFT;
        //rotate by x
        switch (axis)
        {
        case 0:{ //by x
            float a = position[2];
            float b = k - position[1] - 2*CUBE_SHIFT;
            position[1] = b; 
            position[2] = a; 
            /* code */
            break;
        }
        case 1: {// by y
            float a = position[2];
            float b = k - position[0] - 2*CUBE_SHIFT;
            position[0] = a; 
            position[2] = b; 
            
            break;
             }
        case 2:{ //by z
            float a = position[0];
            float b = k - position[1] - 2*CUBE_SHIFT;
            position[1] = a; 
            position[0] = b; 
            break; 
        }
        default:
            break;
        }
        // TODO: rotate sides
    }
};