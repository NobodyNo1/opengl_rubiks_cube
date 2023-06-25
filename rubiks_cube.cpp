#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION

#include "draw.h"
#include <iostream>
#include "tools/shader_loader.h"
#include "other/mouse.h"


static int object_move = MOVE_ON_DRAG_OF_CURSOR;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void processMouseInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void updateRotation();
void processActions();

CameraRotation cameraRotation = { 0.0f, 0.0f, 0.0f, object_move};
DragAction dragAction = { 0.0f, 0.0f, 0.0f, 0.0f, 0 };
// rotation can can be performed (but not necessarily applied)
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
        processActions();
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // apply shaders
        ourShader.use();
        
        // Rotation angle defined by how far cursor is located from center of the screen

        updateRotation();
        draw3_3by3boxes(ourShader.ID, VAO, cameraRotation, dragAction);
        //printf("    a:%f, x:%f, y:%f \n", cameraRotation.angle, cameraRotation.x, cameraRotation.y);
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
    return 2.0f*(number/maxVal) - 1.0f;
}

float flip_and_normalize_mouse_position(float number, int maxVal){
    //mouseState.posX/WINDOW_WIDTH - 1;   0..1
    return 2.0f*((maxVal-number)/maxVal) - 1.0f;
}



void updateRotation(){
    // here origin of vector is center of screen
    if(object_move == MOVE_ON_MOVE_OF_CURSOR){
        // swapped in order to make direction of cursor match rotation of the cube
        cameraRotation.y = normalize_mouse_position( mouseState.posX,WINDOW_WIDTH );
        cameraRotation.x = -flip_and_normalize_mouse_position( mouseState.posY,WINDOW_HEIGHT );
        cameraRotation.angle= mod(sqrt(
            pow(cameraRotation.x,2) + pow(cameraRotation.y, 2)
        )*360.0f, 360.0f);
        return;
    }
    
    // here origin of vector is start hold position
    if(object_move == MOVE_ON_DRAG_OF_CURSOR) {
        if(!mouseState.is_rmb_hold) {
            cameraRotation.x = 0.0;
            cameraRotation.y = 0.0;
            // get last position of model add to the current mouse position
            previousMouseState.posX = mouseState.posX;
            previousMouseState.posX = mouseState.posY;
            return;
        }
        float xoffset = previousMouseState.posX - mouseState.posX ;
        float yoffset = previousMouseState.posY - mouseState.posY; // Reversed since y-coordinates range from bottom to top
        float sensitivity = 0.3f; // Adjust this value to control mouse movement speed
        xoffset *= sensitivity;
        yoffset *= sensitivity;
        previousMouseState.posX = mouseState.posX ;
        previousMouseState.posX = mouseState.posY;

        cameraRotation.y =  yoffset;
        cameraRotation.x =  xoffset;
    }
}

void processActions() {
    if(object_move != MOVE_ON_DRAG_OF_CURSOR)
        return;
    if(!dragAction.isActive && mouseState.is_lmb_hold){
        dragAction.startX = mouseState.posX;
        dragAction.startY = mouseState.posY;
    } else if(dragAction.isActive && !mouseState.is_lmb_hold){
        dragAction.startX = 0.0f;
        dragAction.startY = 0.0f;
        dragAction.x = 0.0f;
        dragAction.y = 0.0f;
    } else {
        dragAction.x = mouseState.posX;
        dragAction.y = mouseState.posY;
    }
    dragAction.isActive = mouseState.is_lmb_hold;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // note that click is single event
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        mouseState.is_rmb_hold = action == GLFW_PRESS;
        printf("Click Changed, isPress: %i!\n",  action == GLFW_PRESS);
    } else if(button == GLFW_MOUSE_BUTTON_LEFT){
        mouseState.is_lmb_hold = action == GLFW_PRESS;
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