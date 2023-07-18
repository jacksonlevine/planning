#pragma once
#include "Model.hpp"

ModelShower::ModelShower(Game* g) : active(false), length(0), gref(g) {
	
	this->me = this->gref->registry.create();

}


void ModelShower::setMesh(std::vector<GLfloat>& verts, std::vector<GLfloat>& cols, std::vector<GLfloat>& uvs) {
	if (verts.size() > 0) {

		if (!this->gref->registry.all_of<MeshComponent>(this->me))
		{
			MeshComponent m;

			this->gref->registry.emplace<MeshComponent>(this->me, m);
		}

		MeshComponent& m = this->gref->registry.get<MeshComponent>(this->me);
		m.length = verts.size();
		this->gref->wrap->bindGeometry(
			m.vbov,
			m.vboc,
			m.vbouv,
			&(verts[0]),
			&(cols[0]),
			&(uvs[0]),
			sizeof(GLfloat) * verts.size(),
			sizeof(GLfloat) * cols.size(),
			sizeof(GLfloat) * uvs.size()
		);
	}
}