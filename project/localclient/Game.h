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
    folly::F14FastMap<std::tuple<int>, uint8_t> data;
    void generate();
    void generateOneChunk();
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


