#pragma once
#include "Tree.hpp"
#include "Game.hpp"
#include "Perlin.h"
#include <time.h>
#include "Maths.hpp"
#include "Biomes.hpp"

perlin p;

long World::worldSeed = 0;
int modelNum = 0;

std::vector<std::function<Model(float, float, float)>> forestObjs = {
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
	}
};
std::vector<std::function<Model(float, float, float)>> mountainObjs = {
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

std::vector<std::function<Model(float, float, float)>> desertObjs = {
	[](
		float x,
		float y,
		float z
	)
	{
		return Plant::getPricklyPearCactusModel(x,y,z);
	},
	[](
		float x,
		float y,
		float z
	)
	{
		return Plant::getSaguaroCactusModel(x,y,z);
	}
};
std::vector<Biome> biomes =
{
	Biome(
		forestObjs,
		[](float height) {
			return (height < -1.0) ? 3 : (height > -2 && height < 9) ? 0
				: (height > 9 && height < 24) ? 1 : 0;
		},
		1
	),
	Biome(
		mountainObjs,
		[](float height) {
			return 5;
		},
		10
	),
	Biome(
		desertObjs,
		[](float height) {
			return 4;
		},
		1
	)
};







Model nextModel(float x, float y, float z, Biome& biome)
{

	modelNum = (modelNum + 1) % biome.objectsHere.size();

	return biome.objectsHere[modelNum](x, y, z);
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
			double noise = p.noise((long double)(worldSeed + localX) / 150.25, 30.253, (long double)(worldSeed + localZ) / 150.25)*25;
			double noise2 = p.noise((long double)(worldSeed + localX) / 800.25, 30.253, (long double)(worldSeed + localZ) / 800.25) * 60;
			noise += noise2;
			double bigNoise = p.noise((long double)(worldSeed + localX) / 13000.25, 30.253, (long double)(worldSeed + localZ) / 13000.25);
			int biomeID = std::max(std::min((int)std::abs(((bigNoise*6))), (int)biomes.size()-1), 0);

			
			Biome& biome = biomes[biomeID];
				if (rando() < (0.0005 * biome.objectFrequency) && noise > Game::instance->waterHeight)
				{
					Model m = nextModel(tup.x, noise, tup.z, biome);
					this->models.insert_or_assign(tup, m);
				}
				HeightTile h;
				h.height = (float)noise;
				h.blockID = biome.blockIdFunction((float)noise);

				this->heights.insert_or_assign(tup, h);
				

			
		}
	}
	return blockCount;
}