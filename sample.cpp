#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include "tools/shader_loader.h"


// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// Shader sources
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    
    out vec3 FragPos;
    out vec3 Normal;
    out vec3 LightPos;
    
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    
    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * aNormal;
        LightPos = vec3(inverse(view) * vec4(0.0, 0.0, 0.0, 1.0));  // Extract camera/view position from view matrix
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    
    in vec3 FragPos;
    in vec3 Normal;
    in vec3 LightPos;
    
    uniform vec3 objectColor;
    uniform vec3 lightColor;
    
    void main()
    {
        vec3 ambient = 0.1 * lightColor;
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(LightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
        vec3 result = (ambient + diffuse) * objectColor;
        FragColor = vec4(result, 1.0);
    }
)";
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
int main()
{
    // Initialize GLFW and create a window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Window", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // Set viewport size and register framebuffer size callback
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // Compile and link shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // Delete shaders as they're linked into the program and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // Create vertex array object (VAO), vertex buffer object (VBO), and element buffer object (EBO)
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    // Bind VAO, VBO, and EBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    
    // Copy vertex and index data to VBO and EBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Set attribute pointers for vertex positions and normals
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    // Unbind VAO, VBO, and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Main rendering loop
    while (!glfwWindowShouldClose(window))
    {
        // Process input
        processInput(window);
        
        // Clear the color and depth buffers
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Activate the shader program
        glUseProgram(shaderProgram);
        
        // Define transformation matrices (model, view, projection)
        glm::mat4 model = glm::mat4(1.0f);
        float angle = static_cast<float>(glfwGetTime()) * 25.0f;  // Adjust the multiplier to change the rotation speed

        // Calculate the rotation angle based on time
        // Create the model matrix with rotation
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.0f));

        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        
        // Pass transformation matrices to the shader program
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        
        GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        // Set object color and light color uniforms
        GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
        glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
        
        GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
        glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
        
        // Bind the VAO
        glBindVertexArray(VAO);
        
        // Draw the object
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        
        // Unbind the VAO
        glBindVertexArray(0);
        
        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Delete allocated resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    
    // Terminate GLFW
    glfwTerminate();
    
    return 0;
}

// Function to handle framebuffer resize events
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Function to process key input
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}
