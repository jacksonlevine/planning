#include "Tree.hpp"
#include "Maths.hpp"
#include "BlockTypes.hpp"
Model Tree::getPineTreeModel(float x, float y, float z)
{
	Model model;

	float height = 5.0f + (rando() * 32.0f);

	glm::vec3 centerBottom(x + 0.5, y-1.0, z + 0.5);

	glm::vec3 center(0.0, 2.0, 0.0);

	glm::vec3 leftFront(-1.0, 0.0, -1.0);
		glm::vec3 centerFront(0.0, 0.0, -1.0);
			glm::vec3 rightFront(1.0, 0.0, -1.0);

	glm::vec3 leftBack(-1.0, 0.0, 1.0);
	glm::vec3 centerLeft(-1.0, 0.0, 0.0);
	glm::vec3 rightBack(1.0, 0.0, 1.0);
	glm::vec3 centerRight(1.0, 0.0, 0.0);

	glm::vec3 centerBack(0.0, 0.0, 1.0);
	GLfloat leafCircle[] = {

		 
		 center.x, center.y, center.z,
		 centerLeft.x, centerLeft.y, centerLeft.z,
		 leftFront.x, leftFront.y, leftFront.z,

		 leftFront.x, leftFront.y, leftFront.z,
		 centerFront.x, centerFront.y, centerFront.z,
		 center.x, center.y, center.z,

		 // DIN

		 center.x, center.y, center.z,
		 centerFront.x, centerFront.y, centerFront.z,
		 rightFront.x, rightFront.y, rightFront.z,
		 

		 
		 rightFront.x, rightFront.y, rightFront.z,
		 centerRight.x, centerRight.y, centerRight.z,
		 center.x, center.y, center.z,



		 center.x, center.y, center.z,
		 centerRight.x, centerRight.y, centerRight.z,
		 rightBack.x, rightBack.y, rightBack.z,

		 
		 rightBack.x, rightBack.y, rightBack.z,
		 centerBack.x, centerBack.y, centerBack.z,
		 center.x, center.y, center.z,


		 center.x, center.y, center.z,
		 centerBack.x, centerBack.y, centerBack.z,
		 leftBack.x, leftBack.y, leftBack.z,


		 leftBack.x, leftBack.y, leftBack.z,
		 centerLeft.x, centerLeft.y, centerLeft.z,
		 center.x, center.y, center.z,


	};
	
	TextureFace topLeft(11, 1);
	TextureFace bottomLeft(11, 0);
	TextureFace bottomRight(12, 0);
	TextureFace topRight(12, 1);



	float leavesStart = height * 0.2;
	float inter = 3.0 + (rando()/8);



	for (float i = leavesStart; i < height; i+= inter)
	{
		
		for (int v = 0; v < 72; v+=3)
		{
			float factor = (height - (i));
			model.verts.insert(model.verts.end(), {
					(leafCircle[v]*factor)+centerBottom.x, i+ leafCircle[v+1]*5.0f + y, (leafCircle[v+2]*factor)+centerBottom.z
				});
			model.cols.insert(model.cols.end(), {
					1.0, 1.0, 1.0
				});
		}
		model.uvs.insert(model.uvs.end(), {


				topRight.br.x, topRight.br.y,
				topLeft.bl.x, topLeft.bl.y,
				bottomLeft.tl.x, bottomLeft.tl.y,

				bottomLeft.tl.x, bottomLeft.tl.y,
				bottomRight.tr.x, bottomRight.tr.y,
				topRight.br.x, topRight.br.y,

				topRight.br.x, topRight.br.y,
				topLeft.bl.x, topLeft.bl.y,
				bottomLeft.tl.x, bottomLeft.tl.y,

				bottomLeft.tl.x, bottomLeft.tl.y,
				bottomRight.tr.x, bottomRight.tr.y,
				topRight.br.x, topRight.br.y,

				topRight.br.x, topRight.br.y,
				topLeft.bl.x, topLeft.bl.y,
				bottomLeft.tl.x, bottomLeft.tl.y,

				bottomLeft.tl.x, bottomLeft.tl.y,
				bottomRight.tr.x, bottomRight.tr.y,
				topRight.br.x, topRight.br.y,

				topRight.br.x, topRight.br.y,
				topLeft.bl.x, topLeft.bl.y,
				bottomLeft.tl.x, bottomLeft.tl.y,

				bottomLeft.tl.x, bottomLeft.tl.y,
				bottomRight.tr.x, bottomRight.tr.y,
				topRight.br.x, topRight.br.y,
			});
	
	
	}


	TextureFace logFace(10, 0);
	model.verts.insert(model.verts.end(), {
			centerBottom.x-1.0f, centerBottom.y, centerBottom.z-1.0f,
			centerBottom.x + 1.0f, centerBottom.y, centerBottom.z - 1.0f,
			centerBottom.x, centerBottom.y+height , centerBottom.z,

			centerBottom.x + 1.0f, centerBottom.y, centerBottom.z - 1.0f,
			centerBottom.x + 1.0f, centerBottom.y, centerBottom.z + 1.0f,
			centerBottom.x, centerBottom.y + height , centerBottom.z,

			centerBottom.x + 1.0f, centerBottom.y, centerBottom.z + 1.0f,
			centerBottom.x - 1.0f, centerBottom.y, centerBottom.z + 1.0f,
			centerBottom.x, centerBottom.y + height , centerBottom.z,

			centerBottom.x - 1.0f, centerBottom.y, centerBottom.z + 1.0f,
			centerBottom.x - 1.0f, centerBottom.y, centerBottom.z - 1.0f,
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