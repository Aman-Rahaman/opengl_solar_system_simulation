#ifndef IMP_VAR_HPP
#define IMP_VAR_HPP


#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "config.hpp"


// Window dimensions
unsigned int WIDTH = 800;
unsigned int HEIGHT = 600;


float cameraAngleX=0.0f, cameraAngleY=0.0f, cameraAngleZ=0.0f;
float cameraRadius = CAMERA_RADIUS;


GLuint shaderProgram;


glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, cameraRadius);
glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.0f);  // Light position
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // White light
// glm::vec3 lightColor = glm::vec3(1.5f, 1.5f, 1.5f);  // Increase brightness



float vertices[] = {
    // Positions (x, y, z)
    0.0f, 0.5f, 0.0f,    
    -0.5f, 0.0f, 0.5f,    
    0.5f, 0.0f, 0.5f,    

    0.0f, 0.5f, 0.0f,    
    0.5f, 0.0f, 0.5f,    
    0.5f, 0.0f, -0.5f,    

    0.0f, 0.5f, 0.0f,    
    0.5f, 0.0f, -0.5f,    
    -0.5f, 0.0f, -0.5f,    

    0.0f, 0.5f, 0.0f,    
    -0.5f, 0.0f, -0.5f,    
    -0.5f, 0.0f, 0.5f,    

    0.0f, -0.5f, 0.0f,    
    -0.5f, 0.0f, 0.5f,    
    0.5f, 0.0f, 0.5f,    

    0.0f, -0.5f, 0.0f,    
    0.5f, 0.0f, 0.5f,    
    0.5f, 0.0f, -0.5f,    

    0.0f, -0.5f, 0.0f,    
    0.5f, 0.0f, -0.5f,    
    -0.5f, 0.0f, -0.5f,    

    0.0f, -0.5f, 0.0f,    
    -0.5f, 0.0f, -0.5f,    
    -0.5f, 0.0f, 0.5f    
};




#endif