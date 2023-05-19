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
};

struct BlockType {
public:
	const bool allOneTexture;
	const TextureFace all;
	const TextureFace top;
	const TextureFace sides;
	const TextureFace bottom;
	BlockType(
		bool allOneTexture,
		TextureFace all,
	    TextureFace top,
	    TextureFace sides,
	    TextureFace bottom
	);
};

extern const BlockType blockTypes[];