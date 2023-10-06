#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION

float slider_vertices[] = {
    // positions          // colors
        0.5f, 0.5f, 0.0f,  
        0.5f, -0.5f, 0.0f, 
        -0.5f,  -0.5f, 0.0f,  
        -0.5f, 0.5f, 0.0f,  
};
unsigned int slider_indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};



struct Area{
    float leftX;
    float topY;
    float width;
    float height;
};

struct Drag
{
    float mouseX;
    float mouseY;
    float startMouseX;
    float startMouseY;
    float isActive;
};

struct Slider
{
    int current;
    int max;
    int min;
};

Area slider_collision_area = {0.0, 0.0, 0.0, 0.0};
Drag slider_drag_state = {0.0, 0.0, 0.0, 0.0,  0 };
GLuint sliderColorLocation;
GLuint sliderScreenResolutionLocation;
GLuint slideModelLocation;

void init_slider(
    GLuint VAO,
    unsigned int shaderProgram
) { 

    slider_collision_area = { 10.0, 10.0, 20.0, 100.0 }; 

    //glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create and bind the Vertex Buffer Object (VBO)
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(slider_vertices), slider_vertices, GL_STATIC_DRAW);

    // Create and bind the Element Buffer Object (EBO)
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(slider_indices), slider_indices, GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);

    sliderColorLocation = glGetUniformLocation(shaderProgram, "sliderColor");
    sliderScreenResolutionLocation = glGetUniformLocation(shaderProgram, "iResolution");
    slideModelLocation = glGetUniformLocation(shaderProgram, "slModel");
    
    // Unbind the VAO, VBO, and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void draw_slider(
    unsigned int shaderProgram,
    GLuint VAO,
    float window_width,
    float window_height
) {
    if(slider_drag_state.isActive){

    } else{
        // detect collision
        // if true start
    }

    float slider_width = 2.0f;
    float slider_height = 0.05f;
    float sliderX =  -0.25f;
    float sliderY = 0.75f;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(sliderX, sliderY, 0.0f));
    model = glm::scale(model, glm::vec3(slider_width, slider_height, 1.0f));
    glUniform3f(sliderColorLocation, 1.0f, 0.5f, 0.5f);
    glUniform2f(sliderScreenResolutionLocation,  window_width, window_height);
    glUniformMatrix4fv(slideModelLocation, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, 6 , GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}