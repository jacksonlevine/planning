#pragma once
#include "Game.hpp"

class SimpleChunk {
	int x;
	int z;
	GLuint vbov;
	GLuint vboc;
	GLuint vbouv;
	Game* gref;
	SimpleChunk(Game* g);
	void rebuildMesh();
	void moveAndRebuildMesh(int x, int z);
};