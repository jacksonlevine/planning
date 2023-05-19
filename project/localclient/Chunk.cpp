#include "Game.h"
#include "BlockTypes.h"
#include "Perlin.h"

perlin p2;

Chunk::Chunk(Game* gref) {
	this->gref = gref;
	this->x = 20000;
	this->y = 20000;
	this->z = 20000;
	this->vboc = (GLuint)0;
	this->vbov = (GLuint)0;
	this->vbouv = (GLuint)0;
	this->dirty = true;
	this->bufferDeleted = true;
	glGenBuffers(1,&(this->vbov));
	glGenBuffers(1,&(this->vboc));
	glGenBuffers(1, &(this->vbouv));
}

void Chunk::rebuildMesh() {
	const int width = this->gref->chunkWidth;

	std::vector<GLfloat>().swap(this->vertices);
	std::vector<GLfloat>().swap(this->colors);
	std::vector<GLfloat>().swap(this->uv);

	std::function<void(glm::vec3*, intTup, float)> addColor = [](glm::vec3* color, intTup tup, float vol) {

		color->r += vol * p2.noise(tup.x / 12.3, tup.z / 12.3);
		color->b += vol * p2.noise(tup.x / 24.3, tup.z / 24.3);
		color->b += vol * p2.noise(tup.x / 16.3, tup.z / 16.3);
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
		for (int y = 0; y < width; y++)
		{
			for (int z = 0; z < width; z++)
			{
				for (int x = 0; x < width; x++)
				{
					intTup tup((this->x * this->gref->chunkWidth) + x, (this->y * this->gref->chunkWidth) + y, (this->z * this->gref->chunkWidth) + z);
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
	this->dirty = true;
}

void Chunk::moveAndRebuildMesh(int newX, int newY, int newZ) {
	this->x = newX;
	this->y = newY;
	this->z = newZ;
	this->rebuildMesh();
}