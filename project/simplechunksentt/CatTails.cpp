#include "Tree.hpp"
#include "Maths.hpp"

intTup grassSpots2[] = {
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


Model Plant::getCatTailsModel(float x, float y, float z)
{
	Model model;

	TextureFace topFace(0, 3);
	TextureFace bottomFace(0, 2);

	for (intTup& i : grassSpots2)
	{
		int wh = Game::instance->waterHeight;

		if (rando() < 0.5)
		{
			Model tuft = General::getCrossModel(x + i.x, y, z + i.z, topFace, 0.5, true);
			model.verts.insert(model.verts.end(), tuft.verts.begin(), tuft.verts.end());
			model.cols.insert(model.cols.end(), tuft.cols.begin(), tuft.cols.end());
			model.uvs.insert(model.uvs.end(), tuft.uvs.begin(), tuft.uvs.end());
		}
		else
		{
			Model tuft = General::getTallCrossModel(x + i.x, y, z + i.z, bottomFace, topFace, 0.5, true);
			model.verts.insert(model.verts.end(), tuft.verts.begin(), tuft.verts.end());
			model.cols.insert(model.cols.end(), tuft.cols.begin(), tuft.cols.end());
			model.uvs.insert(model.uvs.end(), tuft.uvs.begin(), tuft.uvs.end());
		}

		glm::vec3 frontLeft(x + i.x, wh, z + i.z);
		glm::vec3 frontRight(x + i.x + 1, wh, z + i.z);
		glm::vec3 backLeft(x + i.x, wh, z + i.z + 1);
		glm::vec3 backRight(x + i.x + 1, wh, z + i.z + 1);

		model.verts.insert(model.verts.end(), {
				frontLeft.x, frontLeft.y, frontLeft.z,
				backRight.x, backRight.y, backRight.z,
				backRight.x, backRight.y - 20, backRight.z,

				backRight.x, backRight.y - 20, backRight.z,
				frontLeft.x, frontLeft.y - 20, frontLeft.z,
				frontLeft.x, frontLeft.y, frontLeft.z,

				frontRight.x, frontRight.y, frontRight.z, 
				backLeft.x, backLeft.y, backLeft.z,
				backLeft.x, backLeft.y - 20, backLeft.z,

				backLeft.x, backLeft.y - 20, backLeft.z,
				frontRight.x, frontRight.y - 20, frontRight.z,
				frontRight.x, frontRight.y, frontRight.z
			});

		model.cols.insert(model.cols.end(), {
				1.0, 1.0, 1.0,
				1.0, 1.0, 1.0,
				1.0, 1.0, 1.0,

				1.0, 1.0, 1.0,
				1.0, 1.0, 1.0,
				1.0, 1.0, 1.0,

				1.0, 1.0, 1.0,
				1.0, 1.0, 1.0,
				1.0, 1.0, 1.0,

				1.0, 1.0, 1.0,
				1.0, 1.0, 1.0,
				1.0, 1.0, 1.0
			});

		model.uvs.insert(model.uvs.end(), {
				bottomFace.bl.x, bottomFace.bl.y,
				bottomFace.br.x, bottomFace.br.y,
				bottomFace.tr.x, bottomFace.tr.y,

				bottomFace.tr.x, bottomFace.tr.y,
				bottomFace.br.x, bottomFace.br.y,
				bottomFace.bl.x, bottomFace.bl.y,

				bottomFace.bl.x, bottomFace.bl.y,
				bottomFace.br.x, bottomFace.br.y,
				bottomFace.tr.x, bottomFace.tr.y,

				bottomFace.tr.x, bottomFace.tr.y,
				bottomFace.br.x, bottomFace.br.y,
				bottomFace.bl.x, bottomFace.bl.y
			});
	}

	return model;
}