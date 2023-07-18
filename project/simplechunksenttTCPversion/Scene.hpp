#pragma once
#include "GL/glew.h"

struct MeshComponent {
public:
	GLuint vbov;
	GLuint vboc;
	GLuint vbouv;
	int length;
	MeshComponent();
};