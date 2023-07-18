#pragma once
class SimpleChunk;
class ZeroChunk;
#include "Game.hpp"
class SimpleChunk {
public:
	int x;
	int z;
	entt::entity me;
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

class ZeroChunk {
public:
	int x;
	int z;
	entt::entity me;
	std::vector<GLfloat> verts;

	std::vector<GLfloat> cols;

	std::vector<GLfloat> uvs;
	int length;
	bool active;
	Game* gref;
	ZeroChunk(Game* g);
	bool operator==(const intTup& other) const;
	void rebuildMesh();
	void moveAndRebuildMesh(int x, int z);
};