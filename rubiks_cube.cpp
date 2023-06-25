#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION

#include "draw.h"
#include "vertices.h"
#include <iostream>
#include "tools/shader_loader.h"
#include "other/mouse.h"

#define MOVE_ON_MOVE_OF_CURSOR 1
#define MOVE_ON_DRAG_OF_CURSOR 2

static int object_move = MOVE_ON_DRAG_OF_CURSOR;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void processMouseInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void updateRotation();

ModelRotation modelRotation = { 0.0f, 0.0f, 0.0f};
// rotation can can be performed (but not necessarily applied)
ModelRotation phantomModelRotation = {1.0f, 0.0f, 0.0f};
MouseState mouseState;
MouseState previousMouseState;

int main() {
// === OPEN GL INIT ===

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    int glfwError = glfwGetError(NULL);
    if (glfwError != GLFW_NO_ERROR) {
        const char* description;
        glfwGetError(&description);
        std::cerr << "GLFW Error: " << description << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    

// === SETUP DATA ===

      // Create and bind the Vertex Array Object (VAO)
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

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

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    Shader ourShader("res/shaders/box.vs", "res/shaders/box.fs");
    GLuint projectionLocation = glGetUniformLocation(ourShader.ID, "projectionMatrix");
    GLuint modelViewLocation = glGetUniformLocation(ourShader.ID, "modelViewMatrix");
    GLuint viewLocation = glGetUniformLocation(ourShader.ID, "viewMatrix");

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    while (!glfwWindowShouldClose(window))
    {        
        // input
        // -----
        processMouseInput(window);
        processInput(window);
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // apply shaders
        ourShader.use();
        
        // Rotation angle defined by how far cursor is located from center of the screen

        updateRotation();
        draw3_3by3boxes(ourShader.ID, VAO, modelRotation);
        printf("    a:%f, x:%f, y:%f \n", modelRotation.angle, modelRotation.x, modelRotation.y);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    } 
    glfwTerminate();
    return 0;
}

float mod(float a, float b){
    float c = a;
    while(c>b){
        c -= b;
    }
    return c;
}

float normalize_mouse_position(float number, int maxVal){
    //mouseState.posX/WINDOW_WIDTH - 1;   0..1
    return 2.0f*mouseState.posX/maxVal - 1;
}
MouseState lastMousePos;

void updateRotation(){
    // here origin of vector is center of screen
    if(object_move == MOVE_ON_MOVE_OF_CURSOR){
        
        lastMousePos.posX =  mouseState.posX;
        lastMousePos.posY =  mouseState.posY;

        // swapped in order to make direction of cursor match rotation of the cube
        modelRotation.y = normalize_mouse_position( mouseState.posX,WINDOW_WIDTH );
        modelRotation.x = normalize_mouse_position( mouseState.posY,WINDOW_HEIGHT );
        modelRotation.angle= mod(sqrt(
            pow(modelRotation.x,2) + pow(modelRotation.y, 2)
        )*360.0f, 360.0f);
        return;
    }
    
    // here origin of vector is start hold position
    if(object_move == MOVE_ON_DRAG_OF_CURSOR) {
        //printf("[f] a:%f, x:%f, y:%f \n",
        // phantomModelRotation.angle, phantomModelRotation.x, phantomModelRotation.y);
        if(!mouseState.is_lbm_hold) {
            lastMousePos.posX =  mouseState.posX;
            lastMousePos.posY =  mouseState.posY;
            // get last position of model add to the current mouse position
            phantomModelRotation.x = modelRotation.y + normalize_mouse_position( mouseState.posX,WINDOW_WIDTH );
            phantomModelRotation.y = modelRotation.x + normalize_mouse_position( mouseState.posY,WINDOW_HEIGHT );
            
            phantomModelRotation.angle = modelRotation.angle;
            return;
        }
        //printf("ph x: %f, y: %f\n", phantomModelRotation.x, phantomModelRotation.y);
        //printf("   x: %f, y: %f\n", mouseState.posX, mouseState.posY);
        double x,y;
        // phantomModelRotation = last_model_pos + last_mouse_pos
        // x = last_model_pos + last_mouse_pos - current_mouse_pos
        // where shifted mouse pos = last_mouse_pos - current_mouse_pos
        // x = last_pos + shifted_mouse_pos
        x = phantomModelRotation.x - normalize_mouse_position( mouseState.posX,WINDOW_WIDTH );
        y = phantomModelRotation.y - normalize_mouse_position( mouseState.posY,WINDOW_HEIGHT );
        // rotation is fucked up, because the orientation is changes depending from the last state
        if(x == 0 && y == 0)
            return;
        modelRotation.y = normalize_mouse_position(lastMousePos.posX - mouseState.posX,WINDOW_WIDTH );
        modelRotation.x = normalize_mouse_position(lastMousePos.posY - mouseState.posY,WINDOW_WIDTH );
        modelRotation.angle =mod(sqrt(
            pow( x,2) + pow(y , 2)
        )*360.0f, 360.0f);
        
        lastMousePos.posX =  mouseState.posX;
        lastMousePos.posY =  mouseState.posY;
        // last_model_pos = -1..1
        // last_mouse_pos = 0..1
    }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // note that click is single event
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        mouseState.is_lbm_hold = action == GLFW_PRESS;
        printf("Click Changed, isPress: %i!\n",  action == GLFW_PRESS);
    }
}

void processMouseInput(GLFWwindow *window)
{
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    if(mouseX < 0 || mouseY <0 || mouseX > WINDOW_WIDTH || mouseY> WINDOW_HEIGHT)
        return;
    previousMouseState = mouseState;
    mouseState.posX = mouseX;
    mouseState.posY = mouseY;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}