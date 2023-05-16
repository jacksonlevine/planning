#include "Game.h"

Game::Game() : chunkWidth(16) {
	for (int i = 0; i < 50; i++)
	{
		Chunk c(this->chunkWidth);
		this->chunkPool.push_back(c);
	}
}