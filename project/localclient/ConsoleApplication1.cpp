#include <iostream>
#include <format>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GLSetup.hpp"
#include "Game.hpp"
#include <iterator>
#include "stb_image.h"
#include "BlockTypes.hpp"

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Set the minification filter to nearest neighbor
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Set the magnification filter to nearest neighbor
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("texture.png", &width, &height, &nrChannels, 0);
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
    glPatchParameteri(GL_PATCH_VERTICES, 0);
    glPatchParameteri(GL_PATCH_DEFAULT_OUTER_LEVEL, 1);
    glPatchParameteri(GL_PATCH_DEFAULT_INNER_LEVEL, 1);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    TextureFace skyFace = TextureFace(0,0);
    GLfloat skyVerts[108] = {
        // Front face
        -0.5f, -0.5f,  0.5f, // Bottom-left
         0.5f, -0.5f,  0.5f, // Bottom-right
         0.5f,  0.5f,  0.5f, // Top-right

          0.5f,  0.5f,  0.5f, // Top-right
        -0.5f,  0.5f,  0.5f, // Top-left
        -0.5f, -0.5f,  0.5f, // Bottom-left

        // Back face
         0.5f, -0.5f, -0.5f, // Bottom-left
        -0.5f, -0.5f, -0.5f, // Bottom-right
        -0.5f,  0.5f, -0.5f, // Top-right

        -0.5f,  0.5f, -0.5f, // Top-right
         0.5f,  0.5f, -0.5f, // Top-left
         0.5f, -0.5f, -0.5f, // Bottom-left

         // Left face
         -0.5f, -0.5f, -0.5f, // Bottom-back
         -0.5f, -0.5f,  0.5f, // Bottom-front
         -0.5f,  0.5f,  0.5f, // Top-front

         -0.5f,  0.5f,  0.5f, // Top-front
         -0.5f,  0.5f, -0.5f, // Top-back
         -0.5f, -0.5f, -0.5f, // Bottom-back

         // Right face
          0.5f, -0.5f,  0.5f, // Bottom-back
          0.5f, -0.5f, -0.5f, // Bottom-front
          0.5f,  0.5f, -0.5f, // Top-front

          0.5f,  0.5f, -0.5f, // Top-front
          0.5f,  0.5f,  0.5f, // Top-back
          0.5f, -0.5f,  0.5f, // Bottom-back

          // Top face
          -0.5f,  0.5f,  0.5f, // Front-left
           0.5f,  0.5f,  0.5f, // Front-right
           0.5f,  0.5f, -0.5f, // Back-right

            0.5f,  0.5f, -0.5f, // Back-right
          -0.5f,  0.5f, -0.5f, // Back-left
          -0.5f,  0.5f,  0.5f, // Front-left

          // Bottom face
          -0.5f, -0.5f, -0.5f, // Front-left
           0.5f, -0.5f, -0.5f, // Front-right
           0.5f, -0.5f,  0.5f, // Back-right
            0.5f, -0.5f,  0.5f, // Back-right
          -0.5f, -0.5f,  0.5f  // Back-left
          - 0.5f, -0.5f, -0.5f, // Front-left
    };
    GLfloat skyUVs[72] = {
        // Front face
        skyFace.bl.x, skyFace.bl.y, // Bottom-left
         skyFace.br.x, skyFace.br.y, // Bottom-right
         skyFace.tr.x, skyFace.tr.y, // Top-right

         skyFace.tr.x, skyFace.tr.y, // Top-right
        skyFace.tl.x, skyFace.tl.y, // Top-left
        skyFace.bl.x, skyFace.bl.y, // Bottom-left
        // Back face
        skyFace.bl.x, skyFace.bl.y, // Bottom-left
         skyFace.br.x, skyFace.br.y, // Bottom-right
         skyFace.tr.x, skyFace.tr.y, // Top-right

         skyFace.tr.x, skyFace.tr.y, // Top-right
        skyFace.tl.x, skyFace.tl.y, // Top-left
        skyFace.bl.x, skyFace.bl.y, // Bottom-left
        // Left face
        skyFace.bl.x, skyFace.bl.y, // Bottom-left
         skyFace.br.x, skyFace.br.y, // Bottom-right
         skyFace.tr.x, skyFace.tr.y, // Top-right

         skyFace.tr.x, skyFace.tr.y, // Top-right
        skyFace.tl.x, skyFace.tl.y, // Top-left
        skyFace.bl.x, skyFace.bl.y, // Bottom-left

        // Right face
         skyFace.bl.x, skyFace.bl.y, // Bottom-left
         skyFace.br.x, skyFace.br.y, // Bottom-right
         skyFace.tr.x, skyFace.tr.y, // Top-right

         skyFace.tr.x, skyFace.tr.y, // Top-right
        skyFace.tl.x, skyFace.tl.y, // Top-left
        skyFace.bl.x, skyFace.bl.y, // Bottom-left

         // Top face
         skyFace.bl.x, skyFace.bl.y, // Bottom-left
         skyFace.br.x, skyFace.br.y, // Bottom-right
         skyFace.tr.x, skyFace.tr.y, // Top-right

         skyFace.tr.x, skyFace.tr.y, // Top-right
        skyFace.tl.x, skyFace.tl.y, // Top-left
        skyFace.bl.x, skyFace.bl.y, // Bottom-left

         // Bottom face
         skyFace.bl.x, skyFace.bl.y, // Bottom-left
         skyFace.br.x, skyFace.br.y, // Bottom-right
         skyFace.tr.x, skyFace.tr.y, // Top-right

         skyFace.tr.x, skyFace.tr.y, // Top-right
        skyFace.tl.x, skyFace.tl.y, // Top-left
        skyFace.bl.x, skyFace.bl.y, // Bottom-left
    };
    GLfloat skyCols[108] = {
        // Front face
          0.0f, 0.0f, 0.5f, // Bottom-left
          0.0f, 0.0f, 0.5f, // Bottom-right
          0.5f, 0.5f, 1.0f, // Top-right

          0.5f, 0.5f, 1.0f, // Top-right
          0.5f, 0.5f, 1.0f, // Top-left
          0.0f, 0.0f, 0.5f, // Bottom-left

          // Back face
           0.0f, 0.0f, 0.5f, // Bottom-left
           0.0f, 0.0f, 0.5f, // Bottom-right
          0.5f, 0.5f, 1.0f, // Top-right

           0.5f, 0.5f, 1.0f, // Top-right
          0.5f, 0.5f, 1.0f, // Top-left
          0.0f, 0.0f, 0.5f, // Bottom-left

          // Left face
           0.0f, 0.0f, 0.5f, // Bottom-back
           0.0f, 0.0f, 0.5f, // Bottom-front
          0.5f, 0.5f, 1.0f, // Top-front

          0.5f, 0.5f, 1.0f, // Top-front
          0.5f, 0.5f, 1.0f, // Top-back
          0.0f, 0.0f, 0.5f, // Bottom-back

          // Right face
           0.0f, 0.0f, 0.5f, // Bottom-back
           0.0f, 0.0f, 0.5f, // Bottom-front
          0.5f, 0.5f, 1.0f, // Top-front

          0.5f, 0.5f, 1.0f, // Top-front
          0.5f, 0.5f, 1.0f, // Top-back
          0.0f, 0.0f, 0.5f, // Bottom-back

          // Top face
          0.5f, 0.5f, 1.0f, // Front-left
          0.5f, 0.5f, 1.0f, // Front-right
          0.5f, 0.5f, 1.0f, // Back-right

          0.5f, 0.5f, 1.0f, // Back-right
          0.5f, 0.5f, 1.0f, // Back-left
          0.5f, 0.5f, 1.0f, // Front-left

          // Bottom face
          0.0f, 0.0f, 0.5f, // Front-left
          0.0f, 0.0f, 0.5f, // Front-right
          0.0f, 0.0f, 0.5f, // Back-right

          0.0f, 0.0f, 0.5f, // Back-right
          0.0f, 0.0f, 0.5f,  // Back-left
          0.0f, 0.0f, 0.5f, // Front-left
    };
    GLuint skyVBOV;
    GLuint skyVBOC;
    GLuint skyVBOUV;

    glGenBuffers(1, &skyVBOV);
    glGenBuffers(1, &skyVBOC);
    glGenBuffers(1, &skyVBOUV);

    for (int i = 0; i < 108; i++)
    {
        skyVerts[i] *= 500;
    }
    wrap.bindGeometry(
        skyVBOV,
        skyVBOC,
        skyVBOUV,
        &skyVerts[0],
        &skyCols[0],
        &skyUVs[0],
        sizeof(GLfloat)* 108,
        sizeof(GLfloat)* 108,
        sizeof(GLfloat)* 72);

    while (!glfwWindowShouldClose(wrap.window))
    {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        wrap.orientCamera();

        //SKY BIT
        wrap.bindGeometryNoUpload(
            skyVBOV,
            skyVBOC,
            skyVBOUV);
        glDrawArrays(GL_TRIANGLES, 0, 72);
        //END SKY BIT

        for (auto& pair : game.activeChunks) {
            Chunk& c = pair.second;
            if (c.vertices.size() && c.colors.size() && c.uv.size()) {
                if (c.dirty || c.bufferDeleted)
                {
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
                    c.dirty = false;
                    c.bufferDeleted = false;
                }
                else if(c.vbov != 0 && c.vboc != 0 && c.vbouv != 0) {
                    wrap.bindGeometryNoUpload(
                        c.vbov,
                        c.vboc,
                        c.vbouv);
                }
               
                glDrawArrays(GL_TRIANGLES, 0, c.colors.size());
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