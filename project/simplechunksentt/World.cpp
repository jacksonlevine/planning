#pragma once
#include "Tree.hpp"
#include "Game.hpp"
#include "Perlin.h"

#include "Maths.hpp"
perlin p;

void World::generate() {
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


	//this->isHandledMarks.insert_or_assign(coord, 1);

	this->hasSimpMarks.insert_or_assign(coord, 1);

	for (int x = -1; x < CHUNK_WIDTH+1; x++)
	{
		for (int z = -1; z < CHUNK_WIDTH+1; z++)
		{
			int localX = realX + x;
			int localZ = realZ + z;

			intTup tup(localX, localZ);
			double noise = p.noise((double)localX / 125.25, 30.253, (double)localZ / 125.25)*25;

				
			
				if (rando() < 0.0005)
				{
					Model m = Tree::getTreeModel(tup.x, noise, tup.z);
					this->models.insert_or_assign(tup, m);
				}
				else {
					if (rando() < 0.001)
					{
						Model m = Rock::getRockModel(tup.x, noise, tup.z);
						this->models.insert_or_assign(tup, m);
					}
				}

				this->heights.insert_or_assign(tup, (float)noise);
				

			
		}
	}
	return blockCount;
}