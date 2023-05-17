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

IntervalTask::IntervalTask(float interval, std::function<void(Game* g)> lambda, uint8_t id) : interval(interval), lambda(lambda), id(id) {

}

std::size_t intTupHash::operator()(const intTup& tup) const {
	std::size_t hash = 0;
	hash ^= std::hash<int>{}(tup.x) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	hash ^= std::hash<int>{}(tup.y) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	hash ^= std::hash<int>{}(tup.z) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	return hash;
}

void Game::updateTasks(float delt)
{
	for (IntervalTask& task : this->tasks) {
		if (task.timer <= task.interval)
		{
			task.timer += delt;
		}
		else {
			task.lambda(this);
			task.timer = 0;
		}
	}
}

void Game::addTask(std::function<void(Game* g)> func, float interval, uint8_t id)
{
	IntervalTask it(interval, func, id);
	this->tasks.push_back(it);
}

void Game::removeTask(uint8_t id)
{
	this->tasks.erase(std::remove_if(this->tasks.begin(), this->tasks.end(), 
		[id](const IntervalTask& task) {
		return task.id == id;
		}), this->tasks.end());
}

Game::Game(GLWrapper* wr) : wrap(wr), chunkWidth(CHUNK_WIDTH) {
	for (int i = 0; i < 500; i++)
	{
		Chunk c(this);
		this->chunkPool.push_back(c);
	}
}

void Game::surveyNeededChunks()
{
	std::cout << "surveying";
	int x = this->wrap->cameraPos.x;

	int y = this->wrap->cameraPos.y;

	int z = this->wrap->cameraPos.z;

	int chunkX = std::floor(x / CHUNK_WIDTH);
	int chunkY = std::floor(y / CHUNK_WIDTH);
	int chunkZ = std::floor(z / CHUNK_WIDTH);

	for(int j = -chunkY - 2; j < chunkY + 1; j++)
	{
		for (int i = chunkX - 5; i < chunkX + 5; i++)
		{
			for (int k = chunkZ - 5; k < chunkZ + 5; k++)
			{
				intTup tup(i, j, k);
				if (this->activeChunks.find(tup) == this->activeChunks.end())
				{
					if (this->neededChunks.find(tup) == this->neededChunks.end() && this->world.hasBlockMarks.find(tup) != this->world.hasBlockMarks.end())
					{
						this->neededChunks.insert(tup);
					}
				}
			}
		}
	}
}

void Game::rebuildNextChunk()
{
	if (this->neededChunks.size() > 5)
	{
		intTup neededSpot = *(this->neededChunks.begin());
		this->neededChunks.erase(this->neededChunks.begin());
		if (activeChunks.find(neededSpot) == activeChunks.end())
		{
			if (chunkPool.size() > 1)
			{
				Chunk grabbedChunk = *(this->chunkPool.begin());
				this->chunkPool.erase(this->chunkPool.begin());
				grabbedChunk.moveAndRebuildMesh(neededSpot.x, neededSpot.y, neededSpot.z);
				activeChunks.insert_or_assign(neededSpot, grabbedChunk);
				
			}
		}
	}
}