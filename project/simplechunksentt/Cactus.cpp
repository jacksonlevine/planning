#pragma once
#include "Tree.hpp"
#include "BlockTypes.hpp"
#include "Maths.hpp"
#include "glm/glm.hpp"
Model Plant::getCactusModel(float x, float y, float z)
{
	Model model;
	TextureFace cactusFace(13, 0);
	int segmentHeight = (int)(8.0f * rando());
	int posOrNeg = rando() < 0.5 ? -1 : 1;
	glm::vec3 centerBottom(x + 0.5, y, z + 0.5);
	glm::vec3 initDirection = glm::normalize(glm::vec3(rando() * posOrNeg, 0.0f, rando() * posOrNeg));

	for (int i = 0; i < segmentHeight; i++)
	{
		model.verts.insert(model.verts.end(), {
				centerBottom.x + initDirection.x*0.5f, centerBottom.y + i, centerBottom.z + initDirection.z * 0.5f,
				centerBottom.x - initDirection.x * 0.5f, centerBottom.y + i, centerBottom.z - initDirection.z * 0.5f,
				centerBottom.x - initDirection.x * 0.5f, centerBottom.y + i + 1.0f, centerBottom.z - initDirection.z * 0.5f,

				centerBottom.x - initDirection.x * 0.5f, centerBottom.y + i + 1.0f, centerBottom.z - initDirection.z * 0.5f,
				centerBottom.x + initDirection.x * 0.5f, centerBottom.y + i + 1.0f, centerBottom.z + initDirection.z * 0.5f,
				centerBottom.x + initDirection.x * 0.5f, centerBottom.y + i, centerBottom.z + initDirection.z * 0.5f
			});
		model.uvs.insert(model.uvs.end(), {
				cactusFace.tl.x, cactusFace.tl.y,
				cactusFace.tr.x, cactusFace.tr.y,
				cactusFace.br.x, cactusFace.br.y,

				cactusFace.br.x, cactusFace.br.y,
				cactusFace.bl.x, cactusFace.bl.y,
				cactusFace.tl.x, cactusFace.tl.y
			});
		model.cols.insert(model.cols.end(), {
				1.0, 1.0, 1.0,
				1.0, 1.0, 1.0,
				1.0, 1.0, 1.0,

				1.0, 1.0, 1.0,
				1.0, 1.0, 1.0,
				1.0, 1.0, 1.0,
			});
		initDirection = rotateVector(initDirection, 90.0f);  // Rotate by 90 degrees
	}
	return model;
}