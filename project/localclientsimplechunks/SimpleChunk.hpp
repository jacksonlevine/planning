#pragma once
class SimpleChunk;
#include "Game.hpp"

class SimpleChunk {
public:
	int x;
	int z;
	GLuint vbov;
	GLuint vboc;
	GLuint vbouv;
	std::vector<GLfloat> verts;

	std::vector<GLfloat> cols;

	std::vector<GLfloat> uvs;
	int length;
	bool active;
	Game* gref;
	SimpleChunk(Game* g);
	bool operator==(const intTup& other) const;
	void rebuildMesh();
	void moveAndRebuildMesh(int x, int z);
};