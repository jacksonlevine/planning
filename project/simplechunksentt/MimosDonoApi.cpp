#include "MimosDonoApi.hpp"


std::shared_ptr<MimosDonoApi> MimosDonoApi::instance = nullptr;

MimosDonoApi::MimosDonoApi(Game& g) : game(g) {
}

void MimosDonoApi::setClientPosition(float x, float y, float z)
{
	this->game.wrap->cameraPos = glm::vec3(x, y, z);
}

std::tuple<float, float, float> MimosDonoApi::getClientPosition()
{
	glm::vec3 vec = this->game.wrap->cameraPos;
	return std::make_tuple(vec.x, vec.y, vec.z);
}

void MimosDonoApi::addNewModel(std::vector<GLfloat> verts, std::vector<GLfloat> uvs) {
	// Your implementation here...
}


void MimosDonoApi::_initializePython()
{
	//pybind11::module_ mimosdono = pybind11::module_("mimosdono");
	//Make sure the interpreters running ebfoer this
	pybind11::class_<MimosDonoApi>(pybind11::module::import("__main__"), "MimosDonoApi")
			.def("setClientPosition", &MimosDonoApi::setClientPosition)
			.def("getClientPosition", &MimosDonoApi::getClientPosition);

	pybind11::globals()["mdma"] = pybind11::cast(MimosDonoApi::instance);

	
}


