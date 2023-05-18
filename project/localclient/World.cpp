#include "Game.h"
#include "Perlin.h"

perlin p;

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

int World::generateOneChunk(intTup coord) {


	int realX = coord.x * CHUNK_WIDTH;
	int realY = coord.y * CHUNK_WIDTH;
	int realZ = coord.z * CHUNK_WIDTH;

	int blockCount = 0;


	this->isHandledMarks.insert_or_assign(coord, 1);
	for (int y = 0; y < CHUNK_WIDTH; y++)
	{
		for (int x = 0; x < CHUNK_WIDTH; x++)
		{
			for (int z = 0; z < CHUNK_WIDTH; z++)
			{
				int localX = realX + x;
				int localY = realY + y;
				int localZ = realZ + z;

				intTup tup(localX, localY, localZ);
				double noise = p.noise((double)localX / 25.25, 30.253, (double)localZ / 25.25)*15;
				if (localY < noise)
				{
					this->data.insert_or_assign(tup, 1);
					blockCount++;
					if (blockCount == std::pow(CHUNK_WIDTH, 3))
					{
						this->fullBlockMarks.insert_or_assign(coord, 1);
					}
					if (this->hasBlockMarks.find(coord) == this->hasBlockMarks.end())
					{
						this->hasBlockMarks.insert_or_assign(coord, 1);
					}
				}
			}
		}
	}
	return blockCount;
}