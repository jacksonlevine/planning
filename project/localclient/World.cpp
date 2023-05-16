#include "Game.h"
#include "Perlin.h"

perlin p;

World::World(Game* g) : gref(g) {

}

void World::generate() {
	for (int x = -5; x < 5; x++)
	{
		for (int z = -5; z < 5; z++)
		{
			for (int y = -5; y < 5; y++)
			{
				intTup tup(x,y,z);
				this->generateOneChunk(tup);
			}
		}
	}
}

void World::generateOneChunk(intTup coord) {


	int realX = coord.x * this->gref->chunkWidth;
	int realY = coord.y * this->gref->chunkWidth;
	int realZ = coord.z * this->gref->chunkWidth;

	for (int y = 0; y < this->gref->chunkWidth; y++)
	{
		for (int x = 0; x < this->gref->chunkWidth; x++)
		{
			for (int z = 0; z < this->gref->chunkWidth; z++)
			{
				int localX = realX + x;
				int localY = realY + y;
				int localZ = realZ + z;

				intTup tup(localX, localY, localZ);
				double noise = p.noise((double)localX / 25.25, 30.253, (double)localZ / 25.25)*15;
				if (localY < noise)
				{
					this->data.insert_or_assign(tup, 1);
				}
			}
		}
	}
}