#pragma once
#include <folly/Optional.h>
#include <folly/container/F14Map.h>
#include <folly/container/F14Set.h>
#include "GLSetup.h"

#define CHUNK_WIDTH 20

struct intTup {
public:
    int x;
    int y;
    int z;
    intTup(int x, int y, int z);
    bool operator==(const intTup& other) const;
};

struct intTupHash {
    std::size_t operator()(const intTup& tup) const;
};

class Game;
class Chunk {
public:
    int x;
    int y;
    int z;
    bool dirty;
    bool bufferDeleted;
    Chunk(Game* gref);
    Game* gref;
    GLuint vbov;
    GLuint vboc;
    GLuint vbouv;
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> colors;
    std::vector<GLfloat> uv;
    void rebuildMesh();
    void moveAndRebuildMesh(int newX, int newY, int newZ);
 };

class World {
public:
    folly::F14FastMap<intTup, uint8_t, intTupHash> data;
    folly::F14FastMap<intTup, uint8_t, intTupHash> hasBlockMarks;
    folly::F14FastMap<intTup, uint8_t, intTupHash> fullBlockMarks;
    folly::F14FastMap<intTup, uint8_t, intTupHash> isHandledMarks;
    void generate();
    int generateOneChunk(intTup pos);
};

struct IntervalTask {
    float timer;
    float interval;
    uint8_t id;
    std::function<void(Game* g)> lambda;

    IntervalTask(float interval, std::function<void(Game* g)> lambda, uint8_t id);
};

class Game {
public:
    GLWrapper* wrap;
    World world;
    const uint8_t chunkWidth;
    std::vector<Chunk> chunkPool;
    std::vector<IntervalTask> tasks;
    std::unordered_map<intTup, Chunk, intTupHash> activeChunks;
    std::unordered_set<intTup, intTupHash> neededChunks;
    std::vector<intTup> sortedNeededChunks;
    Game(GLWrapper* wr);
    void updateTasks(float delt);
    void addTask(std::function<void(Game* g)> func, float interval, uint8_t id);
    void removeTask(uint8_t id);
    void surveyNeededChunks();
    void rebuildNextChunk();
    void sortChunkPool();
    static int compareChunksInPool(const void* a, const void* b);
    static Game* instance;
};
