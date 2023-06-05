#pragma once
#include "Game.hpp"
#include "Model.hpp"
#include "glm/glm.hpp"
Game* Game::instance = nullptr;

bool intTup::operator==(const intTup& other) const {
	return (x == other.x) && (y == other.y) && (z == other.z);
}

bool intTup::operator==(const SimpleChunk* other) const {
	return (x == other->x) && (z == other->z);
}

intTup& intTup::operator+=(const intTup& other)
{
	x += other.x;
	y += other.y;
	z += other.z;

	return *this;
}

intTup operator+(intTup first, // parameter as value, move-construct (or elide)
	const intTup& second)
{
	first.x += second.x;
	first.y += second.y;
	first.z += second.z;

	return first; // NRVO (or move-construct)
}

intTup::intTup(int x, int y, int z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

intTup::intTup(int x, int z)
{
	this->x = x;
	this->y = 0;
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
int Game::compareChunksInPool(const void* a, const void* b)
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


			if (distanceA < distanceB)
				return 1;
			else if (distanceA > distanceB)
				return -1;
			else
				return 0;

	}
	return 0;
}

intTup Game::castRayBlocking(float x, float y, float z, glm::vec3 d, float maxDistance, float stepBack) {
	float distanceTraveled = 0;
	float currentX = x;
	float currentY = y;
	float currentZ = z;

	while (distanceTraveled < maxDistance) {
		currentX += d.x * 0.05;
		currentY += d.y * 0.05;
		currentZ += d.z * 0.05;
		distanceTraveled += 0.05;
	
		int blockX = currentX >= 0 ? (int)std::floor(currentX) : (int)std::ceil(currentX) - 1;
		int blockY = currentY >= 0 ? (int)std::floor(currentY) : (int)std::ceil(currentY) - 1;
		int blockZ = currentZ >= 0 ? (int)std::floor(currentZ) : (int)std::ceil(currentZ) - 1;
			intTup key(blockX, blockZ);
			intTup key1(blockX, blockY, blockZ);
	if (this->world.heights.find(key) != this->world.heights.end()) {
		if (this->world.heights.at(key).height >= currentY)
		{
			int blockX2 = currentX >= 0 ? (int)std::floor(currentX) : (int)std::ceil(currentX) - 1;
			int blockY2 = currentY + stepBack >= 0 ? (int)std::floor(currentY + stepBack) : (int)std::ceil(currentY + stepBack) - 1;
			int blockZ2 = currentZ >= 0 ? (int)std::floor(currentZ) : (int)std::ceil(currentZ) - 1;
			return intTup(blockX2, blockY2, blockZ2);
		}
	}
	if (this->world.data.find(key1) != this->world.data.end())
	{
		int blockX2 = currentX - d.x * stepBack >= 0 ? (int)std::floor(currentX - d.x * stepBack) : (int)std::ceil(currentX - d.x * stepBack) - 1;
		int blockY2 = currentY - d.y * stepBack >= 0 ? (int)std::floor(currentY - d.y * stepBack) : (int)std::ceil(currentY - d.y * stepBack) - 1;
		int blockZ2 = currentZ - d.z * stepBack >= 0 ? (int)std::floor(currentZ - d.z * stepBack) : (int)std::ceil(currentZ - d.z * stepBack) - 1;
		return intTup(blockX2, blockY2, blockZ2);
	}
	}

	return intTup(-69000,69000,0);
};

void Game::placeBlock(intTup spot, uint8_t blockID)
{
	int chunkX = spot.x < 0 ? ((int)spot.x - CHUNK_WIDTH + 1) / CHUNK_WIDTH : (int)spot.x / CHUNK_WIDTH;
	int chunkY = spot.y < 0 ? ((int)spot.y - CHUNK_WIDTH + 1) / CHUNK_WIDTH : (int)spot.y / CHUNK_WIDTH;
	int chunkZ = spot.z < 0 ? ((int)spot.z - CHUNK_WIDTH + 1) / CHUNK_WIDTH : (int)spot.z / CHUNK_WIDTH;
	intTup chunkSpot(chunkX, chunkY, chunkZ);

	this->world.data.insert_or_assign(spot, blockID);
	this->world.hasBlockMarks.insert_or_assign(chunkSpot, 1);

	if (this->activeChunks.find(chunkSpot) == this->activeChunks.end())
	{

			if (chunkPool.size() > 0)
			{
				Chunk grabbedChunk = *(this->chunkPool.begin());
				this->chunkPool.erase(this->chunkPool.begin());
				intTup grabbedSpot(grabbedChunk.x, grabbedChunk.y, grabbedChunk.z);
				if (grabbedChunk.active == true)
				{
					activeChunks.erase(grabbedSpot);
					grabbedChunk.active = false;
				}


				//grabbedChunk.bufferDeleted = true;
				grabbedChunk.moveAndRebuildMesh(chunkSpot.x, chunkSpot.y, chunkSpot.z);
				//activeChunks.insert_or_assign(neededSpot, grabbedChunk);
				this->chunkPool.insert(this->chunkPool.end(), grabbedChunk);

			}


	}
	else {
		this->activeChunks.at(chunkSpot).rebuildMesh();
	}
}

