#pragma once
#include <folly/Optional.h>
#include <folly/container/F14Map.h>
#include "GLSetup.h"
class Chunk {
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> colors;

 };

class Game {
    GLWrapper wrap;
    std::vector<Chunk> chunkPool;
    folly::F14FastMap<std::tuple<int>, Chunk*> activeChunks;
};

class World {
public:
    folly::F14FastMap<std::tuple<int>, uint8_t> data;
    void generate();
    void generateOneChunk();
};

