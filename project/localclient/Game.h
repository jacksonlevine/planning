#pragma once
#include <folly/Optional.h>
#include <folly/container/F14Map.h>
#include "GLSetup.h"

class Game;
class Chunk {
public:
    Chunk(Game* gref);
    Game* gref;
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> colors;
    void rebuildMesh();
    void moveAndRebuildMesh();
 };

class World {
public:
    folly::F14FastMap<std::tuple<int, int, int>, uint8_t> data;
    Game* gref;
    void generate();
    void generateOneChunk(std::tuple<int, int, int> pos);
    World(Game* g);
};

class Game {
public:
    GLWrapper* wrap;
    World world;
    const uint8_t chunkWidth;
    std::vector<Chunk> chunkPool;
    folly::F14FastMap<std::tuple<int>, Chunk*> activeChunks;
    Game(GLWrapper* wrapref);
};


