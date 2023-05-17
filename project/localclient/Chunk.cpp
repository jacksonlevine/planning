#include "Game.h"

Chunk::Chunk(Game* gref) {
	this->gref = gref;
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->vboc = (GLuint)0;
	this->vbov = (GLuint)0;
	glGenBuffers(1,&(this->vbov));
	glGenBuffers(1,&(this->vboc));
}

void Chunk::rebuildMesh() {
	const int width = this->gref->chunkWidth;

	std::vector<GLfloat>().swap(this->vertices);
	std::vector<GLfloat>().swap(this->colors);

	intTup tup2(this->x, this->y, this->z);
	intTup tup1(this->x * CHUNK_WIDTH, this->y * CHUNK_WIDTH, this->z * CHUNK_WIDTH);
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
	}
	else
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
							for (int i = 0; i < 6; i++)
							{
								this->colors.insert(this->colors.end(),
									{
										(GLfloat)0.7,
										(GLfloat)0.0,
										(GLfloat)0.0
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
							for (int i = 0; i < 6; i++)
							{
								this->colors.insert(this->colors.end(),
									{
										(GLfloat)0.6,
										(GLfloat)0.0,
										(GLfloat)0.0
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

							for (int i = 0; i < 6; i++)
							{
								this->colors.insert(this->colors.end(),
									{
										(GLfloat)0.7,
										(GLfloat)0.0,
										(GLfloat)0.0
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


							for (int i = 0; i < 6; i++)
							{
								this->colors.insert(this->colors.end(),
									{
										(GLfloat)0.6,
										(GLfloat)0.0,
										(GLfloat)0.0
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

							for (int i = 0; i < 6; i++)
							{
								this->colors.insert(this->colors.end(),
									{
										(GLfloat)1.0,
										(GLfloat)0.0,
										(GLfloat)0.0
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


							for (int i = 0; i < 6; i++)
							{
								this->colors.insert(this->colors.end(),
									{
										(GLfloat)1.0,
										(GLfloat)0.0,
										(GLfloat)0.0
									});

							}
						}
					}
				}
			}
		}
	}
}

void Chunk::moveAndRebuildMesh(int newX, int newY, int newZ) {
	this->x = newX;
	this->y = newY;
	this->z = newZ;
	this->rebuildMesh();
}