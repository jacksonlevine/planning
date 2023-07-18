#include "Tree.hpp".
#include "Maths.hpp"
#include "BlockTypes.hpp"
Model Rock::getSmallRockModel(float x, float y, float z) {
	return General::getCrossModel(x, y, z, TextureFace(7, 2), 0, false);
}

// BIG BOULDER MODEL

Model Rock::getRockModel(float x, float y, float z)
{
	Model model;

	float frown = rando()/2.0;

	float bottomTier = 1.8 + rando()/2.0;
	float secondTierRecess = std::min(rando(), bottomTier-1.0f);
	float topPointMag = 1.0 + rando()* 2;

	float height = 1.5 + (1.5 * rando()) + (bottomTier * rando());

	float sidesOffset = bottomTier;

	glm::vec3 fl(x - bottomTier, y - 1.5, z - bottomTier);
	glm::vec3 fr(x + bottomTier, y - 1.5, z - bottomTier);
	glm::vec3 br(x + bottomTier, y - 1.5, z + bottomTier);
	glm::vec3 bl(x - bottomTier, y - 1.5, z + bottomTier);

	glm::vec3 centerf = glm::mix(fl, fr, 0.5) + glm::vec3(0, height, 0);
	glm::vec3 centerfleft = glm::mix(fl, fr, 0.5) + glm::vec3(-sidesOffset, height - frown, secondTierRecess);
	glm::vec3 centerfright = glm::mix(fl, fr, 0.5) + glm::vec3(sidesOffset, height - frown, secondTierRecess);

	glm::vec3 centerl = glm::mix(fl, bl, 0.5) + glm::vec3(0, height, 0);
	glm::vec3 centerlleft = glm::mix(fl, bl, 0.5) + glm::vec3(secondTierRecess, height - frown, sidesOffset);
	glm::vec3 centerlright = glm::mix(fl, bl, 0.5) + glm::vec3(secondTierRecess, height - frown, -sidesOffset);

	glm::vec3 centerr = glm::mix(fr, br, 0.5) + glm::vec3(0, height, 0);
	glm::vec3 centerrleft = glm::mix(fr, br, 0.5) + glm::vec3(-secondTierRecess, height - frown, -sidesOffset);
	glm::vec3 centerrright = glm::mix(fr, br, 0.5) + glm::vec3(-secondTierRecess, height - frown, sidesOffset);

	glm::vec3 centerb = glm::mix(br, bl, 0.5) + glm::vec3(0, height, 0);
	glm::vec3 centerbleft = glm::mix(br, bl, 0.5) + glm::vec3(sidesOffset, height - frown, -secondTierRecess);
	glm::vec3 centerbright = glm::mix(br, bl, 0.5) + glm::vec3(-sidesOffset, height - frown, -secondTierRecess);


		glm::vec3 topcenter = ((centerf + centerb + centerl + centerr) / 4.0f) + glm::vec3(0, topPointMag, 0);


	model.verts.insert(model.verts.end(), {
			fl.x, fl.y, fl.z,
			fr.x, fr.y, fr.z, 
			centerf.x, centerf.y, centerf.z,


				fl.x, fl.y, fl.z,
				centerf.x, centerf.y, centerf.z,
				centerfleft.x, centerfleft.y, centerfleft.z,

				centerf.x, centerf.y, centerf.z,
				fr.x, fr.y, fr.z,
				centerfright.x, centerfright.y, centerfright.z,



			fr.x, fr.y, fr.z,
			br.x, br.y, br.z,
			centerr.x, centerr.y, centerr.z,

				fr.x, fr.y, fr.z,
				centerr.x, centerr.y, centerr.z,
				centerrleft.x, centerrleft.y, centerrleft.z,


				centerr.x, centerr.y, centerr.z,
				br.x, br.y, br.z,
				centerrright.x, centerrright.y, centerrright.z,

			br.x, br.y, br.z,
			bl.x, bl.y, bl.z,
			centerb.x, centerb.y, centerb.z,

				br.x, br.y, br.z,
				centerb.x, centerb.y, centerb.z,
				centerbleft.x, centerbleft.y, centerbleft.z,

			
				centerb.x, centerb.y, centerb.z,
				bl.x, bl.y, bl.z,
				centerbright.x, centerbright.y, centerbright.z,

			bl.x, bl.y, bl.z,
			fl.x, fl.y, fl.z,
			centerl.x, centerl.y, centerl.z,

				bl.x, bl.y, bl.z,
				centerl.x, centerl.y, centerl.z,
				centerlleft.x, centerlleft.y, centerlleft.z,
				
				centerl.x, centerl.y, centerl.z,
				fl.x, fl.y, fl.z,
				centerlright.x, centerlright.y, centerlright.z,



				//top parts

				centerfleft.x, centerfleft.y, centerfleft.z,
				centerf.x, centerf.y, centerf.z,
				topcenter.x, topcenter.y, topcenter.z,

				topcenter.x, topcenter.y, topcenter.z,
				centerf.x, centerf.y, centerf.z,
				centerfright.x, centerfright.y, centerfright.z,



				centerrleft.x, centerrleft.y, centerrleft.z,
				centerr.x, centerr.y, centerr.z,
				topcenter.x, topcenter.y, topcenter.z,

				topcenter.x, topcenter.y, topcenter.z,
				centerr.x, centerr.y, centerr.z,
				centerrright.x, centerrright.y, centerrright.z,



				centerbleft.x, centerbleft.y, centerbleft.z,
				centerb.x, centerb.y, centerb.z,
				topcenter.x, topcenter.y, topcenter.z,

				topcenter.x, topcenter.y, topcenter.z,
				centerb.x, centerb.y, centerb.z,
				centerbright.x, centerbright.y, centerbright.z,



				centerlleft.x, centerlleft.y, centerlleft.z,
				centerl.x, centerl.y, centerl.z,
				topcenter.x, topcenter.y, topcenter.z,

				topcenter.x, topcenter.y, topcenter.z,
				centerl.x, centerl.y, centerl.z,
				centerlright.x, centerlright.y, centerlright.z
		});
	TextureFace stone(2,0);

	for (int i = 0; i < 20; i++)
	{
		model.cols.insert(model.cols.end(), {
				1.0, 1.0, 1.0,
				1.0, 1.0, 1.0,
				1.0, 1.0, 1.0
			});

		model.uvs.insert(model.uvs.end(), {
				stone.bl.x, stone.bl.y,
				stone.br.x, stone.br.y,
				stone.tl.x, stone.tl.y
			});
	}
	return model;
}