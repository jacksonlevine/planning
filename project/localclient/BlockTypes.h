#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

struct TextureFace {
	glm::vec2 tl;
	glm::vec2 bl;
	glm::vec2 br;
	glm::vec2 tr;
};

struct BlockType {
public:
	const bool allOneTexture;
	const TextureFace top;
	const TextureFace sides;
	const TextureFace bottom;

};

extern const BlockType blockTypes[];