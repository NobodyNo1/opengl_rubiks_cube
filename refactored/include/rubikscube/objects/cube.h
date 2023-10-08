#include <glm/glm.hpp>
#include "side.h"
#include <iostream>
class Cube {

    private:
        int cubeId;
        Side* sides;
        glm::vec3 position;

    public:
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



};