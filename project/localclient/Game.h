#pragma once
#include <folly/Optional.h>
#include <folly/container/F14Map.h>
#include "GLSetup.h"

struct intTup {
public:
    int x;
    int y;
    int z;
    intTup(int x, int y, int z);
    bool operator==(const intTup& other) const;
};

class Game;
class Chunk {
public:
    int x;
    int y;
    int z;
    Chunk(Game* gref);
    Game* gref;
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> colors;
    void rebuildMesh();
    void moveAndRebuildMesh(int newX, int newY, int newZ);
 };

class World {
public:
    folly::F14NodeMap<intTup, uint8_t> data;
    Game* gref;
    void generate();
    void generateOneChunk(intTup pos);
    World(Game* g);
};

struct IntervalTask {
    float timer;
    float interval;
    uint8_t id;
    std::function<void()> lambda;

    IntervalTask(float interval, std::function<void()> lambda, uint8_t id);
};

class Game {
public:
    //GLWrapper* wrap;
    World world;
    const uint8_t chunkWidth;
    std::vector<Chunk> chunkPool;
    std::vector<IntervalTask> tasks;
    folly::F14NodeMap<intTup, Chunk*> activeChunks;
    Game(World w);
    void updateTasks(float delt);
    void addTask(std::function<void()> func, float interval, uint8_t id);
};


