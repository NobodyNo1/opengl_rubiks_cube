#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include "config.hpp"
// #include "draw.h"
// 
// #include "tools/shader_loader.h"
// #include "tools/path_helper.h"
// #include "ui/slider.h"
// #include "cube_init.h"
#include <tools/logger.hpp>
#include <tools/shader_loader.hpp>
#include <objects/view.hpp>
#include <handler/action.hpp>

#define SUCCESS_EXIT     0
#define FAIL_EXIT       -1


void framebuffer_size_callback(GLFWwindow* window, int width, int height);

GLFWwindow* setupGLFW() {
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
        log(LOG_ERROR, "Failed to create GLFW window");     
        return nullptr;
    }
    if (glfwGetError(NULL) != GLFW_NO_ERROR) {
        const char* description;
        glfwGetError(&description);
        string error_message = "GLFW Error";
        if(description != NULL){
            string description_string = description;
            error_message = error_message + ": " + description_string;
        }

        log(LOG_ERROR, error_message);     
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        log(LOG_ERROR, "Failed to initialize GLAD");     
        return nullptr;
    }
    

    // === SETUP DATA ===    

    // Enable depth testing
    return window;
}
ActionHandler *actionHandler;
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
   actionHandler->mouseHandler->mouse_button_callback(window, button, action, mods);
}

int start() {
    GLFWwindow* window = setupGLFW();
    if (window == nullptr) {
        return EXIT_FAILURE;
    }
    View *view = new View();
    //init_slider(VAO[1], slider_shader.ID);
   
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    actionHandler = new ActionHandler(window);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    while (!glfwWindowShouldClose(window))
    {        
        actionHandler->handleActions(window, WINDOW_WIDTH, WINDOW_HEIGHT);

        view->update(actionHandler->dragAction);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // apply shaders
        // ourShader.use();
        
        // // Rotation angle defined by how far cursor is located from center of the screen
        view->draw();
        // slider_shader.use();
        // draw_slider(slider_shader.ID, VAO[1], WINDOW_WIDTH, WINDOW_HEIGHT);
        // printf("    a:%f, x:%f, y:%f \n", cameraRotation.angle,
        // cameraRotation.x, cameraRotation.y);
        //  glfw: swap buffers and poll IO events (keys pressed/released, mouse
        //  moved etc.)
        //  -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    } 
end:
    //memory clean
    glfwTerminate();
    return EXIT_SUCCESS;
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