#include "Game.hpp"
#include "BlockTypes.hpp"
#include "Perlin.h"

perlin p2;

Chunk::Chunk(Game* gref) : active(false), gref(gref){
	this->x = 20000;
	this->y = 20000;
	this->z = 20000;
	this->me = this->gref->registry.create();
	this->dirty = true;
	this->bufferDeleted = true;
}

void Chunk::rebuildMesh() {
	const int width = this->gref->chunkWidth;

	std::vector<GLfloat>().swap(this->vertices);
	std::vector<GLfloat>().swap(this->colors);
	std::vector<GLfloat>().swap(this->uv);

	std::function<void(glm::vec3*, intTup, float)> addColor = [](glm::vec3* color, intTup tup, float vol) {

		color->r += vol * p2.noise(tup.x / 24.3, tup.z / 24.3);
		color->b += vol * p2.noise(tup.x / 48.3, tup.z / 48.3);
		color->b += vol * p2.noise(tup.x / 32.3, tup.z / 32.3);
	};

	intTup tup2(this->x, this->y, this->z);
	intTup tup1(this->x * CHUNK_WIDTH, this->y * CHUNK_WIDTH, this->z * CHUNK_WIDTH);

	/*
		if(this->gref->world.fullBlockMarks.find(tup2) != this->gref->world.fullBlockMarks.end())
	{
		this->vertices.insert(this->vertices.end(),
			{ (GLfloat)tup1.x,
			(GLfloat)tup1.y,
			(GLfloat)tup1.z,

			(GLfloat)(tup1.x),
			(GLfloat)(tup1.y +CHUNK_WIDTH),
			(GLfloat)(tup1.z),

			(GLfloat)(tup1.x),
			(GLfloat)(tup1.y +CHUNK_WIDTH),
			(GLfloat)(tup1.z +CHUNK_WIDTH),

			(GLfloat)(tup1.x),
			(GLfloat)(tup1.y +CHUNK_WIDTH),
			(GLfloat)(tup1.z +CHUNK_WIDTH),
			(GLfloat)(tup1.x),
			(GLfloat)(tup1.y),
			(GLfloat)(tup1.z +CHUNK_WIDTH),
			(GLfloat)(tup1.x),
			(GLfloat)(tup1.y),
			(GLfloat)(tup1.z) });
		for (int i = 0; i < 6; i++)
		{
			this->colors.insert(this->colors.end(),
				{
					(GLfloat)0.7,
					(GLfloat)0.0,
					(GLfloat)0.0
				});

		}
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)0.0,
				(GLfloat)1.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)0.0,
				(GLfloat)0.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)1.0,
				(GLfloat)0.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)1.0,
				(GLfloat)0.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)1.0,
				(GLfloat)1.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)0.0,
				(GLfloat)1.0,
			});
		this->vertices.insert(this->vertices.end(),
			{ (GLfloat)(tup1.x),
			(GLfloat)(tup1.y),
			(GLfloat)(tup1.z),
			(GLfloat)(tup1.x +CHUNK_WIDTH),
			(GLfloat)(tup1.y),
			(GLfloat)(tup1.z),
			(GLfloat)(tup1.x +CHUNK_WIDTH),
			(GLfloat)(tup1.y +CHUNK_WIDTH),
			(GLfloat)(tup1.z),
			(GLfloat)(tup1.x +CHUNK_WIDTH),
			(GLfloat)(tup1.y +CHUNK_WIDTH),
			(GLfloat)(tup1.z),
			(GLfloat)(tup1.x),
			(GLfloat)(tup1.y +CHUNK_WIDTH),
			(GLfloat)(tup1.z),
			(GLfloat)(tup1.x),
			(GLfloat)(tup1.y),
			(GLfloat)(tup1.z) });
		for (int i = 0; i < 6; i++)
		{
			this->colors.insert(this->colors.end(),
				{
					(GLfloat)0.6,
					(GLfloat)0.0,
					(GLfloat)0.0
				});

		}
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)0.0,
				(GLfloat)1.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)0.0,
				(GLfloat)0.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)1.0,
				(GLfloat)0.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)1.0,
				(GLfloat)0.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)1.0,
				(GLfloat)1.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)0.0,
				(GLfloat)1.0,
			});
		this->vertices.insert(this->vertices.end(),
			{ (GLfloat)(tup1.x +CHUNK_WIDTH),
			(GLfloat)(tup1.y),
			(GLfloat)(tup1.z),
			(GLfloat)(tup1.x +CHUNK_WIDTH),
			(GLfloat)(tup1.y),
			(GLfloat)(tup1.z +CHUNK_WIDTH),
			(GLfloat)(tup1.x +CHUNK_WIDTH),
			(GLfloat)(tup1.y +CHUNK_WIDTH),
			(GLfloat)(tup1.z +CHUNK_WIDTH),
			(GLfloat)(tup1.x +CHUNK_WIDTH),
			(GLfloat)(tup1.y +CHUNK_WIDTH),
			(GLfloat)(tup1.z +CHUNK_WIDTH),
			(GLfloat)(tup1.x +CHUNK_WIDTH),
			(GLfloat)(tup1.y +CHUNK_WIDTH),
			(GLfloat)(tup1.z),
			(GLfloat)(tup1.x +CHUNK_WIDTH),
			(GLfloat)(tup1.y),
			(GLfloat)(tup1.z) });

		for (int i = 0; i < 6; i++)
		{
			this->colors.insert(this->colors.end(),
				{
					(GLfloat)0.7,
					(GLfloat)0.0,
					(GLfloat)0.0
				});

		}
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)0.0,
				(GLfloat)1.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)0.0,
				(GLfloat)0.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)1.0,
				(GLfloat)0.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)1.0,
				(GLfloat)0.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)1.0,
				(GLfloat)1.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)0.0,
				(GLfloat)1.0,
			});
		this->vertices.insert(this->vertices.end(),
			{ (GLfloat)(tup1.x),
			(GLfloat)(tup1.y),
			(GLfloat)(tup1.z +CHUNK_WIDTH),
			(GLfloat)(tup1.x),
			(GLfloat)(tup1.y +CHUNK_WIDTH),
			(GLfloat)(tup1.z +CHUNK_WIDTH),
			(GLfloat)(tup1.x +CHUNK_WIDTH),
			(GLfloat)(tup1.y +CHUNK_WIDTH),
			(GLfloat)(tup1.z +CHUNK_WIDTH),
			(GLfloat)(tup1.x +CHUNK_WIDTH),
			(GLfloat)(tup1.y +CHUNK_WIDTH),
			(GLfloat)(tup1.z +CHUNK_WIDTH),
			(GLfloat)(tup1.x +CHUNK_WIDTH),
			(GLfloat)(tup1.y),
			(GLfloat)(tup1.z +CHUNK_WIDTH),
			(GLfloat)(tup1.x),
			(GLfloat)(tup1.y),
			(GLfloat)(tup1.z +CHUNK_WIDTH) });


		for (int i = 0; i < 6; i++)
		{
			this->colors.insert(this->colors.end(),
				{
					(GLfloat)0.6,
					(GLfloat)0.0,
					(GLfloat)0.0
				});

		}
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)0.0,
				(GLfloat)1.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)0.0,
				(GLfloat)0.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)1.0,
				(GLfloat)0.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)1.0,
				(GLfloat)0.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)1.0,
				(GLfloat)1.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)0.0,
				(GLfloat)1.0,
			});
		this->vertices.insert(this->vertices.end(),
			{ (GLfloat)(tup1.x),
			(GLfloat)(tup1.y +CHUNK_WIDTH),
			(GLfloat)(tup1.z),
			(GLfloat)(tup1.x +CHUNK_WIDTH),
			(GLfloat)(tup1.y +CHUNK_WIDTH),
			(GLfloat)(tup1.z),
			(GLfloat)(tup1.x +CHUNK_WIDTH),
			(GLfloat)(tup1.y +CHUNK_WIDTH),
			(GLfloat)(tup1.z +CHUNK_WIDTH),
			(GLfloat)(tup1.x +CHUNK_WIDTH),
			(GLfloat)(tup1.y +CHUNK_WIDTH),
			(GLfloat)(tup1.z +CHUNK_WIDTH),
			(GLfloat)(tup1.x),
			(GLfloat)(tup1.y +CHUNK_WIDTH),
			(GLfloat)(tup1.z +CHUNK_WIDTH),
			(GLfloat)(tup1.x),
			(GLfloat)(tup1.y +CHUNK_WIDTH),
			(GLfloat)(tup1.z) });

		for (int i = 0; i < 6; i++)
		{
			this->colors.insert(this->colors.end(),
				{
					(GLfloat)1.0,
					(GLfloat)0.0,
					(GLfloat)0.0
				});

		}
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)0.0,
				(GLfloat)1.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)0.0,
				(GLfloat)0.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)1.0,
				(GLfloat)0.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)1.0,
				(GLfloat)0.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)1.0,
				(GLfloat)1.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)0.0,
				(GLfloat)1.0,
			});
		this->vertices.insert(this->vertices.end(),
			{ (GLfloat)(tup1.x),
			(GLfloat)(tup1.y),
			(GLfloat)(tup1.z),
			(GLfloat)(tup1.x),
			(GLfloat)(tup1.y),
			(GLfloat)(tup1.z +CHUNK_WIDTH),
			(GLfloat)(tup1.x +CHUNK_WIDTH),
			(GLfloat)(tup1.y),
			(GLfloat)(tup1.z +CHUNK_WIDTH),
			(GLfloat)(tup1.x +CHUNK_WIDTH),
			(GLfloat)(tup1.y),
			(GLfloat)(tup1.z +CHUNK_WIDTH),
			(GLfloat)(tup1.x +CHUNK_WIDTH),
			(GLfloat)(tup1.y),
			(GLfloat)(tup1.z),
			(GLfloat)(tup1.x),
			(GLfloat)(tup1.y),
			(GLfloat)(tup1.z) });


		for (int i = 0; i < 6; i++)
		{
			this->colors.insert(this->colors.end(),
				{
					(GLfloat)1.0,
					(GLfloat)0.0,
					(GLfloat)0.0
				});

		}
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)0.0,
				(GLfloat)1.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)0.0,
				(GLfloat)0.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)1.0,
				(GLfloat)0.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)1.0,
				(GLfloat)0.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)1.0,
				(GLfloat)1.0,
			});
		this->uv.insert(this->uv.end(),
			{
				(GLfloat)0.0,
				(GLfloat)1.0,
			});
	}
	else
	*/
	{
		for (int y2 = 0; y2 < width; y2++)
		{
			for (int z2 = 0; z2 < width; z2++)
			{
				for (int x2 = 0; x2 < width; x2++)
				{
					intTup tup((this->x * CHUNK_WIDTH) +x2 , (this->y * CHUNK_WIDTH) + y2, (this->z * CHUNK_WIDTH) + z2);

					if (this->gref->world.data.find(tup) != this->gref->world.data.end())
					{
						uint8_t blockID = this->gref->world.data.at(tup);
						if (this->gref->world.data.find(intTup(tup.x - 1, tup.y, tup.z)) == this->gref->world.data.end())
						{
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)tup.x,
								(GLfloat)tup.y,
								(GLfloat)tup.z });
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x),
								(GLfloat)(tup.y + 1),
								(GLfloat)(tup.z) });
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x),
								(GLfloat)(tup.y + 1),
								(GLfloat)(tup.z + 1) });

							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x),
								(GLfloat)(tup.y + 1),
								(GLfloat)(tup.z + 1) });
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x),
								(GLfloat)(tup.y),
								(GLfloat)(tup.z + 1) });
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x),
								(GLfloat)(tup.y),
								(GLfloat)(tup.z) });

							//NEW CODE
							TextureFace face = blockTypes[blockID].allOneTexture ?
								blockTypes[blockID].all : blockTypes[blockID].sides;

							this->uv.insert(this->uv.end(),
								{
									(GLfloat)face.tl.x, (GLfloat)face.tl.y,
									(GLfloat)face.bl.x, (GLfloat)face.bl.y,
									(GLfloat)face.br.x, (GLfloat)face.br.y,

									(GLfloat)face.br.x, (GLfloat)face.br.y,
									(GLfloat)face.tr.x, (GLfloat)face.tr.y,
									(GLfloat)face.tl.x, (GLfloat)face.tl.y
								});
							glm::vec3 color = blockTypes[blockID].baseColor;
							if (blockTypes[blockID].colorizable) {
								float volatility = blockTypes[blockID].colorVolatility;
								addColor(&color, tup, volatility);
							}
							color = color * 0.7f;
							for (int i = 0; i < 6; i++)
							{
								this->colors.insert(this->colors.end(),
									{
										(GLfloat)color.r,
										(GLfloat)color.g,
										(GLfloat)color.b
									});

							}
						}
						if (this->gref->world.data.find(intTup(tup.x, tup.y, tup.z - 1)) == this->gref->world.data.end())
						{
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x),
								(GLfloat)(tup.y),
								(GLfloat)(tup.z) });
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x + 1),
								(GLfloat)(tup.y),
								(GLfloat)(tup.z) });
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x + 1),
								(GLfloat)(tup.y + 1),
								(GLfloat)(tup.z) });

							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x + 1),
								(GLfloat)(tup.y + 1),
								(GLfloat)(tup.z) });
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x),
								(GLfloat)(tup.y + 1),
								(GLfloat)(tup.z) });
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x),
								(GLfloat)(tup.y),
								(GLfloat)(tup.z) });
							//NEW CODE
							TextureFace face = blockTypes[blockID].allOneTexture ?
								blockTypes[blockID].all : blockTypes[blockID].sides;
							this->uv.insert(this->uv.end(),
								{
									(GLfloat)face.tl.x, (GLfloat)face.tl.y,
									(GLfloat)face.bl.x, (GLfloat)face.bl.y,
									(GLfloat)face.br.x, (GLfloat)face.br.y,

									(GLfloat)face.br.x, (GLfloat)face.br.y,
									(GLfloat)face.tr.x, (GLfloat)face.tr.y,
									(GLfloat)face.tl.x, (GLfloat)face.tl.y
								});
							glm::vec3 color = blockTypes[blockID].baseColor;
							if (blockTypes[blockID].colorizable) {
								float volatility = blockTypes[blockID].colorVolatility;
								addColor(&color, tup, volatility);
							}
							color = color * 0.6f;
							for (int i = 0; i < 6; i++)
							{
								this->colors.insert(this->colors.end(),
									{
										(GLfloat)color.r,
										(GLfloat)color.g,
										(GLfloat)color.b
									});

							}
						}
						if (this->gref->world.data.find(intTup(tup.x + 1, tup.y, tup.z)) == this->gref->world.data.end())
						{
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x + 1),
								(GLfloat)(tup.y),
								(GLfloat)(tup.z) });
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x + 1),
								(GLfloat)(tup.y),
								(GLfloat)(tup.z + 1) });
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x + 1),
								(GLfloat)(tup.y + 1),
								(GLfloat)(tup.z + 1) });

							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x + 1),
								(GLfloat)(tup.y + 1),
								(GLfloat)(tup.z + 1) });
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x + 1),
								(GLfloat)(tup.y + 1),
								(GLfloat)(tup.z) });
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x + 1),
								(GLfloat)(tup.y),
								(GLfloat)(tup.z) });
							//NEW CODE
							TextureFace face = blockTypes[blockID].allOneTexture ?
								blockTypes[blockID].all : blockTypes[blockID].sides;
							this->uv.insert(this->uv.end(),
								{
									(GLfloat)face.tl.x, (GLfloat)face.tl.y,
									(GLfloat)face.bl.x, (GLfloat)face.bl.y,
									(GLfloat)face.br.x, (GLfloat)face.br.y,

									(GLfloat)face.br.x, (GLfloat)face.br.y,
									(GLfloat)face.tr.x, (GLfloat)face.tr.y,
									(GLfloat)face.tl.x, (GLfloat)face.tl.y
								});
							glm::vec3 color = blockTypes[blockID].baseColor;
							if (blockTypes[blockID].colorizable) {
								float volatility = blockTypes[blockID].colorVolatility;
								addColor(&color, tup, volatility);
							}
							color = color * 0.7f;
							for (int i = 0; i < 6; i++)
							{
								this->colors.insert(this->colors.end(),
									{
										(GLfloat)color.r,
										(GLfloat)color.g,
										(GLfloat)color.b
									});

							}
						}
						if (this->gref->world.data.find(intTup(tup.x, tup.y, tup.z + 1)) == this->gref->world.data.end())
						{
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x),
								(GLfloat)(tup.y),
								(GLfloat)(tup.z + 1) });
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x),
								(GLfloat)(tup.y + 1),
								(GLfloat)(tup.z + 1) });
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x + 1),
								(GLfloat)(tup.y + 1),
								(GLfloat)(tup.z + 1) });

							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x + 1),
								(GLfloat)(tup.y + 1),
								(GLfloat)(tup.z + 1) });
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x + 1),
								(GLfloat)(tup.y),
								(GLfloat)(tup.z + 1) });
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x),
								(GLfloat)(tup.y),
								(GLfloat)(tup.z + 1) });

							//NEW CODE
							TextureFace face = blockTypes[blockID].allOneTexture ?
								blockTypes[blockID].all : blockTypes[blockID].sides;
							this->uv.insert(this->uv.end(),
								{
									(GLfloat)face.tl.x, (GLfloat)face.tl.y,
									(GLfloat)face.bl.x, (GLfloat)face.bl.y,
									(GLfloat)face.br.x, (GLfloat)face.br.y,

									(GLfloat)face.br.x, (GLfloat)face.br.y,
									(GLfloat)face.tr.x, (GLfloat)face.tr.y,
									(GLfloat)face.tl.x, (GLfloat)face.tl.y
								});
							glm::vec3 color = blockTypes[blockID].baseColor;
							if (blockTypes[blockID].colorizable) {
								float volatility = blockTypes[blockID].colorVolatility;
								addColor(&color, tup, volatility);
							}
							color = color * 0.6f;
							for (int i = 0; i < 6; i++)
							{
								this->colors.insert(this->colors.end(),
									{
										(GLfloat)color.r,
										(GLfloat)color.g,
										(GLfloat)color.b
									});

							}
						}
						if (this->gref->world.data.find(intTup(tup.x, tup.y + 1, tup.z)) == this->gref->world.data.end())
						{
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x),
								(GLfloat)(tup.y + 1),
								(GLfloat)(tup.z) });
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x + 1),
								(GLfloat)(tup.y + 1),
								(GLfloat)(tup.z) });
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x + 1),
								(GLfloat)(tup.y + 1),
								(GLfloat)(tup.z + 1) });

							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x + 1),
								(GLfloat)(tup.y + 1),
								(GLfloat)(tup.z + 1) });
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x),
								(GLfloat)(tup.y + 1),
								(GLfloat)(tup.z + 1) });
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x),
								(GLfloat)(tup.y + 1),
								(GLfloat)(tup.z) });
							//NEW CODE
							TextureFace face = blockTypes[blockID].allOneTexture ?
								blockTypes[blockID].all : blockTypes[blockID].top;
							this->uv.insert(this->uv.end(),
								{
									(GLfloat)face.tl.x, (GLfloat)face.tl.y,
									(GLfloat)face.bl.x, (GLfloat)face.bl.y,
									(GLfloat)face.br.x, (GLfloat)face.br.y,

									(GLfloat)face.br.x, (GLfloat)face.br.y,
									(GLfloat)face.tr.x, (GLfloat)face.tr.y,
									(GLfloat)face.tl.x, (GLfloat)face.tl.y
								});
							glm::vec3 color = blockTypes[blockID].baseColor;
							if (blockTypes[blockID].colorizable) {
								float volatility = blockTypes[blockID].colorVolatility;
								addColor(&color, tup, volatility);
							}

							for (int i = 0; i < 6; i++)
							{
								this->colors.insert(this->colors.end(),
									{
										(GLfloat)color.r,
										(GLfloat)color.g,
										(GLfloat)color.b
									});

							}
						}
						if (this->gref->world.data.find(intTup(tup.x, tup.y - 1, tup.z)) == this->gref->world.data.end())
						{
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x),
								(GLfloat)(tup.y),
								(GLfloat)(tup.z) });

							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x),
								(GLfloat)(tup.y),
								(GLfloat)(tup.z + 1) });

							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x + 1),
								(GLfloat)(tup.y),
								(GLfloat)(tup.z + 1) });

							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x + 1),
								(GLfloat)(tup.y),
								(GLfloat)(tup.z + 1) });
							
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x + 1),
								(GLfloat)(tup.y),
								(GLfloat)(tup.z) });
							
							this->vertices.insert(this->vertices.end(),
								{ (GLfloat)(tup.x),
								(GLfloat)(tup.y),
								(GLfloat)(tup.z) });
							//NEW CODE
							TextureFace face = blockTypes[blockID].allOneTexture ?
								blockTypes[blockID].all : blockTypes[blockID].bottom;
							this->uv.insert(this->uv.end(),
								{
									(GLfloat)face.tl.x, (GLfloat)face.tl.y,
									(GLfloat)face.bl.x, (GLfloat)face.bl.y,
									(GLfloat)face.br.x, (GLfloat)face.br.y,

									(GLfloat)face.br.x, (GLfloat)face.br.y,
									(GLfloat)face.tr.x, (GLfloat)face.tr.y,
									(GLfloat)face.tl.x, (GLfloat)face.tl.y
								});

							glm::vec3 color = blockTypes[blockID].baseColor;
							if (blockTypes[blockID].colorizable) {
								float volatility = blockTypes[blockID].colorVolatility;
								addColor(&color, tup, volatility);
							}

							for (int i = 0; i < 6; i++)
							{
								this->colors.insert(this->colors.end(),
									{
										(GLfloat)color.r,
										(GLfloat)color.g,
										(GLfloat)color.b
									});

							}
							
						}
					}
				}
			}
		}
	}
	//this->dirty = true;
	if (!this->active) {
		this->gref->activeChunks.insert_or_assign(tup2, *this);
		this->active = true;
	}
	if (!this->gref->registry.all_of<MeshComponent>(this->me))
	{
		MeshComponent m;
		m.length = vertices.size();
		this->gref->wrap->bindGeometry(
			m.vbov,
			m.vboc,
			m.vbouv,
			&(this->vertices[0]),
			&(this->colors[0]),
			&(this->uv[0]),
			this->vertices.size() * sizeof(GLfloat),
			this->colors.size() * sizeof(GLfloat),
			this->uv.size() * sizeof(GLfloat)
		);
		this->gref->registry.emplace<MeshComponent>(this->me, m);
	}
	else {
		MeshComponent& m = this->gref->registry.get<MeshComponent>(this->me);
		m.length = vertices.size();
		this->gref->wrap->bindGeometry(
			m.vbov,
			m.vboc,
			m.vbouv,
			&(this->vertices[0]),
			&(this->colors[0]),
			&(this->uv[0]),
			this->vertices.size() * sizeof(GLfloat),
			this->colors.size() * sizeof(GLfloat),
			this->uv.size() * sizeof(GLfloat)
		);
	}
}

void Chunk::moveAndRebuildMesh(int newX, int newY, int newZ) {
	this->x = newX;
	this->y = newY;
	this->z = newZ;
	this->rebuildMesh();
}