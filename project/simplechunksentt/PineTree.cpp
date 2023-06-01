#include "Tree.hpp"
#include "Maths.hpp"
#include "BlockTypes.hpp"
Model Tree::getPineTreeModel(float x, float y, float z)
{
	Model model;

	float height = 7.0f + (rando() * 5.0f);

	glm::vec3 centerBottom(x + 0.5, y-1.0, z + 0.5);

	float leavesStart = height * 0.35;



	TextureFace logFace(10, 0);
	model.verts.insert(model.verts.begin(), {
			centerBottom.x-0.3f, centerBottom.y, centerBottom.z-0.3f,
			centerBottom.x + 0.3f, centerBottom.y, centerBottom.z - 0.3f,
			centerBottom.x, centerBottom.y+height, centerBottom.z,

			centerBottom.x + 0.3f, centerBottom.y, centerBottom.z - 0.3f,
			centerBottom.x + 0.3f, centerBottom.y, centerBottom.z + 0.3f,
			centerBottom.x, centerBottom.y + height, centerBottom.z,

			centerBottom.x + 0.3f, centerBottom.y, centerBottom.z + 0.3f,
			centerBottom.x - 0.3f, centerBottom.y, centerBottom.z + 0.3f,
			centerBottom.x, centerBottom.y + height, centerBottom.z,

			centerBottom.x - 0.3f, centerBottom.y, centerBottom.z + 0.3f,
			centerBottom.x - 0.3f, centerBottom.y, centerBottom.z - 0.3f,
			centerBottom.x, centerBottom.y + height, centerBottom.z,
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
		logFace.bl.x, logFace.bl.y, 
		logFace.br.x, logFace.br.y, 
		logFace.tl.x, logFace.tl.y,

		logFace.bl.x, logFace.bl.y,
		logFace.br.x, logFace.br.y,
		logFace.tl.x, logFace.tl.y,

		logFace.bl.x, logFace.bl.y,
		logFace.br.x, logFace.br.y,
		logFace.tl.x, logFace.tl.y,

		logFace.bl.x, logFace.bl.y,
		logFace.br.x, logFace.br.y,
		logFace.tl.x, logFace.tl.y,
		});
	return model;
}