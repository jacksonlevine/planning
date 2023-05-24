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
#include <stdlib.h>
#include <time.h>


void mygl_GradientBackground(float top_r, float top_g, float top_b, float top_a,
    float bot_r, float bot_g, float bot_b, float bot_a)
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
            "void main()\n"
            " {\n"
            " uint idx = gl_VertexID;\n"
            " gl_Position = vec4(idx & 1, idx >> 1, 0.0, 0.5) * 4.0 - 1.0;\n"
            "v_uv = vec2(gl_Position.xy * 0.5 + 0.5);\n"
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

    glBindVertexArray(background_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}
float rando() {
    return (float)(rand() / (float)RAND_MAX);
}


glm::vec3 rotateAroundPoint(const glm::vec3& point, const glm::vec3& center, float angle)
{
    glm::vec3 translated = point - center;

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 rotated = glm::vec3(rotationMatrix * glm::vec4(translated, 1.0f));

    glm::vec3 finalPoint = rotated + center;

    return finalPoint;
}
class TrianglePen {
public:
    glm::vec3 front;
    glm::vec3 bl;
    glm::vec3 br;
    TrianglePen(
        glm::vec3 front,
        glm::vec3 bl,
        glm::vec3 br
    ) :
        front(front),
        bl(bl),
        br(br)
    {

    }
    void move(glm::vec3 d)
    {
        front += d;
        bl += d;
        br += d;

    }
    void moveToPen(TrianglePen& pen)
    {
        front = pen.front;
        bl = pen.bl;
        br = pen.br;
    }
    void rotate(float ang)
    {
        glm::vec3 center = (front + bl + br) / 3.0f;
        float angle = glm::radians(ang);

        front = rotateAroundPoint(front, center, angle);
        bl = rotateAroundPoint(bl, center, angle);
        br = rotateAroundPoint(br, center, angle);
    }
};


void threadThesePens(
    TrianglePen &pen,
    TrianglePen &penBack, 
    std::vector<GLfloat>& verts, 
    std::vector<GLfloat>& cols,
    std::vector<GLfloat>& uvs,
    int length,
    glm::vec3 currentDirection
) 
{

   


    TextureFace trunkFace(3, 0);
    for (int b = 0; b < length; b++)
    {

        int posOrNeg = rando() > 0.5 ? -1 : 1;

        currentDirection += glm::vec3((rando() * -posOrNeg) / 10, 0, (rando() * posOrNeg) / 10);

        pen.move(currentDirection);
        pen.rotate(-10.0f);


        verts.insert(verts.end(), {
            //left side
            penBack.bl.x, penBack.bl.y, penBack.bl.z,
            penBack.front.x, penBack.front.y, penBack.front.z,
            pen.front.x, pen.front.y, pen.front.z,

            pen.front.x, pen.front.y, pen.front.z,
            pen.bl.x, pen.bl.y, pen.bl.z,
            penBack.bl.x, penBack.bl.y, penBack.bl.z,

            //right side
            penBack.front.x, penBack.front.y, penBack.front.z,
            penBack.br.x, penBack.br.y, penBack.br.z,
            pen.br.x, pen.br.y, pen.br.z,

            pen.br.x, pen.br.y, pen.br.z,
            pen.front.x, pen.front.y, pen.front.z,
            penBack.front.x, penBack.front.y, penBack.front.z,

            //back side
            penBack.br.x, penBack.br.y, penBack.br.z,
            penBack.bl.x, penBack.bl.y, penBack.bl.z,
            pen.bl.x, pen.bl.y, pen.bl.z,

            pen.bl.x, pen.bl.y, pen.bl.z,
            pen.br.x, pen.br.y, pen.br.z,
            penBack.br.x, penBack.br.y, penBack.br.z,

            });
        penBack.moveToPen(pen);

        for (int i = 0; i < 18; i++)
        {
            cols.insert(cols.end(), { 1.0, 1.0, 1.0 });

        }
        for (int i = 0; i < 3; i++)
        {
            uvs.insert(uvs.end(), {
                trunkFace.bl.x, trunkFace.bl.y,
                trunkFace.br.x, trunkFace.br.y,
                trunkFace.tr.x, trunkFace.tr.y,

                trunkFace.tr.x, trunkFace.tr.y,
                trunkFace.tl.x, trunkFace.tl.y,
                trunkFace.bl.x, trunkFace.bl.y,

                });

        }
    }
}


