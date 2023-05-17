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



    Game game(&wrap);
    game.world.generate();
    std::function<void(Game* g)> surveyTask = [](Game* g) { g->surveyNeededChunks(); };
    std::function<void(Game* g)> chunkQueueTask = [](Game* g) { g->rebuildNextChunk(); };
    game.addTask(surveyTask, 3.0f, 1);
    game.addTask(chunkQueueTask, 0.1, 2);
    std::cout << (game.tasks.size());
    //Chunk c(&game);
    //c.rebuildMesh();

    //wrap.bindGeometry(
    //    &(c.vertices[0]), 
    //   &(c.colors[0]), 
    //    sizeof(GLfloat) * c.vertices.capacity(), 
    //    sizeof(GLfloat) * c.colors.capacity());

    float deltaTime = 0;
    float lastFrame = 0;

    // Main loop
    while (!glfwWindowShouldClose(wrap.window))
    {

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        wrap.orientCamera();


        for (const auto& pair : game.activeChunks) {
            const Chunk& c = pair.second;
            wrap.bindGeometry(
                &(c.vertices[0]),
                &(c.colors[0]),
                sizeof(GLfloat) * c.vertices.capacity(),
                sizeof(GLfloat) * c.colors.capacity());
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        game.updateTasks(deltaTime*10);
        //std::cout << deltaTime;




        glfwSwapBuffers(wrap.window);

        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}