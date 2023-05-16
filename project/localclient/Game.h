#pragma once
#include <folly/Optional.h>
#include <folly/container/F14Map.h>

class Chunk {

};

class Game {
    std::vector<Chunk> chunkPool;
    folly::F14FastMap<std::tuple<int>, &Chunk> activeChunks;
};

class World {
public:
    folly::F14FastMap<std::tuple<int>, uint8_t> data;
    void generate();
    void generateOneChunk();
};

