#pragma once
#include <vector>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Scene.hpp"
#include "entt/entt.hpp"
class ModelShower;
class Model;
#include "Game.hpp"

class Model {
public:
    std::vector<GLfloat> verts;
    std::vector<GLfloat> cols;
    std::vector<GLfloat> uvs;
};

class ModelShower {
public:

    Game* gref;
    entt::entity me;
    int length;
    bool active;
    ModelShower(Game* g);
    void setMesh(std::vector<GLfloat>& verts, std::vector<GLfloat>& cols, std::vector<GLfloat>& uvs);
};