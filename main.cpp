#include "../glad/include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#include <vector>

#include "./shaders.hpp"
#include "./important_variables.hpp"
#include "./sphere.hpp"
#include "./config.hpp"


using namespace std;


// Define static members (required for linking)
float Sphere::earth_rev_angle;
float Sphere::sun_rev_angle;
float Sphere::moon_rev_angle;
float Sphere::space_rev_angle;

float Sphere::earth_rot_angle;
float Sphere::sun_rot_angle;
float Sphere::moon_rot_angle;
float Sphere::space_rot_angle;

MotionType Sphere::motionType;

glm::vec3 Sphere::earth_P0;
glm::vec3 Sphere::earth_P1;
glm::vec3 Sphere::earth_P2;
glm::vec3 Sphere::earth_P3;

glm::vec3 Sphere::moon_P0;
glm::vec3 Sphere::moon_P1;
glm::vec3 Sphere::moon_P2;
glm::vec3 Sphere::moon_P3;


void Sphere::initialize_config_variables(){
    // Initialize static members
    Sphere::earth_rev_angle = 0.0f;
    Sphere::sun_rev_angle   = 0.0f;
    Sphere::moon_rev_angle  = 0.0f;
    Sphere::space_rev_angle = 0.0f;

    Sphere::earth_rot_angle = 0.0f;
    Sphere::sun_rot_angle   = 0.0f;
    Sphere::moon_rot_angle  = 0.0f;
    Sphere::space_rot_angle = 0.0f;

    Sphere::motionType = ORBIT;
}




// Callback function to handle key input
void processInput(GLFWwindow* window) {
    const float cameraSpeed = 0.05f;
    float angle = glm::radians(1.0f);

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraAngleY -= cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraAngleY += cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraAngleX -= cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraAngleX += cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            cameraAngleZ += cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            cameraAngleZ -= cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            cameraRadius -= 0.1f;
        }
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
            cameraRadius += 0.1f;
        }
        
}



bool pauseSimulation = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        if(pauseSimulation)
            pauseSimulation = false;
        else
            pauseSimulation = true;
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        cameraAngleZ = 0.0f;
        cameraAngleX = 0.0f;
        cameraAngleY = 0.0f;
        cameraRadius = CAMERA_RADIUS;
    }
    else if (key == GLFW_KEY_G && action == GLFW_PRESS) {
        Sphere::set_spline();
    }

}



void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    
    WIDTH = width;
    HEIGHT = height;
}




int main() {

    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE); 

    // Create a window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Per Pixel Shading", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Load OpenGL using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Register the key callback function
    glfwSetKeyCallback(window, key_callback);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    // Build and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    checkShaderCompilation(vertexShader, "VERTEX");

    // Build and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    checkShaderCompilation(fragmentShader, "FRAGMENT");

    // Link shaders to a program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    Sphere::initialize_config_variables();

    Sphere earth(1.0f, EARTH, 5);
    Sphere sun(5.0f, SUN, 5);  
    Sphere moon(0.25f, MOON, 5);
    Sphere space(50.0f, SPACE, 3); // for starry sky
    

    glUseProgram(shaderProgram);    // Use the shader program

    while (!glfwWindowShouldClose(window)) {

        // Input processing
        processInput(window);

        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(!pauseSimulation)
            Sphere::update_angles();

        glDisable(GL_DEPTH_TEST);  // Disable depth testing so background always renders first
        space.render();
        glEnable(GL_DEPTH_TEST);   // Enable it again for normal rendering
        
        sun.render();
        earth.render();
        moon.render();
        
        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
        // glfwSwapInterval(1);
    }

    // Clean up
    moon.clean();
    earth.clean();
    sun.clean();
    space.clean();
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
