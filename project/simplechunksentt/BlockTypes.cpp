#include "BlockTypes.hpp"

//Chunk THIS out!

const float onePixel = 0.0018382352941176;     // 1/544      Padding
const float textureWidth = 0.0588235294117647; // 1/17       32 pixel texture width
const float oneOver16 = 0.0625;

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

TextureFace::TextureFace(
	int x,
	int y
) : 
	tl(glm::vec2(0.0f + onePixel + (oneOver16 * (float)x), 1.0f - ((float)y * oneOver16) - onePixel)),
	bl(glm::vec2(0.0f + onePixel + (oneOver16 * (float)x), 1.0f - ((float)y * oneOver16) - textureWidth - onePixel)),
	br(glm::vec2(0.0f + onePixel + (oneOver16 * (float)x) + textureWidth, 1.0f - ((float)y * oneOver16) - textureWidth - onePixel)),
	tr(glm::vec2(0.0f + onePixel + (oneOver16 * (float)x) + textureWidth, 1.0f - ((float)y * oneOver16) - onePixel))
{

}

enum MyFlags { 
	COLORIZABLE = true,
	NOT_COLORIZABLE = false,
	UNIFORM = true,
	NOT_UNIFORM = false
};

const BlockType blockTypes[] = {

	BlockType(
		UNIFORM,
		TextureFace(
			0, 0
		),
		COLORIZABLE,
		glm::vec3(0.1, 0.6, 0.09),
		0.35f
	),

	BlockType(
		NOT_UNIFORM,
		TextureFace(
			2, 0
		),
		TextureFace(
			1, 0
		),
		TextureFace(
			2, 0
		)
	),

	BlockType(
		UNIFORM,
		TextureFace(
			8, 0
		),
		COLORIZABLE,
		glm::vec3(178.0 / 255.0, 119.0 / 255.0, 86.0 / 255.0),
		0.00f
	),

	BlockType(
		UNIFORM,
		TextureFace(
			9, 0
		)
	),

	BlockType(
		UNIFORM,
		TextureFace(
		0, 1
		)
	),

	BlockType(
		UNIFORM,
		TextureFace(
		1, 1
		)
	)
};