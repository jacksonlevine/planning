#pragma once
#include "Tree.hpp"
#include "Game.hpp"
#include "Perlin.h"
#include <time.h>
#include "Maths.hpp"
#include "Biomes.hpp"

perlin p;

long World::worldSeed = 0;
bool World::generateSeed = true;
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
	}, 
	[](
		float x,
		float y,
		float z
	)
	{
		return Rock::getSmallRockModel(x,y,z);
	},
	[](
		float x,
		float y,
		float z
	)
	{
		return Plant::getShortGrassModel(x,y,z);
	},
	[](
		float x,
		float y,
		float z
	)
	{
		return Plant::getDeadShortGrassModel(x,y,z);
	},
	[](
		float x,
		float y,
		float z
	)
	{
		return Plant::getTallGrassModel(x,y,z);
	},
	[](
		float x,
		float y,
		float z
	)
	{
		return Plant::getDeadTallGrassModel(x,y,z);
	},
	[](
		float x,
		float y,
		float z
	)
	{
		return Plant::getBlackberryBushModel(x,y,z);
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
	},
	[](
		float x,
		float y,
		float z
	)
	{
		return Plant::getBlackberryBushModel(x,y,z);
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
	},
	[](
		float x,
		float y,
		float z
	)
	{
		return Plant::getDeadShortGrassModel(x,y,z);
	},
	[](
		float x,
		float y,
		float z
	)
	{
		return Plant::getDeadTallGrassModel(x,y,z);
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
		10
	),
	Biome(
		mountainObjs,
		[](float height) {
			return 5;
		},
		2
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
	if (World::generateSeed)
	{
		srand(time(NULL));
		worldSeed = 10000 + rand() * 10000 + rand() * 100000;
	}

	for (int x = -5; x < 5; x++)
	{
		for (int z = -5; z < 5; z++)
		{

				intTup tup(x,z);
				this->generateOneChunk(tup);

		}
	}
}
double World::getTerrainNoise(int localX, int localZ)
{
	double noise = p.noise((long double)(worldSeed + localX) / 150.25, 30.253, (long double)(worldSeed + localZ) / 150.25) * 25;

	double noise2 = p.noise((long double)(worldSeed + localX) / 800.25, 30.253, (long double)(worldSeed + localZ) / 800.25) * 60;
	return noise + noise2;
}
intTup loadPen[] = {
	intTup(0,0),
	intTup(1,0),
	intTup(0,1),
	intTup(1,1)
};
int World::generateOneChunk(intTup coord) {
	
	int realX = coord.x * CHUNK_WIDTH;
	int realY = coord.y * CHUNK_WIDTH;
	int realZ = coord.z * CHUNK_WIDTH;

	int blockCount = 0;

	this->hasSimpMarks.insert_or_assign(coord, 1);

	for (int x = -1; x < CHUNK_WIDTH+1; x+=2)
	{
		for (int z = -1; z < CHUNK_WIDTH+1; z+=2)
		{
			intTup t(realX + x, realZ + z);
			double n = 0;
			double bigNoise = p.noise((long double)(worldSeed + t.x) / 13000.25, 30.253, (long double)(worldSeed + t.z) / 13000.25);
			int biomeID = std::max(std::min((int)std::abs(((bigNoise * 6))), (int)biomes.size() - 1), 0);


			double objectBigNoise = p.noise((long double)(worldSeed + t.x) / 5.025, 129.253, (long double)(worldSeed + t.z) / 5.025);
			double objectNoise = p.noise((long double)(worldSeed + t.x) / 0.225, 60.253, (long double)(worldSeed + t.z) / .225)*5 - objectBigNoise;

			Biome& biome = biomes[biomeID];
			for (intTup& lp : loadPen)
			{
				int localX = realX + x + lp.x;
				int localZ = realZ + z + lp.z;

				intTup tup(localX, localZ);
				double noise = World::getTerrainNoise(localX, localZ);
				n = noise;
				
				HeightTile h;
				h.height = (float)noise;
				h.blockID = biome.blockIdFunction((float)noise);

				this->heights.insert_or_assign(tup, h);
			}

			if (std::abs(objectNoise) < (0.003 * biome.objectFrequency) && n > Game::instance->waterHeight)
			{
				Model m = nextModel(t.x, n, t.z, biome);
				this->models.insert_or_assign(t, m);
			} else
			if (n < Game::instance->waterHeight && std::abs( objectNoise) < (0.003 * biome.objectFrequency) && Game::instance->waterHeight-n < 5)
			{
				Model m = Plant::getCatTailsModel(t.x, n, t.z);
				this->models.insert_or_assign(t, m);
			}
		}
	}
	return blockCount;
}