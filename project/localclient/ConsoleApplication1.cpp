#include <iostream>
#include <format>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <folly/Optional.h>
#include <folly/container/F14Map.h>
#include "GLSetup.h"
class World {
public: 
    folly::F14FastMap<std::tuple<int>, uint8_t> data;
    void generate();
    void generateOneChunk();
};

void World::generate() {

}

void World::generateOneChunk() {

}



int main()
{
    GLWrapper test;
    test.initializeGL();
    test.setupVAO();

    // Define the vertex data of a triangle
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f, // bottom-left
         0.5f, -0.5f, 0.0f, // bottom-right
         0.0f,  0.5f, 0.0f  // top-center
    };

    // Define the color data of the vertices
    GLfloat colors[] = {
        1.0f, 0.0f, 0.0f, // bottom-left
        0.0f, 1.0f, 0.0f, // bottom-right
        0.0f, 0.0f, 1.0f  // top-center
    };

    test.bindGeometry(&vertices[0], &colors[0], sizeof(vertices), sizeof(colors));

    float deltaTime = 0;
    float lastFrame = 0;
    // Main loop
    while (!glfwWindowShouldClose(test.window))
    {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        test.runGLLoop();
   
    }

    // Clean up
    //glDeleteBuffers(1, &vbo_vertices);
    //glDeleteBuffers(1, &vbo_colors);
    //glDeleteVertexArrays(1, &vao);
    glfwTerminate();

    return 0;
}