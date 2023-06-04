#include "Scene.hpp"

MeshComponent::MeshComponent() : length(0) {
	glGenBuffers(1, &this->vbov);
	glGenBuffers(1, &this->vboc);
	glGenBuffers(1, &this->vbouv);
}