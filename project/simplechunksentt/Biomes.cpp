#include "Biomes.hpp"

Biome::Biome(
	std::vector<std::function<Model(float, float, float)>> oh,
	std::function<uint8_t(float)> bidf
) :
	objectsHere(oh),
	blockIdFunction(bidf)
{

}