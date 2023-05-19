#include "BlockTypes.h"

float onePixel = 0.0018382352941176;     // 1/544
float textureWidth = 0.0588235294117647; // 1/17


BlockType::BlockType(
	bool allOneTexture,
	TextureFace all,
	TextureFace top,
	TextureFace sides,
	TextureFace bottom
) : allOneTexture(allOneTexture), all(all), top(top), sides(sides), bottom(bottom) {

}

TextureFace::TextureFace(
	glm::vec2 tl,
	glm::vec2 bl,
	glm::vec2 br,
	glm::vec2 tr
) : tl(tl), bl(bl), br(br), tr(tr) {

}

const BlockType blockTypes[] = {
	
};