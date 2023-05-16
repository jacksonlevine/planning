#include <iostream>
#include <format>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GLSetup.h"
#include "Game.h"
#include <iterator>



int main()
{
    GLWrapper wrap;
    wrap.initializeGL();
    wrap.setupVAO();


    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f, 
         0.5f, -0.5f, 0.0f, 
         0.0f,  0.5f, 0.0f 
    };


    GLfloat colors[] = {
        1.0f, 0.0f, 0.0f, 
        0.0f, 1.0f, 0.0f, 
        0.0f, 0.0f, 1.0f  
    };

    Chunk c;
    

    wrap.bindGeometry(&vertices[0], &colors[0], sizeof(vertices), sizeof(colors));

    float deltaTime = 0;
    float lastFrame = 0;

    // Main loop
    while (!glfwWindowShouldClose(wrap.window))
    {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        wrap.runGLLoop();
   
    }

    glfwTerminate();

    return 0;
}