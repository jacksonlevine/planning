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

IntervalTask::IntervalTask(float interval, std::function<void()> lambda, uint8_t id) : interval(interval), lambda(lambda), id(id) {

}

void Game::updateTasks(float delt)
{
	for (IntervalTask task : this->tasks) {
		if (task.timer < task.interval)
		{
			task.timer += delt;
		}
		else {
			task.lambda();
			task.timer = 0;
		}
	}
}

void Game::addTask(std::function<void()> func, float interval, uint8_t id)
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

Game::Game(World w) : world(w), chunkWidth(16) {
	for (int i = 0; i < 50; i++)
	{
		Chunk c(this);
		this->chunkPool.push_back(c);
	}
}