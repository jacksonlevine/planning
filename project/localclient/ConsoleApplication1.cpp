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

        // Draw stuff here using OpenGL calls
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


    // Generate a vertex array object (VAO)
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao); //AND PUSH IT TO GlWrapper.vaos

    // Generate a vertex buffer object (VBO) for the position data
    GLuint vbo_vertices;
    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set up the vertex attribute pointers for the position buffer object
    GLint pos_attrib = glGetAttribLocation(test.shaderProgram, "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Generate a vertex buffer object (VBO) for the color data
    GLuint vbo_colors;
    glGenBuffers(1, &vbo_colors);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    // Set up the vertex attribute pointers for the color buffer object
    GLint col_attrib = glGetAttribLocation(test.shaderProgram, "color");
    glEnableVertexAttribArray(col_attrib);
    glVertexAttribPointer(col_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);


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
    glDeleteBuffers(1, &vbo_vertices);
    glDeleteBuffers(1, &vbo_colors);
    glDeleteVertexArrays(1, &vao);
    glfwTerminate();

    return 0;
}