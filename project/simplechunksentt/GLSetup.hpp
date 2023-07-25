#pragma once
#include <iostream>
#include <format>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

struct InputState {
	bool forward;
	bool left;
	bool right;
	bool back;
	bool jump;
	float forwardVelocity;
	float upVelocity;
	float rightVelocity;
};

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

	int wi;
	int he;

	glm::mat4 projection;
	glm::mat4 mvp;
	glm::vec3 direction;
	GLuint shaderProgram;
	GLuint vao;
	GLFWwindow* window;
	float deltaTime;
	InputState activeState;
	GLWrapper();
	int initializeGL();
	void setFOV(int newF);
	void bindGeometry(GLuint vbov, GLuint vboc, GLuint vbouv, const GLfloat* vertices, const GLfloat* colors, const GLfloat* uv, int vsize, int csize, int usize);
	void bindGeometryNoUpload(GLuint vbov, GLuint vboc, GLuint vbouv);
	void setupVAO();
	void bindVAO();
	void orientCamera();
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static GLWrapper* instance;
};

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);


