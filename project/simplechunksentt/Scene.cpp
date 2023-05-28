#include "Scene.hpp"

MeshComponent::MeshComponent() {
	glGenBuffers(1, &this->vbov);
	glGenBuffers(1, &this->vboc);
	glGenBuffers(1, &this->vbouv);
}