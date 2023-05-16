#pragma once
#include <iostream>
#include <format>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <folly/Optional.h>
#include <folly/container/F14Map.h>
class GLWrapper {
public:
	std::vector<GLuint> vaos;
	int vaoIndex;
	GLuint shaderProgram;
	GLFWwindow* window;
	int initializeGL();
	void runGLLoop();
};

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);


