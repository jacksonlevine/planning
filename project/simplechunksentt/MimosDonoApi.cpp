#include "MimosDonoApi.hpp"
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <python3.10/Python.h>

void MimosDonoApi::setClientPosition(float x, float y, float z)
{
	this->game.wrap->cameraPos = glm::vec3(x, y, z);
}

std::tuple<float, float, float> MimosDonoApi::getClientPosition()
{
	glm::vec3 vec = this->game.wrap->cameraPos;
	return std::make_tuple(vec.x, vec.y, vec.z);
}

PYBIND11_EMBEDDED_MODULE(mdapi, m) {
    pybind11::class_<MimosDonoApi>(m, "MimosDonoApi")
        .def("setClientPosition", &MimosDonoApi::setClientPosition)
        .def("getClientPosition", &MimosDonoApi::getClientPosition);
}
