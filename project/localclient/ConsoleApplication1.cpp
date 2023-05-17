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
#include "stb_image.h"

int main()
{
    GLWrapper wrap;
    wrap.initializeGL();
    wrap.setupVAO();

    Game game(&wrap);
    game.world.generate();
    std::function<void(Game* g)> surveyTask = [](Game* g) { g->surveyNeededChunks(); };
    std::function<void(Game* g)> chunkQueueTask = [](Game* g) { g->rebuildNextChunk(); };
    std::function<void(Game* g)> sortChunkPoolTask = [](Game* g) { g->sortChunkPool(); };
    game.addTask(surveyTask, 5.0f, 1);
    game.addTask(chunkQueueTask, 0.1, 2);
    game.addTask(sortChunkPoolTask, 7.0f, 3);

    /*TEXTURE BIT*/
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    /*END TEXTURE BIT*/


    float deltaTime = 0;
    float lastFrame = 0;
    game.surveyNeededChunks();
    
    const float friction = 0.75;

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
                    c.vbouv,
                    &(c.vertices[0]),
                    &(c.colors[0]),
                    &(c.uv[0]),
                    sizeof(GLfloat) * c.vertices.size(),
                    sizeof(GLfloat) * c.colors.size(),
                    sizeof(GLfloat) * c.uv.size());
                glDrawArrays(GL_TRIANGLES, 0, c.vertices.size());
            }
        }
        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        wrap.deltaTime = deltaTime;
        lastFrame = currentFrame;
        game.updateTasks(deltaTime*10);

        glfwSwapBuffers(wrap.window);


        if (wrap.activeState.forward)
        {
            wrap.activeState.forwardVelocity += deltaTime * 3;
        }
        if (wrap.activeState.forwardVelocity > 0)
        {

            wrap.cameraPos += (wrap.cameraDirection * wrap.activeState.forwardVelocity) * 0.75f;
            wrap.activeState.forwardVelocity *= friction;
        }
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}