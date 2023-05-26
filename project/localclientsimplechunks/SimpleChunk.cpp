#pragma once
#include "SimpleChunk.hpp"

SimpleChunk::SimpleChunk(Game* g) :
	gref(g),
	x(50000),
	z(50000)
{

}

void SimpleChunk::rebuildMesh()
{
	std::vector<GLfloat> verts;
	std::vector<GLfloat> cols;
	std::vector<GLfloat> uvs;
	std::unordered_map<intTup, glm::vec3, intTupHash> localHeights;

	for (int i = -1; i < CHUNK_WIDTH+1; i++)
	{
		for (int k = -1; k < CHUNK_WIDTH+1; k++)
		{
			intTup tup((this->x * CHUNK_WIDTH) + i, (this->z * CHUNK_WIDTH) + k);
			if (this->gref->world.heights.find(tup) != this->gref->world.heights.end())
			{
				localHeights.insert_or_assign(tup, glm::vec3(tup.x, this->gref->world.heights.at(tup), tup.z));
			}	
			else {
				this->gref->world.generateOneChunk(intTup(this->x, this->z));
				this->rebuildMesh();
			}
		}
	}

	for (int i = 0; i < CHUNK_WIDTH; i++)
	{
		for (int k = 0; k < CHUNK_WIDTH; k++)
		{
			intTup tup((this->x * CHUNK_WIDTH) + i, (this->z * CHUNK_WIDTH) + k);

			verts.insert(verts.end(), {
				(float)(tup.x), localHeights.at(tup).y, (float)(tup.z),
				(float)(tup.x) + 1, localHeights.at(tup + intTup(1,0)).y, (float)(tup.z),
				(float)(tup.x) + 1, localHeights.at(tup + intTup(1,1)).y, (float)(tup.z) + 1,

				(float)(tup.x) + 1, localHeights.at(tup + intTup(1,1)).y, (float)(tup.z) + 1,
				(float)(tup.x), localHeights.at(tup + intTup(0,1)).y, (float)(tup.z) + 1,
				(float)(tup.x), localHeights.at(tup).y, (float)(tup.z),
				});
		}
	}
}

void SimpleChunk::moveAndRebuildMesh(int x, int z) {
	this->x = x;
	this->z = z;
	this->rebuildMesh();
}