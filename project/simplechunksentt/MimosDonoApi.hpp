class MimosDonoApi;
#include "Game.hpp"

class MimosDonoApi
{
private:
	Game& game;
public:
	void setClientPosition(float x, float y, float z);
	std::tuple<float, float, float> getClientPosition();
};