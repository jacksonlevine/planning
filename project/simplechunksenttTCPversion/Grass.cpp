#include "Tree.hpp"
#include "BlockTypes.hpp"
#include "Maths.hpp"
intTup grassSpots[] = {
	intTup(0,0),
	intTup(1,0),
	intTup(2,0),
	intTup(1,1),
	intTup(2,1),
	intTup(-1,0),
	intTup(-2,0),
	intTup(-1,1),
	intTup(-2,1),
	intTup(-3,0),
	intTup(3,0),
	intTup(-1,2),
	intTup(-2,2),
	intTup(-1,-1),
	intTup(-2,-1),
	intTup(-3,-1),
	intTup(3,1)
};

Model Plant::getTallGrassModel(float x, float y, float z)
{
	Model model;
	for (intTup& i : grassSpots)
	{
		if (rando() < 0.5)
		{
			Model tuft = General::getCrossModel(x + i.x + 5, y, z + i.z - 3, TextureFace(6, 2), 0, false);
			model.verts.insert(model.verts.end(), tuft.verts.begin(), tuft.verts.end());
			model.cols.insert(model.cols.end(), tuft.cols.begin(), tuft.cols.end());
			model.uvs.insert(model.uvs.end(), tuft.uvs.begin(), tuft.uvs.end());
		}
		else
		{
			Model tuft = General::getTallCrossModel(x + i.x, y, z + i.z, TextureFace(5, 2), TextureFace(5, 3), 0, false);
			model.verts.insert(model.verts.end(), tuft.verts.begin(), tuft.verts.end());
			model.cols.insert(model.cols.end(), tuft.cols.begin(), tuft.cols.end());
			model.uvs.insert(model.uvs.end(), tuft.uvs.begin(), tuft.uvs.end());
		}
	}

	return model;
}

Model Plant::getDeadTallGrassModel(float x, float y, float z)
{
	Model model;
	for (intTup& i : grassSpots)
	{
		if (rando() < 0.5)
		{
			Model tuft = General::getCrossModel(x + i.x + 5, y, z + i.z - 3, TextureFace(9, 2), 0, false);
			model.verts.insert(model.verts.end(), tuft.verts.begin(), tuft.verts.end());
			model.cols.insert(model.cols.end(), tuft.cols.begin(), tuft.cols.end());
			model.uvs.insert(model.uvs.end(), tuft.uvs.begin(), tuft.uvs.end());
		}
		else
		{
			Model tuft = General::getTallCrossModel(x + i.x, y, z + i.z, TextureFace(8, 2), TextureFace(8, 3), 0, false);
			model.verts.insert(model.verts.end(), tuft.verts.begin(), tuft.verts.end());
			model.cols.insert(model.cols.end(), tuft.cols.begin(), tuft.cols.end());
			model.uvs.insert(model.uvs.end(), tuft.uvs.begin(), tuft.uvs.end());
		}
	}

	return model;
}

Model Plant::getDeadShortGrassModel(float x, float y, float z)
{
	Model model;
	for (intTup& i : grassSpots)
	{
		if (rando() < 0.5)
		{
			Model tuft = General::getCrossModel(x + i.x + 5, y, z + i.z - 3, TextureFace(9, 2), 0, false);
			model.verts.insert(model.verts.end(), tuft.verts.begin(), tuft.verts.end());
			model.cols.insert(model.cols.end(), tuft.cols.begin(), tuft.cols.end());
			model.uvs.insert(model.uvs.end(), tuft.uvs.begin(), tuft.uvs.end());
		}
		else
		{
			Model tuft = General::getCrossModel(x + i.x, y, z + i.z, TextureFace(9, 2), 0, false);
			model.verts.insert(model.verts.end(), tuft.verts.begin(), tuft.verts.end());
			model.cols.insert(model.cols.end(), tuft.cols.begin(), tuft.cols.end());
			model.uvs.insert(model.uvs.end(), tuft.uvs.begin(), tuft.uvs.end());
		}
	}

	return model;
}

Model Plant::getShortGrassModel(float x, float y, float z)
{
	Model model;
	for (intTup& i : grassSpots)
	{
		if (rando() < 0.5)
		{
			Model tuft = General::getCrossModel(x + i.x + 5, y, z + i.z - 3, TextureFace(6, 2), 0, false);
			model.verts.insert(model.verts.end(), tuft.verts.begin(), tuft.verts.end());
			model.cols.insert(model.cols.end(), tuft.cols.begin(), tuft.cols.end());
			model.uvs.insert(model.uvs.end(), tuft.uvs.begin(), tuft.uvs.end());
		}
		else
		{
			Model tuft = General::getCrossModel(x + i.x, y, z + i.z, TextureFace(6, 2), 0, false);
			model.verts.insert(model.verts.end(), tuft.verts.begin(), tuft.verts.end());
			model.cols.insert(model.cols.end(), tuft.cols.begin(), tuft.cols.end());
			model.uvs.insert(model.uvs.end(), tuft.uvs.begin(), tuft.uvs.end());
		}
	}

	return model;
}