#pragma once
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
#include "entt/entt.hpp"
#include "Scene.hpp"
void mygl_GradientBackground(float top_r, float top_g, float top_b, float top_a,
    float bot_r, float bot_g, float bot_b, float bot_a, float cameraPitch)
{
    glDisable(GL_DEPTH_TEST);

    static GLuint background_vao = 0;
    static GLuint background_shader = 0;

    if (background_vao == 0)
    {
        glGenVertexArrays(1, &background_vao);

        const GLchar* vs_src =
            "#version 450 core\n"
            "out vec2 v_uv;\n"
            "uniform float cpitch;\n"
            "void main()\n"
            " {\n"
            " uint idx = gl_VertexID;\n"

            " gl_Position = vec4(idx & 1, (idx >> 1), 0.0, 0.5) * 4.0 - 1.0;\n"
            "v_uv = vec2(gl_Position.xy * 0.5 + 0.5+(cpitch/100));\n"
            "}";


        const GLchar* fs_src =
            " #version 450 core\n"
            "uniform vec4 top_color;\n"
            "uniform vec4 bot_color;\n"
            "in vec2 v_uv;\n"
            "out vec4 frag_color;\n"

            "void main()\n"
            "{\n"
            "frag_color = bot_color * (1 - v_uv.y) + top_color * v_uv.y;\n"
            "}";

        GLuint vs_id, fs_id;
        vs_id = glCreateShader(GL_VERTEX_SHADER);
        fs_id = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(vs_id, 1, &vs_src, NULL);
        glShaderSource(fs_id, 1, &fs_src, NULL);
        glCompileShader(vs_id);

        GLint success;
        GLchar infoLog[512];
        glGetShaderiv(vs_id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vs_id, 512, NULL, infoLog);
            std::cerr << "Vertex shader compilation error: " << infoLog << std::endl;
        }

        glCompileShader(fs_id);


        glGetShaderiv(fs_id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fs_id, 512, NULL, infoLog);
            std::cerr << "Fragment shader compilation error: " << infoLog << std::endl;
        }

        background_shader = glCreateProgram();
        glAttachShader(background_shader, vs_id);
        glAttachShader(background_shader, fs_id);
        glLinkProgram(background_shader);
        glDetachShader(background_shader, fs_id);
        glDetachShader(background_shader, vs_id);
        glDeleteShader(fs_id);
        glDeleteShader(vs_id);
    }

    glUseProgram(background_shader);
    GLuint top_color_loc = glGetUniformLocation(background_shader, "top_color");
    GLuint bot_color_loc = glGetUniformLocation(background_shader, "bot_color");
    glUniform4f(top_color_loc, top_r, top_g, top_b, top_a);
    glUniform4f(bot_color_loc, bot_r, bot_g, bot_b, bot_a);
    GLuint cpitch_loc = glGetUniformLocation(background_shader, "cpitch");

    glUniform1f(cpitch_loc, cameraPitch);

    glBindVertexArray(background_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}

void waterTile(
    float bot_r, float bot_g, float bot_b, float bot_a, float wheight, glm::mat4 mvp, glm::vec3 camPos)
{
    //glDisable(GL_DEPTH_TEST);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    static GLuint water_vao = 0;
    static GLuint water_shader = 0;

    if (water_vao == 0)
    {
        glGenVertexArrays(1, &water_vao);

        const GLchar* vs_src =
            "#version 450 core\n"
            "out vec2 v_uv;\n"
            "uniform float wheight;\n"
            "uniform mat4 mvp;\n"
            "uniform vec2 camPos;\n"
            "void main()\n"
            " {\n"
            " uint idx = gl_VertexID;\n"

            " gl_Position = mvp * vec4(  ((camPos.x-2500) + (idx & 1)*5000), wheight, ((camPos.y - 2500) + (idx >> 1) * 5000), 1.0  ) * 5000.0 - 1.0;\n"
            "gl_Position *= 1000.0f;\n"
            "v_uv = vec2(gl_Position.xy * 0.5 + 0.5);\n"
            "}";


        const GLchar* fs_src =
            " #version 450 core\n"
            "uniform float time;\n"
            "uniform vec4 base_color;\n"
            "in vec2 v_uv;\n"
            "out vec4 frag_color;\n"

           " float rand(vec2 co) {\n"
           " return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);\n"
            " }\n"

            "void main()\n"
            "{\n"
            "frag_color = vec4(1.0,1.0,1.0,1.0) * rand(vec2((v_uv.x + time)/100000000, (v_uv.y + time)/100000000));\n"
            "}";

        GLuint vs_id, fs_id;
        vs_id = glCreateShader(GL_VERTEX_SHADER);
        fs_id = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(vs_id, 1, &vs_src, NULL);
        glShaderSource(fs_id, 1, &fs_src, NULL);
        glCompileShader(vs_id);

        GLint success;
        GLchar infoLog[512];
        glGetShaderiv(vs_id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vs_id, 512, NULL, infoLog);
            std::cerr << "Vertex shader compilation error: " << infoLog << std::endl;
        }

        glCompileShader(fs_id);


        glGetShaderiv(fs_id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fs_id, 512, NULL, infoLog);
            std::cerr << "Fragment shader compilation error: " << infoLog << std::endl;
        }

        water_shader = glCreateProgram();
        glAttachShader(water_shader, vs_id);
        glAttachShader(water_shader, fs_id);
        glLinkProgram(water_shader);
        glDetachShader(water_shader, fs_id);
        glDetachShader(water_shader, vs_id);
        glDeleteShader(fs_id);
        glDeleteShader(vs_id);
    }

    glUseProgram(water_shader);
    GLuint time_loc = glGetUniformLocation(water_shader, "time");
    GLuint base_color_loc = glGetUniformLocation(water_shader, "base_color");
    glUniform1f(time_loc, (float)glfwGetTime());
    glUniform4f(base_color_loc, bot_r, bot_g, bot_b, bot_a);

    GLuint mvp_loc = glGetUniformLocation(water_shader, "mvp");

    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));

    GLuint camPos_loc = glGetUniformLocation(water_shader, "camPos");

    glUniform2f(camPos_loc, camPos.x, camPos.z);


    GLuint wheight_loc = glGetUniformLocation(water_shader, "wheight");

    glUniform1f(wheight_loc, wheight);

    glBindVertexArray(water_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    //glEnable(GL_DEPTH_TEST);
}

