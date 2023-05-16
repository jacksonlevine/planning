#include "Game.h"

Game::Game(GLWrapper* w) : wrap(w), chunkWidth(16) {
	for (int i = 0; i < 50; i++)
	{
		Chunk c(this);
		this->chunkPool.push_back(c);
	}
}