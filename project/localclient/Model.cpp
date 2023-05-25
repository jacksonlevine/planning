#pragma once
#include "Model.hpp"

ModelShower::ModelShower() : active(false), length(0) {
	glGenBuffers(1, &this->vbov);
	glGenBuffers(1, &this->vboc);
	glGenBuffers(1, &this->vbouv);
}