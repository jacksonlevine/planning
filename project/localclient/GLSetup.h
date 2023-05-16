#pragma once
#include <iostream>
#include <format>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class GLWrapper {
public:
	glm::vec3 cameraPos;
	glm::vec3 cameraTarget;
	glm::vec3 cameraDirection;
	glm::vec3 up;
	glm::vec3 cameraRight;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	float cameraSpeed;
	float cameraYaw;
	float cameraPitch;
	float lastX, lastY;
	bool firstMouse;
	glm::mat4 view;
	glm::mat4 model;
	glm::mat4 projection;
	glm::mat4 mvp;
	glm::vec3 direction;
	GLuint shaderProgram;
	GLFWwindow* window;
	GLWrapper();
	int initializeGL();
	void runGLLoop();
	void bindGeometry(const GLfloat* vertices, const GLfloat* colors, int vsize, int csize);
	void setupVAO();
	void orientCamera();
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
};

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);


