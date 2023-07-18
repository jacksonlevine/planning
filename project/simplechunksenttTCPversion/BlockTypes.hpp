#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

struct TextureFace {
public:
	glm::vec2 tl;
	glm::vec2 bl;
	glm::vec2 br;
	glm::vec2 tr;
	TextureFace(
		glm::vec2 tl,
		glm::vec2 bl,
		glm::vec2 br,
		glm::vec2 tr
	);
	TextureFace(
		int x,
		int y
	);
};

struct BlockType {
public:
	const bool allOneTexture;
	const bool colorizable;
	const glm::vec3 baseColor;
	const float colorVolatility;
	const TextureFace all;
	const TextureFace top;
	const TextureFace sides;
	const TextureFace bottom;
	BlockType(
		bool allOneTexture,
	    TextureFace top,
	    TextureFace sides,
	    TextureFace bottom
	);
	BlockType(
		bool allOneTexture,
		TextureFace all
	);
	BlockType(
		bool allOneTexture,
		TextureFace top,
		TextureFace sides,
		TextureFace bottom,
		bool colorizable,
		glm::vec3 baseColor,
		float colorVolatility
	);
	BlockType(
		bool allOneTexture,
		TextureFace all,
		bool colorizable,
		glm::vec3 baseColor,
		float colorVolatility
	);
};

extern const BlockType blockTypes[];
extern const float onePixel;
extern const float textureWidth;
extern const float oneOver16;