#pragma once
#include "Tree.hpp"
#include "Game.hpp"
#include "Perlin.h"
#include <time.h>
#include "Maths.hpp"
perlin p;

long World::worldSeed = 0;
int modelNum = 0;

std::vector<std::function<Model(float,float,float)>> objs = {
	[](
		float x,
		float y,
		float z
	)
	{
		return Tree::getTreeModel(x,y,z);
	},
	[](
		float x,
		float y,
		float z
	)
	{
		return Rock::getRockModel(x,y,z);
	},
	[](
		float x,
		float y,
		float z
	)
	{
		return Tree::getPineTreeModel(x,y,z);
	}
};

Model nextModel(float x, float y, float z)
{
	modelNum = (modelNum + 1) % objs.size();
	return objs[modelNum](x, y, z);
}

void World::generate() {
	srand(time(NULL));
	worldSeed = 10000 + rand()*10000 + rand()*100000;
	for (int x = -5; x < 5; x++)
	{
		for (int z = -5; z < 5; z++)
		{

				intTup tup(x,z);
				this->generateOneChunk(tup);

		}
	}
}

int World::generateOneChunk(intTup coord) {
	
	int realX = coord.x * CHUNK_WIDTH;
	int realY = coord.y * CHUNK_WIDTH;
	int realZ = coord.z * CHUNK_WIDTH;

	int blockCount = 0;

	this->hasSimpMarks.insert_or_assign(coord, 1);

	for (int x = -1; x < CHUNK_WIDTH+1; x++)
	{
		for (int z = -1; z < CHUNK_WIDTH+1; z++)
		{
			int localX = realX + x;
			int localZ = realZ + z;

			intTup tup(localX, localZ);
			double noise = p.noise((long double)(worldSeed + localX) / 125.25, 30.253, (long double)(worldSeed + localZ) / 125.25)*25;

				
			
				if (rando() < 0.0005)
				{
					Model m = nextModel(tup.x, noise, tup.z);
					this->models.insert_or_assign(tup, m);
				}


				this->heights.insert_or_assign(tup, (float)noise);
				

			
		}
	}
	return blockCount;
}