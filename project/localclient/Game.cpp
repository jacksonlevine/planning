#include "Game.h"

bool intTup::operator==(const intTup& other) const {
	return (x == other.x) && (y == other.y) && (z == other.z);
}

intTup::intTup(int x, int y, int z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Game::Game(GLWrapper* w) : wrap(w), chunkWidth(16) {
	for (int i = 0; i < 50; i++)
	{
		Chunk c(this);
		this->chunkPool.push_back(c);
	}
}