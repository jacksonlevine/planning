#include "Tree.hpp"
#include "Maths.hpp"

Model Plant::getBlackberryBushModel(float x, float y, float z)
{
	Model model;

	TextureFace leftTop(2.0f, 3);
	TextureFace leftBottom(2.0f, 2.0f);

	TextureFace middleTop(3, 3);
	TextureFace middleBottom(3, 2.0f);

	TextureFace rightTop(4, 3);
	TextureFace rightBottom(4, 2);

	TextureFace bushTop(0, 4);

	int width = 4 * rando() + (rando() * 2.0f);
	int depth = 4 * rando() + (rando() * 2.0f);

	glm::vec3 frontLeft(x, World::getTerrainNoise(x, z), z);
	glm::vec3 backLeft(x, World::getTerrainNoise(x, z+depth+2.0f), z+depth+2.0f);

	glm::vec3 frontRight(x+width+2.0f, World::getTerrainNoise(x+width+2.0f, z), z);
	glm::vec3 backRight(x+width+2.0f, World::getTerrainNoise(x+width+2.0f, z+depth+2.0f), z + depth + 2.0f);

	glm::vec3 leanBack(0, 1, -0.5);
	glm::vec3 leanForward(0, 1, 0.5);
	glm::vec3 leanLeft(-0.5, 1, 0);
	glm::vec3 leanRight(0.5, 1, 0);

	//LEFT SIDE CORNERS 
	model.verts.insert(model.verts.end(), {
		    //front left, lean forward corner
			frontLeft.x,(float)World::getTerrainNoise(frontLeft.x, frontLeft.z),frontLeft.z,
			frontLeft.x + 1,(float)World::getTerrainNoise(frontLeft.x+1, frontLeft.z),frontLeft.z,
			frontLeft.x + 1 + leanForward.x * 2.0f,(float)World::getTerrainNoise(frontLeft.x+1, frontLeft.z) + leanForward.y * 2.0f,frontLeft.z + leanForward.z * 2.0f,

			frontLeft.x + 1 + leanForward.x * 2.0f,(float)World::getTerrainNoise(frontLeft.x +  1, frontLeft.z) + leanForward.y * 2.0f,frontLeft.z + leanForward.z * 2.0f,
			frontLeft.x + leanForward.x * 2.0f,(float)World::getTerrainNoise(frontLeft.x, frontLeft.z) + leanForward.y * 2.0f,frontLeft.z + leanForward.z * 2.0f,
			frontLeft.x,frontLeft.y,frontLeft.z,

			//front left, lean right corner
			frontLeft.x ,(float)World::getTerrainNoise(frontLeft.x, frontLeft.z+1),frontLeft.z+ 1,
			frontLeft.x,(float)World::getTerrainNoise(frontLeft.x, frontLeft.z),frontLeft.z,
			frontLeft.x + leanRight.x * 2.0f,(float)World::getTerrainNoise(frontLeft.x, frontLeft.z) + leanRight.y * 2.0f,frontLeft.z + leanRight.z * 2.0f,

			frontLeft.x + leanRight.x * 2.0f,(float)World::getTerrainNoise(frontLeft.x, frontLeft.z) + leanRight.y * 2.0f,frontLeft.z + leanRight.z * 2.0f,
			frontLeft.x + leanRight.x * 2.0f,(float)World::getTerrainNoise(frontLeft.x, frontLeft.z+1) + leanRight.y * 2.0f,frontLeft.z + 1 + leanRight.z * 2.0f,
			frontLeft.x ,(float)World::getTerrainNoise(frontLeft.x, frontLeft.z+1),frontLeft.z + 1,

			//back left, lean back corner
			backLeft.x + 1, (float)World::getTerrainNoise(backLeft.x+1, backLeft.z), backLeft.z,
			backLeft.x, (float)World::getTerrainNoise(backLeft.x, backLeft.z), backLeft.z,
			backLeft.x + leanBack.x*2.0f, (float)World::getTerrainNoise(backLeft.x, backLeft.z) +leanBack.y*2.0f, backLeft.z+leanBack.z*2.0f,

			backLeft.x + leanBack.x * 2.0f, (float)World::getTerrainNoise(backLeft.x, backLeft.z) + leanBack.y * 2.0f, backLeft.z + leanBack.z * 2.0f,
			backLeft.x + 1 + leanBack.x * 2.0f, (float)World::getTerrainNoise(backLeft.x, backLeft.z) + leanBack.y * 2.0f, backLeft.z + leanBack.z * 2.0f,
			backLeft.x + 1, (float)World::getTerrainNoise(backLeft.x+1, backLeft.z), backLeft.z,

			//back left, lean right corner
			backLeft.x, (float)World::getTerrainNoise(backLeft.x, backLeft.z), backLeft.z,
			backLeft.x, (float)World::getTerrainNoise(backLeft.x, backLeft.z-1), backLeft.z - 1,
			backLeft.x + leanRight.x*2.0f, (float)World::getTerrainNoise(backLeft.x, backLeft.z-1) + leanRight.y*2.0f, backLeft.z - 1 + leanRight.z*2.0f,

			backLeft.x + leanRight.x * 2.0f,(float)World::getTerrainNoise(backLeft.x, backLeft.z-1) + leanRight.y * 2.0f, backLeft.z - 1 + leanRight.z * 2.0f,
			backLeft.x + leanRight.x * 2.0f, (float)World::getTerrainNoise(backLeft.x, backLeft.z) + leanRight.y * 2.0f, backLeft.z + leanRight.z * 2.0f,
			backLeft.x, (float)World::getTerrainNoise(backLeft.x, backLeft.z), backLeft.z,

			//front right, lean forward corner
			frontRight.x-1, (float)World::getTerrainNoise(frontRight.x-1, frontRight.z), frontRight.z,
			frontRight.x, (float)World::getTerrainNoise(frontRight.x, frontRight.z), frontRight.z,
			frontRight.x + leanForward.x*2.0f, (float)World::getTerrainNoise(frontRight.x, frontRight.z) + leanForward.y * 2.0f, frontRight.z + leanForward.z * 2.0f,

			frontRight.x + leanForward.x * 2.0f, (float)World::getTerrainNoise(frontRight.x, frontRight.z) + leanForward.y * 2.0f, frontRight.z + leanForward.z * 2.0f,
			frontRight.x - 1 + leanForward.x * 2.0f, (float)World::getTerrainNoise(frontRight.x - 1, frontRight.z) + leanForward.y * 2.0f, frontRight.z + leanForward.z * 2.0f,
			frontRight.x - 1, (float)World::getTerrainNoise(frontRight.x - 1, frontRight.z), frontRight.z,

			//front right, lean left corner
			frontRight.x, (float)World::getTerrainNoise(frontRight.x, frontRight.z), frontRight.z,
			frontRight.x, (float)World::getTerrainNoise(frontRight.x, frontRight.z+1), frontRight.z+1,
			frontRight.x + leanLeft.x*2.0f, (float)World::getTerrainNoise(frontRight.x, frontRight.z + 1) + leanLeft.y * 2.0f, frontRight.z + 1 + leanLeft.z * 2.0f,

			frontRight.x + leanLeft.x * 2.0f, (float)World::getTerrainNoise(frontRight.x, frontRight.z + 1) + leanLeft.y * 2.0f, frontRight.z + 1 + leanLeft.z * 2.0f,
			frontRight.x + leanLeft.x * 2.0f, (float)World::getTerrainNoise(frontRight.x, frontRight.z) + leanLeft.y * 2.0f, frontRight.z + leanLeft.z * 2.0f,
			frontRight.x, (float)World::getTerrainNoise(frontRight.x, frontRight.z), frontRight.z,

			//back right, lean back corner
			backRight.x - 1, (float)World::getTerrainNoise(backRight.x - 1, backRight.z), backRight.z,
			backRight.x, (float)World::getTerrainNoise(backRight.x, backRight.z), backRight.z,
			backRight.x + leanBack.x * 2.0f, (float)World::getTerrainNoise(backRight.x, backRight.z) + leanBack.y * 2.0f, backRight.z + leanBack.z * 2.0f,

			backRight.x + leanBack.x * 2.0f, (float)World::getTerrainNoise(backRight.x, backRight.z) + leanBack.y * 2.0f, backRight.z + leanBack.z * 2.0f,
			backRight.x -1 + leanBack.x * 2.0f, (float)World::getTerrainNoise(backRight.x-1, backRight.z) + leanBack.y * 2.0f, backRight.z + leanBack.z * 2.0f,
			backRight.x - 1, (float)World::getTerrainNoise(backRight.x - 1, backRight.z), backRight.z,

			//back right, lean left corner
			backRight.x, (float)World::getTerrainNoise(backRight.x, backRight.z-1), backRight.z-1,
			backRight.x, (float)World::getTerrainNoise(backRight.x, backRight.z), backRight.z,
			backRight.x + leanLeft.x * 2.0f, (float)World::getTerrainNoise(backRight.x, backRight.z) + leanLeft.y * 2.0f, backRight.z + leanLeft.z * 2.0f,

			backRight.x + leanLeft.x * 2.0f, (float)World::getTerrainNoise(backRight.x, backRight.z) + leanLeft.y * 2.0f, backRight.z + leanLeft.z * 2.0f,
			backRight.x + leanLeft.x * 2.0f, (float)World::getTerrainNoise(backRight.x, backRight.z-1) + leanLeft.y * 2.0f , backRight.z -1 + leanLeft.z * 2.0f,
			backRight.x, (float)World::getTerrainNoise(backRight.x, backRight.z - 1), backRight.z - 1,
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
			//front right lean forward
			rightBottom.tl.x, rightBottom.tl.y,
			rightBottom.tr.x, rightBottom.tr.y,
			rightTop.br.x, rightTop.br.y,

			rightTop.br.x, rightTop.br.y,
			rightTop.bl.x, rightTop.bl.y,
			rightBottom.tl.x, rightBottom.tl.y,
			//front right lean left
			leftBottom.tl.x, leftBottom.tl.y,
			leftBottom.tr.x, leftBottom.tr.y,
			leftTop.br.x, leftTop.br.y,

			leftTop.br.x, leftTop.br.y,
			leftTop.bl.x, leftTop.bl.y,
			leftBottom.tl.x, leftBottom.tl.y,
			//back right lean back
			rightBottom.tl.x, rightBottom.tl.y,
			rightBottom.tr.x, rightBottom.tr.y,
			rightTop.br.x, rightTop.br.y,

			rightTop.br.x, rightTop.br.y,
			rightTop.bl.x, rightTop.bl.y,
			rightBottom.tl.x, rightBottom.tl.y,
			//back right lean left
			rightBottom.tl.x, rightBottom.tl.y,
			rightBottom.tr.x, rightBottom.tr.y,
			rightTop.br.x, rightTop.br.y,

			rightTop.br.x, rightTop.br.y,
			rightTop.bl.x, rightTop.bl.y,
			rightBottom.tl.x, rightBottom.tl.y,
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
			frontLeft.x + i, (float)World::getTerrainNoise(frontLeft.x + i, frontLeft.z),frontLeft.z,
			frontLeft.x + i + 1,(float)World::getTerrainNoise(frontLeft.x + i + 1, frontLeft.z),frontLeft.z,
			frontLeft.x + i + 1 + leanForward.x * 2.0f,(float)World::getTerrainNoise(frontLeft.x + i + 1, frontLeft.z) + leanForward.y * 2.0f,frontLeft.z + leanForward.z * 2.0f,

			frontLeft.x + i + 1 + leanForward.x * 2.0f, (float)World::getTerrainNoise(frontLeft.x + i + 1, frontLeft.z) + leanForward.y * 2.0f,frontLeft.z + leanForward.z * 2.0f,
			frontLeft.x + i + leanForward.x * 2.0f,(float)World::getTerrainNoise(frontLeft.x + i, frontLeft.z) + leanForward.y * 2.0f,frontLeft.z + leanForward.z * 2.0f,
			frontLeft.x + i,(float)World::getTerrainNoise(frontLeft.x + i, frontLeft.z),frontLeft.z,

			//back left, lean back corner
			backLeft.x + i + 1, (float)World::getTerrainNoise(backLeft.x + i + 1, backLeft.z), backLeft.z,
			backLeft.x + i , (float)World::getTerrainNoise(backLeft.x + i, backLeft.z), backLeft.z,
			backLeft.x + i + leanBack.x * 2.0f, (float)World::getTerrainNoise(backLeft.x + i, backLeft.z) + leanBack.y * 2.0f, backLeft.z + leanBack.z * 2.0f,

			backLeft.x + i + leanBack.x * 2.0f, (float)World::getTerrainNoise(backLeft.x + i, backLeft.z) + leanBack.y * 2.0f, backLeft.z + leanBack.z * 2.0f,
			backLeft.x + i + 1 + leanBack.x * 2.0f, (float)World::getTerrainNoise(backLeft.x + i + 1, backLeft.z) + leanBack.y * 2.0f, backLeft.z + leanBack.z * 2.0f,
			backLeft.x + i + 1, (float)World::getTerrainNoise(backLeft.x + i + 1, backLeft.z), backLeft.z,
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
	//DEPTH MIDDLES (ONLY ON LEFT SIDE) ADDING RIGHT RN
	for (int i = 1; i < depth + 1; i++)
	{
		model.verts.insert(model.verts.end(), {
			//front left, lean right corner
			frontLeft.x ,(float)World::getTerrainNoise(frontLeft.x, frontLeft.z + 1 + i),frontLeft.z + 1 + i,
			frontLeft.x,(float)World::getTerrainNoise(frontLeft.x, frontLeft.z + i),frontLeft.z + i,
			frontLeft.x + leanRight.x * 2.0f,(float)World::getTerrainNoise(frontLeft.x, frontLeft.z + i) + leanRight.y * 2.0f,frontLeft.z + i + leanRight.z * 2.0f,

			frontLeft.x + leanRight.x * 2.0f,(float)World::getTerrainNoise(frontLeft.x, frontLeft.z + i) + leanRight.y * 2.0f,frontLeft.z + i + leanRight.z * 2.0f,
			frontLeft.x + leanRight.x * 2.0f,(float)World::getTerrainNoise(frontLeft.x, frontLeft.z + i + 1) + leanRight.y * 2.0f,frontLeft.z + i + 1 + leanRight.z * 2.0f,
			frontLeft.x ,(float)World::getTerrainNoise(frontLeft.x, frontLeft.z + i + 1),frontLeft.z + i + 1,

			//front right, lean left corner
			frontRight.x, (float)World::getTerrainNoise(frontRight.x, frontRight.z + i), frontRight.z + i,
			frontRight.x, (float)World::getTerrainNoise(frontRight.x, frontRight.z + i + 1), frontRight.z + i + 1,
			frontRight.x + leanLeft.x * 2.0f, (float)World::getTerrainNoise(frontRight.x, frontRight.z + i + 1) + leanLeft.y * 2.0f, frontRight.z + i + 1 + leanLeft.z * 2.0f,

			frontRight.x + leanLeft.x * 2.0f, (float)World::getTerrainNoise(frontRight.x, frontRight.z + i + 1) + leanLeft.y * 2.0f, frontRight.z + i + 1 + leanLeft.z * 2.0f,
			frontRight.x + leanLeft.x * 2.0f, (float)World::getTerrainNoise(frontRight.x, frontRight.z + i) + leanLeft.y * 2.0f, frontRight.z + i + leanLeft.z * 2.0f,
			frontRight.x, (float)World::getTerrainNoise(frontRight.x, frontRight.z + i), frontRight.z + i,
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

	for (float i = 0.5; i < width + 1 + 0.5; i++)
	{
		for (float d = 0.5; d < depth + 1 + 0.5; d++)
		{
			model.verts.insert(model.verts.end(), {
					frontLeft.x + i, (float)World::getTerrainNoise(frontLeft.x + i, frontLeft.z + d) + 1.1f, frontLeft.z + d,
					frontLeft.x + i + 1, (float)World::getTerrainNoise(frontLeft.x + i + 1, frontLeft.z + d) + 1.1f, frontLeft.z + d,
					frontLeft.x + i + 1,(float)World::getTerrainNoise(frontLeft.x + i + 1, frontLeft.z + d + 1) + 1.1f, frontLeft.z + d + 1,

					frontLeft.x + i + 1, (float)World::getTerrainNoise(frontLeft.x + i + 1, frontLeft.z + d + 1) + 1.1f, frontLeft.z + d + 1,
					frontLeft.x + i, (float)World::getTerrainNoise(frontLeft.x + i, frontLeft.z + d + 1) + 1.1f, frontLeft.z + d + 1,
					frontLeft.x + i, (float)World::getTerrainNoise(frontLeft.x + i, frontLeft.z + d) + 1.1f, frontLeft.z + d,
				});
			model.uvs.insert(model.uvs.end(), {
					bushTop.tl.x, bushTop.tl.y,
					bushTop.tr.x, bushTop.tr.y,
					bushTop.br.x, bushTop.br.y,

					bushTop.br.x, bushTop.br.y,
					bushTop.bl.x, bushTop.bl.y,
					bushTop.tl.x, bushTop.tl.y,
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
	}

	return model;
}