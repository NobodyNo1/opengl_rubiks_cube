#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION

#include "draw.h"
#include <iostream>
#include "tools/shader_loader.h"
#include "tools/path_helper.h"
#include "ui/slider.h"
#include "cube_init.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);


int start() {
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

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Load shaders
    char* vertex_shader_path = get_path("src/res/shaders/box.vs");
    char* fragment_shader_path = get_path("src/res/shaders/box.fs");

    Shader ourShader(vertex_shader_path, fragment_shader_path);

    char* slider_vertex_shader_path = get_path("src/res/shaders/slider.vs");
    char* slider_fragment_shader_path = get_path("src/res/shaders/slider.fs");

    Shader slider_shader(slider_vertex_shader_path, slider_fragment_shader_path);
    
    // Create and bind the Vertex Array Object (VAO)
    GLuint VAO[2];
    glGenVertexArrays(2, VAO);
    cube_init(VAO[0]);
    init_slider(VAO[1], slider_shader.ID);

    //initiallizing cube
    init3_3boxes();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    while (!glfwWindowShouldClose(window))
    {        
        // input
        // -----
        processMouseInput(window, WINDOW_WIDTH, WINDOW_HEIGHT);
        processKeyboardInput(window);
        //Handle Actions
        handleActions(WINDOW_WIDTH, WINDOW_HEIGHT);
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // apply shaders
        ourShader.use();
        
        // Rotation angle defined by how far cursor is located from center of the screen
        draw3_3by3boxes(ourShader.ID, VAO[0], cameraPositionState, dragAction);
        slider_shader.use();
        draw_slider(slider_shader.ID, VAO[1], WINDOW_WIDTH, WINDOW_HEIGHT);
        // printf("    a:%f, x:%f, y:%f \n", cameraRotation.angle,
        // cameraRotation.x, cameraRotation.y);
        //  glfw: swap buffers and poll IO events (keys pressed/released, mouse
        //  moved etc.)
        //  -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    } 
    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    WINDOW_HEIGHT = height;
    WINDOW_WIDTH = width;
}