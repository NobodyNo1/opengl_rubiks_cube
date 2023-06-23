#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION

#include "vertices.h"
#include <iostream>
#include "tools/shader_loader.h"


#define WINDOW_WIDTH    1280
#define WINDOW_HEIGHT   720

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

float aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

float modF(float value, float mod) {
    int result = value;
    while(result > mod){
        result-=mod;
    }
    return result;
}
float sideColors[] = {
    1.0f, 0.0f, 0.0f, // Front face (red)
    0.0f, 0.0f, 1.0f, // Right face (blue)
    1.0f, 0.5f, 0.0f, // Back face (orange) ??
    0.0f, 1.0f, 0.0f, // Left face (green)
    1.0f, 1.0f, 0.0f, // Bottom face (yellow)
    1.0f, 1.0f, 1.0f  // Top face (white)
};

void drawCube(
    unsigned int shaderProgram,
    GLuint VAO,
    glm::mat4 projection,
    glm::mat4 view,
    glm::mat4 model,
    glm::vec3 color
) {
    for(int i = 1; i <= 6; i++){
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
        int arrIdx = i - 1;
        glUniform3f(objectColorLoc, sideColors[0+arrIdx*3], sideColors[1+arrIdx*3], sideColors[2+arrIdx*3]);

        // Bind the VAO
        glBindVertexArray(VAO);

        // Draw the box
        glDrawElements(GL_TRIANGLES, 6*i, GL_UNSIGNED_INT, 0);

        // Unbind the VAO
        glBindVertexArray(0);
    }
}

void drawCube(
    unsigned int shaderProgram,
    GLuint VAO,
    glm::mat4 projection,
    glm::mat4 view,
    glm::mat4 model
) {
    glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f); 
    drawCube(shaderProgram, VAO, projection, view, model, color);
}

void drawBox(unsigned int shaderProgram, GLuint VAO) {
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    // Create the view matrix
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 model = glm::mat4(1.0f);
    float angle = static_cast<float>(glfwGetTime()) * 25.0f;  // Adjust the multiplier to change the rotation speed

    // Calculate the rotation angle based on time
    // Create the model matrix with rotation
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.0f));

    drawCube(shaderProgram, VAO, projection, view, model);
}

void draw3_3by3boxes(unsigned int shaderProgram, GLuint VAO) {
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    // Create the view matrix
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 model = glm::mat4(1.0f);
    float angle = static_cast<float>(glfwGetTime()) * 25.0f;  // Adjust the multiplier to change the rotation speed
    //float angle = 0.0f;

    glm::mat4 scaledAndRotatedModel = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    scaledAndRotatedModel = glm::rotate(scaledAndRotatedModel, glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.0f));
    // Calculate the rotation angle based on time
    // Create the model matrix with rotation

    float padding = 0.01f;
    float spread = 1.0f + padding;
    
    for(int i = 0; i < 3; i++) {
        float z = spread * (i-1);
        for(int j = 0; j < 3; j++) {
            float y = spread * (j-1);
            for(int k = 0; k<3; k++){
                if(k==1 &&k == j && j == i) continue;
                float x = spread*(k-1);

                model = glm::translate(scaledAndRotatedModel, glm::vec3(x, y, z));
                drawCube(shaderProgram, VAO, projection, view, model);
            }
        }
    }
    

    //drawCube(shaderProgram, VAO, projection, view, model);
}

int main() {
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


    //glm::mat4 projectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

     

     while (!glfwWindowShouldClose(window))
    {        

        // input
        // -----
        processInput(window);
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //
        ourShader.use();
        draw3_3by3boxes(ourShader.ID, VAO);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    } 
    glfwTerminate();
    return 0;
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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