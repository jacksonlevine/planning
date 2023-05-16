#include "Game.h"
#include "Perlin.h"

const perlin p;

World::World(Game* g) : gref(g) {

}

void World::generate() {

}

void World::generateOneChunk(std::tuple<int> coord) {

	auto [x, y, z] = coord;
	int realX = std::get<0>(coord) * this->gref->chunkWidth;
	int realY = std::get<1>(coord) * this->gref->chunkWidth;
	int realZ = std::get<2>(coord) * this->gref->chunkWidth;
}