int main()
{
    GLWrapper wrap;
    wrap.initializeGL();
    wrap.setupVAO();

    Game game(&wrap);
    game.world.generate();
    std::function<void(Game* g)> surveyTask = [](Game* g) { g->surveyNeededChunks(); };
    //std::function<void(Game* g)> chunkQueueTask = [](Game* g) { g->rebuildNextChunk(); };
    //std::function<void(Game* g)> sortChunkPoolTask = [](Game* g) { g->sortChunkPool(); };

    game.addTask(surveyTask, 5.0f, 1);
    //game.addTask(chunkQueueTask, 0.1, 2);
    //game.addTask(sortChunkPoolTask, 7.0f, 3);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
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

   
    float velocity = 0.0f;
    float jumpTimer = 0.0f;
    while (!glfwWindowShouldClose(wrap.window))
    {
       /* std::cout << wrap.cameraPos.x;
        std::cout << "   z:";
        std::cout << wrap.cameraPos.z;
        std::cout << "   x:";*/
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        wrap.orientCamera();

        //SKY BIT
 
        mygl_GradientBackground(0.5f, 0.5f, 1.0f, 1.0f,
            0.0f, 0.0f, 0.5f, 1.0f, wrap.cameraPitch);


        waterTile(
            0.0f, 0.0f, 1.0f, 1.0f, -10.0f, wrap.mvp, wrap.cameraPos + glm::vec3(1000, 0, 0));
        //END SKY BIT
        glBindVertexArray(wrap.vao);
        
        glUseProgram(wrap.shaderProgram);
        //glDepthMask(GL_TRUE);





        auto meshesView = game.registry.view<MeshComponent>();

        for (auto& entity : meshesView)
        {
            MeshComponent& m = game.registry.get<MeshComponent>(entity);
            wrap.bindGeometryNoUpload(
                m.vbov,
                m.vboc,
                m.vbouv);

            glDrawArrays(GL_TRIANGLES, 0, m.length);
        }


       // glDepthMask(GL_FALSE);
        glBindVertexArray(0);


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
            auto dir = wrap.cameraDirection;
            dir.y = 0;
            wrap.cameraPos += (dir * wrap.activeState.forwardVelocity) * 0.65f;
            wrap.activeState.forwardVelocity *= friction;
        }
        if (wrap.activeState.jump)
        {
            jumpTimer += deltaTime/2.0f;
            velocity -= (deltaTime*25) - jumpTimer;
        }
        intTup camTup((int)std::floor(wrap.cameraPos.x), (int)std::floor(wrap.cameraPos.z));





        if (game.world.heights.find(camTup) != game.world.heights.end()) {
        

            float xi = std::floor(wrap.cameraPos.x);
            float zi = std::floor(wrap.cameraPos.z);
            

            float tx = wrap.cameraPos.x - xi;
            float tz = wrap.cameraPos.z - zi;

            float q11 = game.world.heights.at(camTup);
            float q12 = game.world.heights.at(camTup + intTup(1, 0));
            float q21 = game.world.heights.at(camTup + intTup(0,1));

            float q22 = game.world.heights.at(camTup + intTup(1, 1));




            //float height = (1 - tx) * (1 - tz) * q11 + tx * (1 - tz) * q21 + (1 - tx) * tz * q12 + tx * tz * q22;

         // Perform bilinear interpolation
            float height = glm::mix(glm::mix(q11, q12, tx), glm::mix(q21, q22, tx), tz);



            if (wrap.cameraPos.y - 2 > height) {
                velocity += deltaTime * 6;

            }
            else {
                //wrap.cameraPos.y += ( - ((wrap.cameraPos.y) - (height + 2))   )/4.0f;
                wrap.cameraPos.y = height + 2;
                //velocity = 0;
                wrap.activeState.jump = false;
                jumpTimer = 0.0f;
            }
            if ((wrap.cameraPos.y-2) - velocity >= height)
            {
                wrap.cameraPos += glm::vec3(0, -velocity, 0);
            }
            else {
                wrap.cameraPos.y = height + 2;
                velocity = 0;
                wrap.activeState.jump = false;
                jumpTimer = 0.0f;
            }

        }
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}