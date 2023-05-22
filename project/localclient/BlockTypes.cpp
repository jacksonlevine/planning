#include "BlockTypes.hpp"

//Chunk THIS out!

const float onePixel = 0.0018382352941176;     // 1/544      Padding
const float textureWidth = 0.0588235294117647; // 1/17       32 pixel texture width


BlockType::BlockType(
	bool allOneTexture,
	TextureFace top,
	TextureFace sides,
	TextureFace bottom
) :
	allOneTexture(allOneTexture),
	all(sides),
	top(top),
	sides(sides),
	bottom(bottom),
	colorizable(false),
	baseColor(glm::vec3(1.0, 1.0, 1.0)),
	colorVolatility(0)
{

}

BlockType::BlockType(
	bool allOneTexture,
	TextureFace all
) : 
	allOneTexture(allOneTexture), 
	all(all), 
	top(all), 
	sides(all), 
	bottom(all),
	colorizable(false),
	baseColor(glm::vec3(1.0, 1.0, 1.0)),
	colorVolatility(0)
{

}

BlockType::BlockType(
	bool allOneTexture,
	TextureFace top,
	TextureFace sides,
	TextureFace bottom,
	bool colorizable,
    glm::vec3 baseColor,
    float colorVolatility
) :
	allOneTexture(allOneTexture),
	all(sides),
	top(top),
	sides(sides),
	bottom(bottom),
	colorizable(colorizable),
	baseColor(baseColor),
	colorVolatility(colorVolatility)
{

}

BlockType::BlockType(
	bool allOneTexture,
	TextureFace all,
	bool colorizable,
	glm::vec3 baseColor,
	float colorVolatility
) :
	allOneTexture(allOneTexture),
	all(all),
	top(all),
	sides(all),
	bottom(all),
	colorizable(colorizable),
	baseColor(baseColor),
	colorVolatility(colorVolatility)
{

}

TextureFace::TextureFace(
	glm::vec2 tl,
	glm::vec2 bl,
	glm::vec2 br,
	glm::vec2 tr
) : tl(tl), bl(bl), br(br), tr(tr) {

}

const BlockType blockTypes[] = {
	BlockType(
		true,
		TextureFace(
			glm::vec2(0.0f + onePixel + ((textureWidth + (onePixel * 2.0f)) * 0.0f)               , 1.0f - onePixel),
			glm::vec2(0.0f + onePixel + ((textureWidth + (onePixel * 2.0f)) * 0.0f)               , 1.0f - textureWidth - onePixel),
			glm::vec2(0.0f + onePixel + ((textureWidth + (onePixel * 2.0f)) * 0.0f) + textureWidth, 1.0f - textureWidth - onePixel),
			glm::vec2(0.0f + onePixel + ((textureWidth + (onePixel * 2.0f)) * 0.0f) + textureWidth, 1.0f - onePixel)
		),
		true,
		glm::vec3(0.1, 0.6, 0.09),
		0.35f
	),
	BlockType(
		false,
		TextureFace(
			glm::vec2(0.0f + onePixel + ((textureWidth + (onePixel * 2.0f)) * 2.0f)               , 1.0f - onePixel),
			glm::vec2(0.0f + onePixel + ((textureWidth + (onePixel * 2.0f)) * 2.0f)               , 1.0f - textureWidth - onePixel),
			glm::vec2(0.0f + onePixel + ((textureWidth + (onePixel * 2.0f)) * 2.0f) + textureWidth, 1.0f - textureWidth - onePixel),
			glm::vec2(0.0f + onePixel + ((textureWidth + (onePixel * 2.0f)) * 2.0f) + textureWidth, 1.0f - onePixel)
		),
		TextureFace(
			glm::vec2(0.0f + onePixel + ((textureWidth + (onePixel * 2.0f)) * 1.0f)               , 1.0f - onePixel),
			glm::vec2(0.0f + onePixel + ((textureWidth + (onePixel * 2.0f)) * 1.0f)               , 1.0f - textureWidth - onePixel),
			glm::vec2(0.0f + onePixel + ((textureWidth + (onePixel * 2.0f)) * 1.0f) + textureWidth, 1.0f - textureWidth - onePixel),
			glm::vec2(0.0f + onePixel + ((textureWidth + (onePixel * 2.0f)) * 1.0f) + textureWidth, 1.0f - onePixel)
		),
		TextureFace(
			glm::vec2(0.0f + onePixel + ((textureWidth + (onePixel * 2.0f)) * 2.0f)               , 1.0f - onePixel),
			glm::vec2(0.0f + onePixel + ((textureWidth + (onePixel * 2.0f)) * 2.0f)               , 1.0f - textureWidth - onePixel),
			glm::vec2(0.0f + onePixel + ((textureWidth + (onePixel * 2.0f)) * 2.0f) + textureWidth, 1.0f - textureWidth - onePixel),
			glm::vec2(0.0f + onePixel + ((textureWidth + (onePixel * 2.0f)) * 2.0f) + textureWidth, 1.0f - onePixel)
		)
	),
};