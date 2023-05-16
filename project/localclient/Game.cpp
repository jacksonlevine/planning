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

IntervalTask::IntervalTask(float interval, std::function<void()> lambda) : interval(interval), lambda(lambda) {

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

Game::Game(World w) : world(w), chunkWidth(16) {
	for (int i = 0; i < 50; i++)
	{
		Chunk c(this);
		this->chunkPool.push_back(c);
	}
}