int main()
{

    GLWrapper wrap;
    wrap.initializeGL();
    wrap.setupVAO();

    //Game game(&wrap);
    //game.world.generate();
    //std::function<void(Game* g)> surveyTask = [](Game* g) { g->surveyNeededChunks(); };
    //std::function<void(Game* g)> chunkQueueTask = [](Game* g) { g->rebuildNextChunk(); };
    //std::function<void(Game* g)> sortChunkPoolTask = [](Game* g) { g->sortChunkPool(); };

   // game.addTask(surveyTask, 5.0f, 1);
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
    //game.surveyNeededChunks();
    
    const float friction = 0.75;
    glPatchParameteri(GL_PATCH_VERTICES, 0);
    glPatchParameteri(GL_PATCH_DEFAULT_OUTER_LEVEL, 1);
    glPatchParameteri(GL_PATCH_DEFAULT_INNER_LEVEL, 1);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    //MODEL CREATION
    //glBindVertexArray(wrap.vao);
    GLuint vboV;
    GLuint vboC;
    GLuint vboUV;

    glGenBuffers(1, &vboV);
    glGenBuffers(1, &vboC);
    glGenBuffers(1, &vboUV);

    std::vector<GLfloat> verts;
    std::vector<GLfloat> cols;
    std::vector<GLfloat> uvs;

 

    TrianglePen pen(
        glm::vec3(0.0, 0.0, 0.0),
        glm::vec3(-0.5, 0.0, 0.7),
        glm::vec3(0.5, 0.0, 0.8)
    );

    TrianglePen penBack(
        glm::vec3(0.0, 0.0, 0.0),
        glm::vec3(-0.5, 0.0, 0.7),
        glm::vec3(0.5, 0.0, 0.8)
    );

    srand((unsigned)time(NULL));

    int posOrNeg2 = rando() > 0.5 ? -1 : 1;

    glm::vec3 currentDirection(0, 0.5f, 0);
    threadThesePens(pen, penBack, verts, cols, uvs, 9, currentDirection);


    glm::vec3 tbCenter = (pen.bl + pen.br) / 2.0f;
    int leftBranches = rando() * 8.0;
    int rightBranches = rando() * 8.0;

    for (int z = 0; z < leftBranches; z++)
    {
        int branchLength = rando() * 20;
        glm::vec3 currentDirection2(-rando() / 3 + (rando() / 10) * posOrNeg2, 0.5f, -(rando() / 2));

        TrianglePen pen2(
            pen.front,
            pen.bl,
            tbCenter
        );

        TrianglePen pen2Back(
            pen.front,
            pen.bl,
            tbCenter
        );
  
        threadThesePens(pen2, pen2Back, verts, cols, uvs, branchLength, currentDirection2);
    }
    for(int z = 0; z < rightBranches; z++)
    {
        int branchLength = rando() * 20;
        glm::vec3 currentDirection3(rando() / 3 + (rando() / 10) * posOrNeg2, 0.5f, (rando() / 2));
        TrianglePen pen3(
            pen.front,
            tbCenter,
            pen.br
        );
        TrianglePen pen3Back(
            pen.front,
            tbCenter,
            pen.br
        );
        threadThesePens(pen3, pen3Back, verts, cols, uvs, branchLength, currentDirection3);
    }


    //END MODEL CREATION
   
    

    while (!glfwWindowShouldClose(wrap.window))
    {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        wrap.orientCamera();

        //SKY BIT
 
        mygl_GradientBackground(0.5f, 0.5f, 1.0f, 1.0f,
            0.0f, 0.0f, 0.5f, 1.0f);
        //END SKY BIT
        glBindVertexArray(wrap.vao);

        glUseProgram(wrap.shaderProgram);
        wrap.bindGeometry(
            vboV,
            vboC,
            vboUV,
            &(verts[0]),
            &(cols[0]),
            &(uvs[0]),
            sizeof(GLfloat)* verts.size(),
            sizeof(GLfloat)* cols.size(),
            sizeof(GLfloat)* uvs.size());

        // BIND ONCE ^^ 



               
        glDrawArrays(GL_TRIANGLES, 0, verts.size());
     

        glBindVertexArray(0);
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        wrap.deltaTime = deltaTime;
        lastFrame = currentFrame;
        //game.updateTasks(deltaTime*10);

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