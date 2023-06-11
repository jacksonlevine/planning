#include "Tree.hpp"

Model General::getCrossModel(float x, float y, float z, TextureFace face)
{
	Model model;

	glm::vec3 frontLeft(x, y, z);
	glm::vec3 frontRight(x + 1, y, z);
	glm::vec3 backLeft(x, y, z + 1);
	glm::vec3 backRight(x + 1, y, z + 1);

	float flHeight = Game::instance->world.heights.at(intTup((int)frontLeft.x, (int)frontLeft.z)).height;
	float frHeight = Game::instance->world.heights.at(intTup((int)frontLeft.x + 1, (int)frontLeft.z)).height;
	float blHeight = Game::instance->world.heights.at(intTup((int)frontLeft.x, (int)frontLeft.z + 1)).height;
	float brHeight = Game::instance->world.heights.at(intTup((int)frontLeft.x + 1, (int)frontLeft.z + 1)).height;

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