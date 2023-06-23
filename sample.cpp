#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Vertex shader source code
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;

    out vec3 color;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        color = aColor;
    }
)";

// Fragment shader source code
const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 color;
    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(color, 1.0f);
    }
)";

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Box", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the current OpenGL context active
    glfwMakeContextCurrent(window);

    // Load OpenGL function pointers with glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Compile and link the vertex and fragment shaders
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

    // Clean up the shader objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Define the vertices and colors of the box
    float vertices[] = {
        // Position             // Color
        -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,   // Bottom left back
         0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,   // Bottom right back
         0.5f,  0.5f, -0.5f,    0.0f, 0.0f, 1.0f,   // Top right back
        -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0.0f,   // Top left back

        -0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,   // Bottom left front
         0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,   // Bottom right front
         0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,   // Top right front
        -0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 0.0f    // Top left front
    };

    GLuint indices[] = {
        // Back face
        0, 1, 2,
        2, 3, 0,

        // Front face
        4, 5, 6,
        6, 7, 4,

        // Left face
        7, 3, 0,
        0, 4, 7,

        // Right face
        1, 5, 6,
        6, 2, 1,

        // Top face
        7, 6, 2,
        2, 3, 7,

        // Bottom face
        0, 1, 5,
        5, 4, 0
    };

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

    // Create the projection matrix
    float aspectRatio = 800.0f / 600.0f;  // Adjust the aspect ratio if needed
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

    // Create the view matrix
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Main rendering loop
    while (!glfwWindowShouldClose(window))
    {
        // Clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use the shader program
        glUseProgram(shaderProgram);

        // Set the projection matrix uniform
        GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Set the view matrix uniform
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // Calculate the rotation angle based on time
        float angle = static_cast<float>(glfwGetTime()) * 25.0f;  // Adjust the multiplier to change the rotation speed

        // Create the model matrix with rotation
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.0f));

        // Set the model matrix uniform
        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Bind the VAO
        glBindVertexArray(VAO);

        // Draw the box
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Unbind the VAO
        glBindVertexArray(0);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up resources
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);

    // Terminate GLFW
    glfwTerminate();

    return 0;
}
