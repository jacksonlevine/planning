#pragma once
#include <vector>
#include "GL/glew.h"
#include "GLFW/glfw3.h"

class Model {
public:
    std::vector<GLfloat> verts;
    std::vector<GLfloat> cols;
    std::vector<GLfloat> uvs;
};

class ModelShower {
public:
    GLuint vbov;
    GLuint vboc;
    GLuint vbouv;
    int length;
    bool active;
    ModelShower();
};