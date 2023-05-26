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

	for (int i = 0; i < CHUNK_WIDTH; i++)
	{
		for (int k = 0; k < CHUNK_WIDTH; k++)
		{
			intTup tup((this->x * CHUNK_WIDTH) + i, (this->z * CHUNK_WIDTH) + k);
			if (this->gref->world.heights.find(tup) != this->gref->world.heights.end())
			{

			}	
			else {
				this->gref->world.generateOneChunk(intTup(this->x, this->z));
				this->rebuildMesh();
			}
		}
	}
}

void SimpleChunk::moveAndRebuildMesh(int x, int z) {
	this->x = x;
	this->z = z;
	this->rebuildMesh();
}