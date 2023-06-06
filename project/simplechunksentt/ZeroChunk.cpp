#pragma once
#include "SimpleChunk.hpp"
#include "BlockTypes.hpp"
#include "Perlin.h"
#include "Scene.hpp"
#include "entt/entt.hpp"
perlin p4;
ZeroChunk::ZeroChunk(Game* g) :
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
bool ZeroChunk::operator==(const intTup& other) const {
	return (x == other.x) && (z == other.z);
}

void ZeroChunk::rebuildMesh()
{
	std::vector<GLfloat>().swap(this->verts);
	std::vector<GLfloat>().swap(this->cols);
	std::vector<GLfloat>().swap(this->uvs);

	std::function<void(glm::vec3*, intTup, float)> addColor = [](glm::vec3* color, intTup tup, float vol) {

		color->r += vol * p4.noise(tup.x / 24.3, tup.z / 24.3);
		color->g += vol * p4.noise(tup.x / 48.3, tup.z / 48.3);
		color->b += vol * p4.noise(tup.x / 32.3, tup.z / 32.3);
	};

	intTup overallTup(this->x, this->z);




			intTup frontLeft(this->x * CHUNK_WIDTH, this->z * CHUNK_WIDTH);
			intTup frontRight(this->x * CHUNK_WIDTH + CHUNK_WIDTH, this->z * CHUNK_WIDTH);
			intTup backRight(this->x * CHUNK_WIDTH + CHUNK_WIDTH, this->z * CHUNK_WIDTH + CHUNK_WIDTH);
			intTup backLeft(this->x * CHUNK_WIDTH, this->z * CHUNK_WIDTH + CHUNK_WIDTH);
			while (this->gref->world.heights.find(frontLeft) == this->gref->world.heights.end())
			{
				this->gref->world.generateOneChunk(intTup(this->x, this->z));
				//this->rebuildMesh();
			}

			HeightTile& fl = this->gref->world.heights.at(frontLeft);
			HeightTile& fr = this->gref->world.heights.at(frontRight);
			HeightTile& br = this->gref->world.heights.at(backRight);
			HeightTile& bl = this->gref->world.heights.at(backLeft);

			uint8_t blockID = fl.blockID;

			//if (p3.noise(tup.x / 12.5f, tup.y / 12.5f, tup.z / 12.5f) * 10 > 5) blockID = 2;

			verts.insert(verts.end(), {
				(float)(frontLeft.x), fl.height, (float)(frontLeft.z),
				(float)(frontRight.x), fr.height, (float)(frontRight.z),
				(float)(backRight.x), br.height, (float)(backRight.z) ,

				(float)(backRight.x), br.height, (float)(backRight.z) ,
				(float)(backLeft.x), bl.height, (float)(backLeft.z) + 1,
				(float)(frontLeft.x), fl.height, (float)(frontLeft.z)
				});
			TextureFace face = blockTypes[blockID].top;
			glm::vec3 color = blockTypes[blockID].baseColor;
			if (blockTypes[blockID].colorizable) {
				float volatility = blockTypes[blockID].colorVolatility;
				addColor(&color, frontLeft, volatility);
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


	this->length = verts.size();

	if (!this->active) {
		this->gref->activeZeroChunks.insert_or_assign(overallTup, *this);
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

void ZeroChunk::moveAndRebuildMesh(int x, int z) {
	this->x = x;
	this->z = z;
	this->rebuildMesh();
}