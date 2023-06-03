#pragma once
#include "SimpleChunk.hpp"
#include "BlockTypes.hpp"
#include "Perlin.h"
#include "Scene.hpp"
#include "entt/entt.hpp"
perlin p3;
SimpleChunk::SimpleChunk(Game* g) :
	gref(g),
	x(50000),
	z(50000),
	active(false),
	length(0)
{
	/*glGenBuffers(1, &this->vbov);
	glGenBuffers(1, &this->vboc);
	glGenBuffers(1, &this->vbouv);*/
	this->me = this->gref->registry.create();
}
bool SimpleChunk::operator==(const intTup& other) const {
	return (x == other.x) && (z == other.z);
}

void SimpleChunk::rebuildMesh()
{
	std::vector<GLfloat>().swap(this->verts);
	std::vector<GLfloat>().swap(this->cols);
	std::vector<GLfloat>().swap(this->uvs);

	std::function<void(glm::vec3*, intTup, float)> addColor = [](glm::vec3* color, intTup tup, float vol) {

		color->r += vol * p3.noise(tup.x / 24.3, tup.z / 24.3);
		color->g += vol * p3.noise(tup.x / 48.3, tup.z / 48.3);
		color->b += vol * p3.noise(tup.x / 32.3, tup.z / 32.3);
	};

	intTup overallTup(this->x, this->z);


	for (int i = 0; i < CHUNK_WIDTH; i++)
	{
		for (int k = 0; k < CHUNK_WIDTH; k++)
		{

			intTup tup((this->x * CHUNK_WIDTH) + i, (this->z * CHUNK_WIDTH) + k);

			while (this->gref->world.heights.find(tup) == this->gref->world.heights.end())
			{
				this->gref->world.generateOneChunk(intTup(this->x, this->z));
				//this->rebuildMesh();
			}

			if (this->gref->world.models.find(tup) != this->gref->world.models.end()) {
				ModelShower grabbed = *(this->gref->modelShowerPool.begin());
				this->gref->modelShowerPool.erase(this->gref->modelShowerPool.begin());
				Model& model = this->gref->world.models.at(tup);
				grabbed.length = model.verts.size();
				
				grabbed.setMesh(model.verts, model.cols, model.uvs);

				this->gref->modelShowerPool.push_back(grabbed);
			}
			HeightTile& tile = this->gref->world.heights.at(tup);
			uint8_t blockID = tile.blockID;

			//if (p3.noise(tup.x / 12.5f, tup.y / 12.5f, tup.z / 12.5f) * 10 > 5) blockID = 2;

			verts.insert(verts.end(), {
				(float)(tup.x), tile.height, (float)(tup.z),
				(float)(tup.x) + 1, this->gref->world.heights.at(tup + intTup(1,0)).height, (float)(tup.z),
				(float)(tup.x) + 1, this->gref->world.heights.at(tup + intTup(1,1)).height, (float)(tup.z) + 1,

				(float)(tup.x) + 1, this->gref->world.heights.at(tup + intTup(1,1)).height, (float)(tup.z) + 1,
				(float)(tup.x), this->gref->world.heights.at(tup + intTup(0,1)).height, (float)(tup.z) + 1,
				(float)(tup.x), tile.height, (float)(tup.z),
				});
			TextureFace face = blockTypes[blockID].top;
			glm::vec3 color = blockTypes[blockID].baseColor;
			if (blockTypes[blockID].colorizable) {
				float volatility = blockTypes[blockID].colorVolatility;
				addColor(&color, tup, volatility);
			}
			cols.insert(cols.end(), {
					color.r, color.g, color.b,
					color.r, color.g, color.b,
					color.r, color.g, color.b,

					color.r, color.g, color.b,
					color.r, color.g, color.b,
					color.r, color.g, color.b,
				});
			uvs.insert(uvs.end(), {
					(GLfloat)face.tl.x, (GLfloat)face.tl.y,
					(GLfloat)face.bl.x, (GLfloat)face.bl.y,
					(GLfloat)face.br.x, (GLfloat)face.br.y,

					(GLfloat)face.br.x, (GLfloat)face.br.y,
					(GLfloat)face.tr.x, (GLfloat)face.tr.y,
					(GLfloat)face.tl.x, (GLfloat)face.tl.y
				});
		}
	}

	this->length = verts.size();

	if (!this->active) {
		this->gref->activeSimpChunks.insert_or_assign(overallTup, *this);
		this->active = true;
	}
	if (verts.size() > 0) {

		if (!this->gref->registry.all_of<MeshComponent>(this->me))
		{
			MeshComponent m;
			m.length = verts.size();
			this->gref->wrap->bindGeometry(
				m.vbov,
				m.vboc,
				m.vbouv,
				&(this->verts[0]),
				&(this->cols[0]),
				&(this->uvs[0]),
				verts.size() * sizeof(GLfloat),
				cols.size() * sizeof(GLfloat),
				uvs.size() * sizeof(GLfloat)
			);
			this->gref->registry.emplace<MeshComponent>(this->me, m);
		}
		else {
			MeshComponent& m = this->gref->registry.get<MeshComponent>(this->me);
			m.length = verts.size();
			this->gref->wrap->bindGeometry(
				m.vbov,
				m.vboc,
				m.vbouv,
				&(this->verts[0]),
				&(this->cols[0]),
				&(this->uvs[0]),
				verts.size() * sizeof(GLfloat),
				cols.size() * sizeof(GLfloat),
				uvs.size() * sizeof(GLfloat)
			);
		}
	}
}

void SimpleChunk::moveAndRebuildMesh(int x, int z) {
	this->x = x;
	this->z = z;
	this->rebuildMesh();
}