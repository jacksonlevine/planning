#include "Tree.hpp"
#include "Maths.hpp"

Model Plant::getBlackberryBushModel(float x, float y, float z)
{
	Model model;

	TextureFace leftTop(2, 3);
	TextureFace leftBottom(2, 2);

	TextureFace middleTop(3, 3);
	TextureFace middleBottom(3, 2);

	TextureFace rightTop(4, 3);
	TextureFace rightBottom(4, 2);



	int width = 4 * rando() + (rando() * 2);
	int depth = 4 * rando() + (rando() * 2);

	glm::vec3 frontLeft(x, y, z);
	glm::vec3 backLeft(x, y, z+depth);

	glm::vec3 leanBack(0, 1, -0.4);
	glm::vec3 leanForward(0, 1, 0.4);

	model.verts.insert(model.verts.end(), {
			frontLeft.x,frontLeft.y,frontLeft.z,
			frontLeft.x + 1,frontLeft.y,frontLeft.z,
			frontLeft.x + 1 + leanForward.x,frontLeft.y + leanForward.y,frontLeft.z + leanForward.z,

			frontLeft.x + 1 + leanForward.x,frontLeft.y + leanForward.y,frontLeft.z + leanForward.z,
			frontLeft.x + leanForward.x,frontLeft.y + leanForward.y,frontLeft.z + leanForward.z,
			frontLeft.x,frontLeft.y,frontLeft.z,

			backLeft.x + leanBack.x, backLeft.y + leanBack.y, backLeft.z + leanBack.z,
			backLeft.x - 1 + leanBack.x,backLeft.y + leanBack.y,backLeft.z + leanBack.z,
			backLeft.x - 1 + leanBack.x*2,backLeft.y + leanBack.y*2,backLeft.z + leanBack.z*2,

			backLeft.x - 1 + leanBack.x*2,backLeft.y + leanBack.y*2,backLeft.z + leanBack.z*2,
			backLeft.x + leanBack.x * 2,backLeft.y + leanBack.y * 2,backLeft.z + leanBack.z * 2,
			backLeft.x + leanBack.x,backLeft.y + leanBack.y,backLeft.z + leanBack.z
		});
	model.uvs.insert(model.uvs.end(), {
			leftBottom.tl.x, leftBottom.tl.y,
			leftBottom.tr.x, leftBottom.tr.y,
			leftBottom.br.x, leftBottom.br.y,

			leftBottom.br.x, leftBottom.br.y,
			leftBottom.bl.x, leftBottom.bl.y,
			leftBottom.tl.x, leftBottom.tl.y,

			leftTop.tl.x, leftTop.tl.y,
			leftTop.tr.x, leftTop.tr.y,
			leftTop.br.x, leftTop.br.y,

			leftTop.br.x, leftTop.br.y,
			leftTop.bl.x, leftTop.bl.y,
			leftTop.tl.x, leftTop.tl.y
		});

	model.verts.insert(model.cols.end(), {
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



	for (int i = 0; i < width; i++)
	{
		
	}

	return model;
}