#include "Tree.hpp"

Model General::getTallCrossModel(float x, float y, float z, TextureFace bottom, TextureFace top, int yOff, bool flat)
{
	Model model;

			Model tuft = General::getCrossModel(x, y, z, bottom, 0 + yOff, flat);
			model.verts.insert(model.verts.end(), tuft.verts.begin(), tuft.verts.end());
			model.cols.insert(model.cols.end(), tuft.cols.begin(), tuft.cols.end());
			model.uvs.insert(model.uvs.end(), tuft.uvs.begin(), tuft.uvs.end());

			Model tuft2 = General::getCrossModel(x, y, z, top, 1 + yOff, flat);
			model.verts.insert(model.verts.end(), tuft2.verts.begin(), tuft2.verts.end());
			model.cols.insert(model.cols.end(), tuft2.cols.begin(), tuft2.cols.end());
			model.uvs.insert(model.uvs.end(), tuft2.uvs.begin(), tuft2.uvs.end());

	return model;
}

Model General::getCrossModel(float x, float y, float z, TextureFace face, int yOffset, bool flat)
{
	Model model;
	
	glm::vec3 frontLeft(x, y, z);
	glm::vec3 frontRight(x + 1, y, z);
	glm::vec3 backLeft(x, y, z + 1);
	glm::vec3 backRight(x + 1, y, z + 1);

	float flHeight = flat ? Game::instance->waterHeight + yOffset : World::getTerrainNoise((int)frontLeft.x, (int)frontLeft.z) + yOffset;
	float frHeight = flat ? flHeight : World::getTerrainNoise((int)frontLeft.x + 1, (int)frontLeft.z) + yOffset;
	float blHeight = flat ? flHeight : World::getTerrainNoise((int)frontLeft.x, (int)frontLeft.z + 1) + yOffset;
	float brHeight = flat ? flHeight : World::getTerrainNoise((int)frontLeft.x + 1, (int)frontLeft.z + 1) + yOffset;

	frontLeft.y = flHeight;
	frontRight.y = frHeight;
	backLeft.y = blHeight;
	backRight.y = brHeight;

	float height = 1.0;

	model.verts.insert(model.verts.end(), {
			frontLeft.x, frontLeft.y, frontLeft.z,
			backRight.x, backRight.y, backRight.z,
			backRight.x, backRight.y + height, backRight.z,

			backRight.x, backRight.y + height, backRight.z,
			frontLeft.x, frontLeft.y + height, frontLeft.z,
			frontLeft.x, frontLeft.y, frontLeft.z,

			frontRight.x, frontRight.y, frontRight.z,
			backLeft.x, backLeft.y, backLeft.z,
			backLeft.x, backLeft.y + height, backLeft.z,

			backLeft.x, backLeft.y + height, backLeft.z,
			frontRight.x, frontRight.y + height, frontRight.z,
			frontRight.x, frontRight.y, frontRight.z,
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
			1.0, 1.0, 1.0,
		});

	model.uvs.insert(model.uvs.end(), {
				face.tl.x, face.tl.y,
				face.tr.x, face.tr.y,
				face.br.x, face.br.y,

				face.br.x, face.br.y,
				face.bl.x, face.bl.y,
				face.tl.x, face.tl.y,

				face.tl.x, face.tl.y,
				face.tr.x, face.tr.y,
				face.br.x, face.br.y,

				face.br.x, face.br.y,
				face.bl.x, face.bl.y,
				face.tl.x, face.tl.y
		});

	return model;
}