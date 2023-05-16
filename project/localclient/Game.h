#pragma once
#include <folly/Optional.h>
#include <folly/container/F14Map.h>
#include "GLSetup.h"
class Chunk {
public:
    Chunk(uint8_t width);
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> colors;
    void rebuildMesh();
    void moveAndRebuildMesh();
 };

class Game {
    GLWrapper wrap;
    const uint8_t chunkWidth;
    std::vector<Chunk> chunkPool;
    folly::F14FastMap<std::tuple<int>, Chunk*> activeChunks;
    Game();
};

class World {
public:
    folly::F14FastMap<std::tuple<int>, uint8_t> data;
    void generate();
    void generateOneChunk();
};

