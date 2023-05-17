#include "Game.h"

Game* Game::instance = nullptr;

bool intTup::operator==(const intTup& other) const {
	return (x == other.x) && (y == other.y) && (z == other.z);
}

intTup::intTup(int x, int y, int z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

IntervalTask::IntervalTask(float interval, std::function<void(Game* g)> lambda, uint8_t id) : interval(interval), lambda(lambda), id(id), timer(0.0f) {

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
int Game::compareChunks(const void* a, const void* b)
{
	if (instance) {

		const Chunk* chunkA = static_cast<const Chunk*>(a);
		const Chunk* chunkB = static_cast<const Chunk*>(b);

		float distanceA = glm::distance(glm::vec3(
			chunkA->x*CHUNK_WIDTH, 
			chunkA->y * CHUNK_WIDTH, 
			chunkA->z * CHUNK_WIDTH) , instance->wrap->cameraPos);
		float distanceB = glm::distance(glm::vec3(
			chunkB->x * CHUNK_WIDTH, 
			chunkB->y * CHUNK_WIDTH, 
			chunkB->z * CHUNK_WIDTH) , instance->wrap->cameraPos);
		if (distanceA > 100 || distanceB > 100) {

			if (distanceA < distanceB)
				return 1;
			else if (distanceA > distanceB)
				return -1;
			else
				return 0;
		}
		return 0;
	}
	return 0;
}
void Game::sortChunkPool() {
	if (this->chunkPool.size() > 0) {
		std::qsort(&(this->chunkPool[0]), this->chunkPool.size(), sizeof(Chunk), this->compareChunks);
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
	for (int i = 0; i < 250; i++)
	{
		Chunk c(this);
		this->chunkPool.push_back(c);
	}
	instance = this;
}

void Game::surveyNeededChunks()
{
	glm::vec3 dir = this->wrap->cameraDirection;
	dir.y = 0;

	int zSkew = (dir.z == 0 ? 0.5 : dir.z) * 2 * 4;
	int xSkew = (dir.x == 0 ? 0.5 : dir.x) * 2 * 4;
	int ySkew = (dir.y == 0 ? 0.5 : dir.y) * 4;
	//std::cout << "surveying";
	int x = this->wrap->cameraPos.x;

	int y = this->wrap->cameraPos.y;

	int z = this->wrap->cameraPos.z;

	int chunkX = std::floor(x / CHUNK_WIDTH);
	int chunkY = std::floor(y / CHUNK_WIDTH);
	int chunkZ = std::floor(z / CHUNK_WIDTH);
	int diryn = ySkew > 1 ? 1 : std::round(std::abs(ySkew));
	int diryp = ySkew < 1 ? 1 : std::round(ySkew);
	for(int j = chunkY - diryn ; j < chunkY + diryp; j++)
	{

		int dirxn = xSkew > 0 ? 1 : std::round(std::abs(xSkew));
		int dirxp = xSkew < 0 ? 1 : std::round(xSkew);
		for (int i = chunkX - dirxn; i < chunkX + dirxp; i++)
		{
			int dirzn = zSkew > 0 ? 1 : std::round(std::abs(zSkew));
			int dirzp = zSkew < 0 ? 1 : std::round(zSkew);
			for (int k = chunkZ - dirzn; k < chunkZ + dirzp; k++)
			{
				intTup tup(i, j, k);
				if (this->activeChunks.find(tup) == this->activeChunks.end())
				{
					if (this->world.isHandledMarks.find(tup) == this->world.isHandledMarks.end())
					{
						this->world.generateOneChunk(tup);
					}
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
			if (chunkPool.size() > 0)
			{
				Chunk grabbedChunk = *(this->chunkPool.begin());
				this->chunkPool.erase(this->chunkPool.begin());
				intTup grabbedSpot(grabbedChunk.x, grabbedChunk.y, grabbedChunk.z);
				if (this->activeChunks.find(grabbedSpot) != this->activeChunks.end())
				{
					activeChunks.erase(grabbedSpot);
				}

				glDeleteBuffers(1, &grabbedChunk.vbov);
				glDeleteBuffers(1, &grabbedChunk.vboc);
				grabbedChunk.moveAndRebuildMesh(neededSpot.x, neededSpot.y, neededSpot.z);
				if (grabbedChunk.vertices.size() && grabbedChunk.colors.size())
				{
					glGenBuffers(1, &grabbedChunk.vbov);
					glGenBuffers(1, &grabbedChunk.vboc);
				}

				activeChunks.insert_or_assign(neededSpot, grabbedChunk);
				this->chunkPool.insert(this->chunkPool.end(), grabbedChunk);
				
			}
		}
	}
}