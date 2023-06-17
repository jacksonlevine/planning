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
	glm::vec3 backLeft(x, y, z+depth+2);

	glm::vec3 frontRight(x+width+2, y, z);
	glm::vec3 backRight(x+width+2, y, z + depth + 2);

	glm::vec3 leanBack(0, 1, -0.4);
	glm::vec3 leanForward(0, 1, 0.4);
	glm::vec3 leanLeft(-0.4, 1, 0);
	glm::vec3 leanRight(0.4, 1, 0);
	//LEFT SIDE CORNERS 
	model.verts.insert(model.verts.end(), {
		    //front left, lean forward corner
			frontLeft.x,frontLeft.y,frontLeft.z,
			frontLeft.x + 1,frontLeft.y,frontLeft.z,
			frontLeft.x + 1 + leanForward.x * 2,frontLeft.y + leanForward.y * 2,frontLeft.z + leanForward.z * 2,

			frontLeft.x + 1 + leanForward.x * 2,frontLeft.y + leanForward.y * 2,frontLeft.z + leanForward.z * 2,
			frontLeft.x + leanForward.x * 2,frontLeft.y + leanForward.y * 2,frontLeft.z + leanForward.z * 2,
			frontLeft.x,frontLeft.y,frontLeft.z,

			//front left, lean right corner
			frontLeft.x ,frontLeft.y,frontLeft.z+ 1,
			frontLeft.x,frontLeft.y,frontLeft.z,
			frontLeft.x + leanRight.x * 2,frontLeft.y + leanRight.y * 2,frontLeft.z + leanRight.z * 2,

			frontLeft.x + leanRight.x * 2,frontLeft.y + leanRight.y * 2,frontLeft.z + leanRight.z * 2,
			frontLeft.x + leanRight.x * 2,frontLeft.y + leanRight.y * 2,frontLeft.z + 1 + leanRight.z * 2,
			frontLeft.x ,frontLeft.y,frontLeft.z + 1,

			//back left, lean back corner
			backLeft.x + 1, backLeft.y, backLeft.z,
			backLeft.x, backLeft.y, backLeft.z,
			backLeft.x + leanBack.x*2, backLeft.y+leanBack.y*2, backLeft.z+leanBack.z*2,

			backLeft.x + leanBack.x * 2, backLeft.y + leanBack.y * 2, backLeft.z + leanBack.z * 2,
			backLeft.x + 1 + leanBack.x * 2, backLeft.y + leanBack.y * 2, backLeft.z + leanBack.z * 2,
			backLeft.x + 1, backLeft.y, backLeft.z,

			//back left, lean right corner
			backLeft.x, backLeft.y, backLeft.z,
			backLeft.x, backLeft.y, backLeft.z + 1,
			backLeft.x + leanRight.x*2, backLeft.y + leanRight.y*2, backLeft.z + 1 + leanRight.z*2,

			backLeft.x + leanRight.x * 2, backLeft.y + leanRight.y * 2, backLeft.z + 1 + leanRight.z * 2,
			backLeft.x + leanRight.x * 2, backLeft.y + leanRight.y * 2, backLeft.z + leanRight.z * 2,
			backLeft.x, backLeft.y, backLeft.z,
		});
	model.uvs.insert(model.uvs.end(), {
			//front left lean forward
			leftBottom.tl.x, leftBottom.tl.y,
			leftBottom.tr.x, leftBottom.tr.y,
			leftTop.br.x, leftTop.br.y,

			leftTop.br.x, leftTop.br.y,
			leftTop.bl.x, leftTop.bl.y,
			leftBottom.tl.x, leftBottom.tl.y,
			//front left lean right
			rightBottom.tl.x, rightBottom.tl.y,
			rightBottom.tr.x, rightBottom.tr.y,
			rightTop.br.x, rightTop.br.y,

			rightTop.br.x, rightTop.br.y,
			rightTop.bl.x, rightTop.bl.y,
			rightBottom.tl.x, rightBottom.tl.y,
			//back left lean back
			rightBottom.tl.x, rightBottom.tl.y,
			rightBottom.tr.x, rightBottom.tr.y,
			rightTop.br.x, rightTop.br.y,

			rightTop.br.x, rightTop.br.y,
			rightTop.bl.x, rightTop.bl.y,
			rightBottom.tl.x, rightBottom.tl.y,
			//back left lean right
			leftBottom.tl.x, leftBottom.tl.y,
			leftBottom.tr.x, leftBottom.tr.y,
			leftTop.br.x, leftTop.br.y,

			leftTop.br.x, leftTop.br.y,
			leftTop.bl.x, leftTop.bl.y,
			leftBottom.tl.x, leftBottom.tl.y,
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
	//WIDTH MIDDLES (FRONT-LEAN-FORWARD and BACK-LEAN-BACK)
	for (int i = 1; i < width + 1; i++)
	{
		model.verts.insert(model.verts.end(), {
			//front left, lean forward corner
			frontLeft.x + i,frontLeft.y,frontLeft.z,
			frontLeft.x + i + 1,frontLeft.y,frontLeft.z,
			frontLeft.x + i + 1 + leanForward.x * 2,frontLeft.y + leanForward.y * 2,frontLeft.z + leanForward.z * 2,

			frontLeft.x + i + 1 + leanForward.x * 2,frontLeft.y + leanForward.y * 2,frontLeft.z + leanForward.z * 2,
			frontLeft.x + i + leanForward.x * 2,frontLeft.y + leanForward.y * 2,frontLeft.z + leanForward.z * 2,
			frontLeft.x + i,frontLeft.y,frontLeft.z,

			//back left, lean back corner
			backLeft.x + i + 1, backLeft.y, backLeft.z,
			backLeft.x + i , backLeft.y, backLeft.z,
			backLeft.x + i + leanBack.x * 2, backLeft.y + leanBack.y * 2, backLeft.z + leanBack.z * 2,

			backLeft.x + i + leanBack.x * 2, backLeft.y + leanBack.y * 2, backLeft.z + leanBack.z * 2,
			backLeft.x + i + 1 + leanBack.x * 2, backLeft.y + leanBack.y * 2, backLeft.z + leanBack.z * 2,
			backLeft.x + i + 1, backLeft.y, backLeft.z,
			});
		model.uvs.insert(model.uvs.end(), {
			middleBottom.tl.x, middleBottom.tl.y,
			middleBottom.tr.x, middleBottom.tr.y,
			middleTop.br.x, middleTop.br.y,

			middleTop.br.x, middleTop.br.y,
			middleTop.bl.x, middleTop.bl.y,
			middleBottom.tl.x, middleBottom.tl.y,

			middleBottom.tl.x, middleBottom.tl.y,
			middleBottom.tr.x, middleBottom.tr.y,
			middleTop.br.x, middleTop.br.y,

			middleTop.br.x, middleTop.br.y,
			middleTop.bl.x, middleTop.bl.y,
			middleBottom.tl.x, middleBottom.tl.y,
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
	}
	//DEPTH MIDDLES (ONLY ON LEFT SIDE)
	for (int i = 1; i < depth + 1; i++)
	{
		model.verts.insert(model.verts.end(), {
			//front left, lean right corner
			frontLeft.x ,frontLeft.y,frontLeft.z + 1 + i,
			frontLeft.x,frontLeft.y,frontLeft.z + i,
			frontLeft.x + leanRight.x * 2,frontLeft.y + leanRight.y * 2,frontLeft.z + i + leanRight.z * 2,

			frontLeft.x + leanRight.x * 2,frontLeft.y + leanRight.y * 2,frontLeft.z + i + leanRight.z * 2,
			frontLeft.x + leanRight.x * 2,frontLeft.y + leanRight.y * 2,frontLeft.z + i + 1 + leanRight.z * 2,
			frontLeft.x ,frontLeft.y,frontLeft.z + i + 1,
			});
		model.uvs.insert(model.uvs.end(), {
			middleBottom.tl.x, middleBottom.tl.y,
			middleBottom.tr.x, middleBottom.tr.y,
			middleTop.br.x, middleTop.br.y,

			middleTop.br.x, middleTop.br.y,
			middleTop.bl.x, middleTop.bl.y,
			middleBottom.tl.x, middleBottom.tl.y,

			});
		model.cols.insert(model.cols.end(), {
			1.0, 1.0, 1.0,
			1.0, 1.0, 1.0,
			1.0, 1.0, 1.0,

			1.0, 1.0, 1.0,
			1.0, 1.0, 1.0,
			1.0, 1.0, 1.0,


			});
	}


	return model;
}