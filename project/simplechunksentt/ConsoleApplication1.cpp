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
    float bot_r, float bot_g, float bot_b, float bot_a, float wheight, glm::mat4 mvp, glm::mat4 model, glm::vec3 camPos)
{
    //glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    static GLuint water_vao = 0;
    static GLuint water_shader = 0;

    if (water_vao == 0)
    {
        glGenVertexArrays(1, &water_vao);

        const GLchar* vs_src =
            "#version 450 core\n"
            "out vec2 v_uv;\n"
            "out vec2 s;\n"
            "uniform float wheight;\n"
            "uniform mat4 mvp;\n"

            "uniform mat4 model;\n"
            "uniform vec2 camPos;\n"
            "void main()\n"
            " {\n"
            " uint idx = gl_VertexID;\n"

            " gl_Position = mvp * vec4(  ((camPos.x-2500) + (idx & 1)*5000), wheight, ((camPos.y - 2500) + (idx >> 1) * 5000), 1.0  ) * 5000.0 - 1.0;\n"
            "vec4 ha = model * vec4(  ((camPos.x-2500) + (idx & 1)*5000), wheight, ((camPos.y - 2500) + (idx >> 1) * 5000), 1.0  ) * 5000.0 - 1.0;\n"
            "v_uv =  vec2(ha.x, ha.z);\n"
            "s = vec2(gl_Position.x, gl_Position.z);\n"
            "}";


        const GLchar* fs_src =
            " #version 450 core\n"
            "uniform float time;\n"
            "uniform vec4 base_color;\n"
            "in vec2 v_uv;\n"
            "in vec2 s;\n"
            "out vec4 frag_color;\n"


            "vec2 _fade(vec2 t) { return t * t * t * (t * (t * 6.0 - 15.0) + 10.0); }\n"
            "vec3 _fade(vec3 t) { return t * t * t * (t * (t * 6.0 - 15.0) + 10.0); }\n"
            "vec4 gln_rand4(vec4 p) { return mod(((p * 34.0) + 1.0) * p, 289.0); }\n"
            "vec4 _permute(vec4 x) { return mod(((x * 34.0) + 1.0) * x, 289.0); }\n"
        "vec4 _taylorInvSqrt(vec4 r) { return 1.79284291400159 - 0.85373472095314 * r; }\n"

            "float gln_perlin(vec3 P) {\n"
           " vec3 Pi0 = floor(P);\n"        // Integer part for indexing
            "vec3 Pi1 = Pi0 + vec3(1.0);\n" // Integer part + 1
            "Pi0 = mod(Pi0, 289.0);\n"
            "Pi1 = mod(Pi1, 289.0);\n"
           " vec3 Pf0 = fract(P);\n"        // Fractional part for interpolation
           " vec3 Pf1 = Pf0 - vec3(1.0);\n" // Fractional part - 1.0
           " vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);\n"
           " vec4 iy = vec4(Pi0.yy, Pi1.yy);\n"
           " vec4 iz0 = Pi0.zzzz;\n"
           " vec4 iz1 = Pi1.zzzz;\n"

           " vec4 ixy = _permute(_permute(ix) + iy);\n"
           " vec4 ixy0 = _permute(ixy + iz0);\n"
           " vec4 ixy1 = _permute(ixy + iz1);\n"

           " vec4 gx0 = ixy0 / 7.0;\n"
           " vec4 gy0 = fract(floor(gx0) / 7.0) - 0.5;\n"
           " gx0 = fract(gx0);\n"
           " vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);\n"
           " vec4 sz0 = step(gz0, vec4(0.0));\n"
           " gx0 -= sz0 * (step(0.0, gx0) - 0.5);\n"
           " gy0 -= sz0 * (step(0.0, gy0) - 0.5);\n"

           " vec4 gx1 = ixy1 / 7.0;\n"
           " vec4 gy1 = fract(floor(gx1) / 7.0) - 0.5;\n"
           " gx1 = fract(gx1);\n"
           " vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);\n"
           " vec4 sz1 = step(gz1, vec4(0.0));\n"
          "  gx1 -= sz1 * (step(0.0, gx1) - 0.5);\n"
          "  gy1 -= sz1 * (step(0.0, gy1) - 0.5);\n"

           " vec3 g000 = vec3(gx0.x, gy0.x, gz0.x);\n"
           " vec3 g100 = vec3(gx0.y, gy0.y, gz0.y);\n"
           " vec3 g010 = vec3(gx0.z, gy0.z, gz0.z);\n"
           " vec3 g110 = vec3(gx0.w, gy0.w, gz0.w);\n"
           " vec3 g001 = vec3(gx1.x, gy1.x, gz1.x);\n"
           " vec3 g101 = vec3(gx1.y, gy1.y, gz1.y);\n"
          "  vec3 g011 = vec3(gx1.z, gy1.z, gz1.z);\n"
           " vec3 g111 = vec3(gx1.w, gy1.w, gz1.w);\n"

            "vec4 norm0 = _taylorInvSqrt(\n"
            "    vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));\n"
           " g000 *= norm0.x;\n"
           " g010 *= norm0.y;\n"
            "g100 *= norm0.z;\n"
           " g110 *= norm0.w;\n"
           " vec4 norm1 = _taylorInvSqrt(\n"
           "     vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));\n"
           " g001 *= norm1.x;\n"
           " g011 *= norm1.y;\n"
           " g101 *= norm1.z;\n"
          "  g111 *= norm1.w;\n"

          "  float n000 = dot(g000, Pf0);\n"
           " float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));\n"
           " float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));\n"
           " float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));\n"
           " float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));\n"
           " float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));\n"
           " float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));\n"
           " float n111 = dot(g111, Pf1);\n"

           " vec3 fade_xyz = _fade(Pf0);\n"
           " vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111),\n"
           "     fade_xyz.z);\n"
           " vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);\n"
           " float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x);\n"
           " return 2.2 * n_xyz;\n"
        "}\n"


            "void main()\n"
            "{\n"

            "vec2 worldPosition = v_uv + vec2(gl_FragCoord.x, gl_FragCoord.y);"
            "float p1 =  gln_perlin(   vec3((worldPosition.x/500.35) + time, time,  worldPosition.y/500.35 + time)  );\n"

            "frag_color = vec4(1.0,1.0,1.0,0.7) * p1;\n"
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

    GLuint model_loc = glGetUniformLocation(water_shader, "model");

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));


    GLuint camPos_loc = glGetUniformLocation(water_shader, "camPos");

    glUniform2f(camPos_loc, camPos.x, camPos.z);


    GLuint wheight_loc = glGetUniformLocation(water_shader, "wheight");

    glUniform1f(wheight_loc, wheight);

    glBindVertexArray(water_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    //glEnable(GL_DEPTH_TEST);

    glDepthMask(GL_TRUE);
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float velocity = 0.0f;
    float jumpTimer = 0.0f;

    auto meshesView = game.registry.view<MeshComponent>();
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

         //glDepthMask(GL_FALSE);


        //glDepthMask(GL_TRUE);
        //END SKY BIT
        glBindVertexArray(wrap.vao);
        
        glUseProgram(wrap.shaderProgram);
       






        for (auto& entity : meshesView)
        {
            MeshComponent& m = game.registry.get<MeshComponent>(entity);
            wrap.bindGeometryNoUpload(
                m.vbov,
                m.vboc,
                m.vbouv);

            glDrawArrays(GL_TRIANGLES, 0, m.length);
        }

        waterTile(
            0.0f, 0.0f, 1.0f, 1.0f, 5.0f, wrap.mvp, wrap.model, wrap.cameraPos + glm::vec3(1000, 0, 0));

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