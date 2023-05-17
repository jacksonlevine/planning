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
    //Chunk c(&game);
    //c.rebuildMesh();

    //wrap.bindGeometry(
    //    &(c.vertices[0]), 
    //   &(c.colors[0]), 
    //    sizeof(GLfloat) * c.vertices.capacity(), 
    //    sizeof(GLfloat) * c.colors.capacity());

    float deltaTime = 0;
    float lastFrame = 0;
    game.surveyNeededChunks();
    

    const float friction = 0.75;
    // Main loop
    while (!glfwWindowShouldClose(wrap.window))
    {

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        wrap.orientCamera();

        for (const auto& pair : game.activeChunks) {
            const Chunk& c = pair.second;
            if (c.vertices.size() && c.colors.size()) {
                wrap.bindGeometry(
                    c.vbov,
                    c.vboc,
                    &(c.vertices[0]),
                    &(c.colors[0]),
                    sizeof(GLfloat) * c.vertices.size(),
                    sizeof(GLfloat) * c.colors.size());
                glDrawArrays(GL_TRIANGLES, 0, c.vertices.size());
            }
        }
        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        wrap.deltaTime = deltaTime;
        lastFrame = currentFrame;
        game.updateTasks(deltaTime*10);
        game.rebuildNextChunk();
        //std::cout << deltaTime;




        glfwSwapBuffers(wrap.window);


        if (wrap.activeState.forward)
        {
            wrap.activeState.forwardVelocity += deltaTime * 6;
        }
        if (wrap.activeState.forwardVelocity > 0)
        {

            wrap.cameraPos += wrap.cameraDirection * wrap.activeState.forwardVelocity;
            wrap.activeState.forwardVelocity *= friction;
        }
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}