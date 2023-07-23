#pragma once
#include <folly/Optional.h>
#include <folly/container/F14Map.h>
#include <folly/container/F14Set.h>
#include "GLSetup.hpp"
class Game;
struct intTup;
#include "Model.hpp"

#include "entt/entt.hpp"
#include "SimpleChunk.hpp"
#include "Comm.hpp"
#define CHUNK_WIDTH 20

struct intTup {
public:
    int x;
    int y;
    int z;
    intTup(int x, int y, int z);
    intTup(int x, int z);
    bool operator==(const intTup& other) const;
    bool operator==(const SimpleChunk* other) const;
    intTup& operator+=(const intTup& other);
    
};
// non-mutating => non-member function
intTup operator+(intTup first, // parameter as value, move-construct (or elide)
    const intTup& second);


struct intTupHash {
    std::size_t operator()(const intTup& tup) const;
};


class Chunk {
public:
    int x;
    int y;
    int z;
    bool dirty;
    bool bufferDeleted;
    bool active;
    Chunk(Game* gref);
    Game* gref;
    entt::entity me;
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> colors;
    std::vector<GLfloat> uv;
    void rebuildMesh();
    void moveAndRebuildMesh(int newX, int newY, int newZ);
 };

struct HeightTile
{
public:
    float height;
    uint8_t blockID;
};

class World {
public:
    folly::F14FastMap<intTup, uint8_t, intTupHash> data;
    folly::F14FastMap<intTup, uint8_t, intTupHash> hasBlockMarks;
    folly::F14FastMap<intTup, uint8_t, intTupHash> fullBlockMarks;
    folly::F14FastMap<intTup, uint8_t, intTupHash> isHandledMarks;
    folly::F14FastMap<intTup, uint8_t, intTupHash> hasSimpMarks;
    std::unordered_map<intTup, Model, intTupHash> models;
    std::unordered_map<intTup, HeightTile, intTupHash> heights;

    static long worldSeed;
    static bool generateSeed;

    static double getTerrainNoise(int x, int z);
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
    float waterHeight;
    std::vector<ModelShower> modelShowerPool;
    std::vector<ModelShower> activeShowers;

    std::vector<SimpleChunk> simpleChunkPool;
    std::vector<ZeroChunk> zeroChunkPool;
    std::unordered_map<intTup, SimpleChunk, intTupHash> activeSimpChunks;
    std::unordered_map<intTup, ZeroChunk, intTupHash> activeZeroChunks;

    std::vector<std::string> serverCommandQueue;
    std::vector<Player> otherPlayersIfMultiplayer;

    std::vector<intTup> neededSimpChunks;
    std::vector<intTup> neededZeroChunks;

    entt::registry registry;

    Game(GLWrapper* wr);
    intTup castRayBlocking(float x, float y, float z, glm::vec3 d, float maxDistance, float stepBack);
    void onRightClick();
    void onLeftClick();
    void placeBlock(intTup spot, uint8_t blockID);
    void breakBlock(intTup spot);
    void updateTasks(float delt);
    void addTask(std::function<void(Game* g)> func, float interval, uint8_t id);
    void removeTask(uint8_t id);
    void surveyNeededChunks();
    void initialSurvey();
    void rebuildNextChunk();
    void sortChunkPool();
    static int compareChunksInPool(const void* a, const void* b);
    static Game* instance;
    float commsTimer;
};
