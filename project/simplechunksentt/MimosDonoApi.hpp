class MimosDonoApi;
#include "Game.hpp"
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <python3.10/Python.h>

class MimosDonoApi
{
private:
	Game& game;
public:
	MimosDonoApi(Game& g);
	void setClientPosition(float x, float y, float z);
	std::tuple<float, float, float> getClientPosition();
	void _initializePython();
	static std::shared_ptr<MimosDonoApi> instance;
};