#include "Tree.hpp"
#include "BlockTypes.hpp"
Model Plant::getShortGrassModel(float x, float y, float z)
{
	return General::getCrossModel(x, y, z, TextureFace(6, 2));
}