void Game::onRightClick() {
	glm::vec3& pos = this->wrap->cameraPos;
	intTup result = this->castRayBlocking(pos.x, pos.y, pos.z, glm::normalize(wrap->cameraDirection), 7.0f, 0.08f);
	/*std::cout << "   x";
	std::cout << result.x;
	std::cout << "   y";
	std::cout << result.y;*/
	if (!(result == intTup(-69000, 69000, 0)))
	{
		placeBlock(result, 1);
	}
}

void Game::sortChunkPool() {
	if (this->chunkPool.size() > 0) {
		std::qsort(&(this->chunkPool[0]), this->chunkPool.size(), sizeof(Chunk), this->compareChunksInPool);
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
	for (int i = 0; i < 2000; i++)
	{
		ModelShower m(this);
		this->modelShowerPool.push_back(m);
	}
	for (int i = 0; i < 1500; i++)
	{
		SimpleChunk s(this);
		this->simpleChunkPool.push_back(s);
	}
	instance = this;
}

void Game::initialSurvey()
{

	int x = this->wrap->cameraPos.x;

	int y = this->wrap->cameraPos.y;

	int z = this->wrap->cameraPos.z;

	int chunkX = std::floor((x + (CHUNK_WIDTH - (x % CHUNK_WIDTH))) / CHUNK_WIDTH);
	int chunkY = std::floor((y - (y % CHUNK_WIDTH)) / CHUNK_WIDTH);
	int chunkZ = std::floor((z + (CHUNK_WIDTH - (z % CHUNK_WIDTH))) / CHUNK_WIDTH);
	int j = 0;


	for (int i = chunkX - 20; i < chunkX + 20; i++)
	{

		for (int k = chunkZ - 20; k < chunkZ + 20; k++)
		{
			intTup tup(i, j, k);

			//Generate
			if (this->world.hasSimpMarks.find(tup) == this->world.hasSimpMarks.end())
			{
				this->world.generateOneChunk(tup);
			}
			if (this->activeSimpChunks.find(tup) == this->activeSimpChunks.end())
			{
				if (simpleChunkPool.size() > 5) {
					SimpleChunk grabbedSimp = *(this->simpleChunkPool.begin());
					this->simpleChunkPool.erase(this->simpleChunkPool.begin());

					if (this->activeSimpChunks.find(intTup(grabbedSimp.x, grabbedSimp.z)) != this->activeSimpChunks.end())
					{
						this->activeSimpChunks.erase(intTup(grabbedSimp.x, grabbedSimp.z));
					}

					grabbedSimp.active = false;

					grabbedSimp.moveAndRebuildMesh(tup.x, tup.z);

					this->simpleChunkPool.push_back(grabbedSimp);
				}
			}

			if (this->activeChunks.find(tup) == this->activeChunks.end())
			{


				//Or mesh
				if (this->neededChunks.find(tup) == this->neededChunks.end() && this->world.hasBlockMarks.find(tup) != this->world.hasBlockMarks.end())
				{
					int head = 0;
					int maxLoadUp = 8;
					intTup test2(tup.x, tup.y - 1, tup.z);
					if (this->neededChunks.find(test2) == this->neededChunks.end() && this->world.hasBlockMarks.find(test2) != this->world.hasBlockMarks.end())
					{
						this->neededChunks.insert(test2);
					}
					intTup test(tup.x, tup.y + head, tup.z);
					while (this->neededChunks.find(test) == this->neededChunks.end() && this->world.hasBlockMarks.find(test) != this->world.hasBlockMarks.end() && head < maxLoadUp)
					{
						this->neededChunks.insert(test);
						head += 1;
						test = intTup(tup.x, tup.y + head, tup.z);
					}
				}

			}
		}
	}
}

void Game::surveyNeededChunks()
{
	glm::vec3 dir = this->wrap->cameraDirection;
	dir.y = 0;

	int zSkew = (dir.z) * 13.0f;
	int xSkew = (dir.x) * 13.0f;
	//std::cout << "surveying";
	int x = this->wrap->cameraPos.x;

	int y = this->wrap->cameraPos.y;

	int z = this->wrap->cameraPos.z;

	int chunkX = std::floor((x + (CHUNK_WIDTH - (x % CHUNK_WIDTH))) / CHUNK_WIDTH);
	int chunkY = std::floor((y  - (y % CHUNK_WIDTH)) / CHUNK_WIDTH);
	int chunkZ = std::floor((z + (CHUNK_WIDTH - (z % CHUNK_WIDTH))) / CHUNK_WIDTH);
	int j = 0;
	

		int dirxn = std::floor(xSkew > 0 ? 1 : std::round(std::abs(xSkew)));
		int dirxp = std::floor(xSkew < 0 ? 1 : std::round(xSkew));
		for (int i = chunkX - dirxn - 1; i < chunkX + dirxp + 1; i++)
		{
			int dirzn = std::floor(zSkew > 0 ? 1 : std::round(std::abs(zSkew)));
			int dirzp = std::floor(zSkew < 0 ? 1 : std::round(zSkew));
			for (int k = chunkZ - dirzn - 1; k < chunkZ + dirzp + 1; k++)
			{
				intTup tup(i, j, k);

				//Generate
				if (this->world.hasSimpMarks.find(tup) == this->world.hasSimpMarks.end())
				{
					this->world.generateOneChunk(tup);
				}
				if (this->activeSimpChunks.find(tup) == this->activeSimpChunks.end())
				{
					if (simpleChunkPool.size() > 5) {
						SimpleChunk grabbedSimp = *(this->simpleChunkPool.begin());

						if (this->activeSimpChunks.find(intTup(grabbedSimp.x, grabbedSimp.z)) != this->activeSimpChunks.end())
						{
							this->activeSimpChunks.erase(intTup(grabbedSimp.x, grabbedSimp.z));
						}

						grabbedSimp.active = false;
						this->simpleChunkPool.erase(this->simpleChunkPool.begin());

						grabbedSimp.moveAndRebuildMesh(tup.x, tup.z);

						this->simpleChunkPool.push_back(grabbedSimp);
					}
				}

				if (this->activeChunks.find(tup) == this->activeChunks.end())
				{
					
					
					//Or mesh
					if (this->neededChunks.find(tup) == this->neededChunks.end() && this->world.hasBlockMarks.find(tup) != this->world.hasBlockMarks.end())
					{
						int head = 0;
						int maxLoadUp = 8;
						intTup test2(tup.x, tup.y - 1, tup.z);
						if (this->neededChunks.find(test2) == this->neededChunks.end() && this->world.hasBlockMarks.find(test2) != this->world.hasBlockMarks.end())
						{
							this->neededChunks.insert(test2);
						}
						intTup test(tup.x, tup.y + head, tup.z);
						while (this->neededChunks.find(test) == this->neededChunks.end() && this->world.hasBlockMarks.find(test) != this->world.hasBlockMarks.end() && head < maxLoadUp)
						{
							this->neededChunks.insert(test);
							head += 1;
							test = intTup(tup.x, tup.y + head, tup.z);
						}
					}
					
				}
			}
		}
}

void Game::rebuildNextChunk()
{
	if (this->neededChunks.size() > 0)
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
				if (grabbedChunk.active)
				{
					activeChunks.erase(grabbedSpot);
					grabbedChunk.active = false;
				}

			
				//grabbedChunk.bufferDeleted = true;
				grabbedChunk.moveAndRebuildMesh(neededSpot.x, neededSpot.y, neededSpot.z);

				//activeChunks.insert_or_assign(neededSpot, grabbedChunk);
				this->chunkPool.insert(this->chunkPool.end(), grabbedChunk);
				
			}
		}
	}
}