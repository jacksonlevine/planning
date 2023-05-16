#include "Game.h"
#include "Perlin.h"

perlin p;

World::World(Game* g) : gref(g) {

}

void World::generate() {

}

void World::generateOneChunk(std::tuple<int, int, int> coord) {


	int realX = std::get<0>(coord) * this->gref->chunkWidth;
	int realY = std::get<1>(coord) * this->gref->chunkWidth;
	int realZ = std::get<2>(coord) * this->gref->chunkWidth;

	for (int y = 0; y < this->gref->chunkWidth; y++)
	{
		for (int x = 0; x < this->gref->chunkWidth; x++)
		{
			for (int z = 0; z < this->gref->chunkWidth; z++)
			{
				int localX = realX + x;
				int localY = realY + y;
				int localZ = realZ + z;

				std::tuple<int, int, int> tup = std::make_tuple(localX, localY, localZ);
				double noise = p.noise((double)localX / (double)25.25, (double)30.253, (double)localZ / (double)25.25)*15;
				if (localY < noise)
				{
					this->data.insert_or_assign(tup, 1);
				}
			}
		}
	}
}