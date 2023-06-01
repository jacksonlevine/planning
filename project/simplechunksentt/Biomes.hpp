#pragma once
#include <functional>
#include "Model.hpp"
struct Biome
{
public:
	std::vector<std::function<Model(float, float, float)>> objectsHere;
	std::function<uint8_t(intTup, Game*)> blockIdFunction;
	Biome(
		std::vector<std::function<Model(float, float, float)>> oh,
		std::function<uint8_t(intTup, Game*)> bidf
	);
};