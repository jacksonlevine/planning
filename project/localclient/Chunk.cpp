#include "Game.h"

Chunk::Chunk(Game* gref) {
	this->gref = gref;
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

void Chunk::rebuildMesh() {
	const int width = this->gref->chunkWidth;
	this->vertices.clear();
	this->colors.clear();
	for(int y = 0; y < width; y++)
	{
		for (int z = 0; z < width; z++)
		{
			for (int x = 0; x < width; x++)
			{
				intTup tup(this->x*this->gref->chunkWidth + x, this->y * this->gref->chunkWidth + y, this->z * this->gref->chunkWidth + z);
				if (this->gref->world.data.find(tup) != this->gref->world.data.end())
				{
					if (this->gref->world.data.find(intTup(tup.x -1 , tup.y, tup.z)) == this->gref->world.data.end())
					{
						this->vertices.insert(this->vertices.end(),
							{(GLfloat)tup.x, 
							(GLfloat)tup.y, 
							(GLfloat)tup.z});
						this->vertices.insert(this->vertices.end(),
							{ (GLfloat)(tup.x),
							(GLfloat)(tup.y),
							(GLfloat)(tup.z + 1) });
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
									(GLfloat)1.0,
									(GLfloat)0.0,
									(GLfloat)0.0
								});

						}
					}
					if (this->gref->world.data.find(intTup(tup.x, tup.y, tup.z-1)) == this->gref->world.data.end())
					{
						this->vertices.insert(this->vertices.end(),
							{ (GLfloat)(tup.x),
							(GLfloat)(tup.y),
							(GLfloat)(tup.z) });
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
							(GLfloat)(tup.z) });
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
					if (this->gref->world.data.find(intTup(tup.x + 1, tup.y, tup.z)) == this->gref->world.data.end())
					{
						this->vertices.insert(this->vertices.end(),
							{ (GLfloat)(tup.x+1),
							(GLfloat)(tup.y),
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
							{ (GLfloat)(tup.x + 1),
							(GLfloat)(tup.y),
							(GLfloat)(tup.z + 1) });
						this->vertices.insert(this->vertices.end(),
							{ (GLfloat)(tup.x + 1),
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
					if (this->gref->world.data.find(intTup(tup.x, tup.y, tup.z + 1)) == this->gref->world.data.end())
					{
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
							(GLfloat)(tup.y),
							(GLfloat)(tup.z + 1) });


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
					if (this->gref->world.data.find(intTup(tup.x, tup.y + 1, tup.z)) == this->gref->world.data.end())
					{
						this->vertices.insert(this->vertices.end(),
							{ (GLfloat)(tup.x),
							(GLfloat)(tup.y + 1),
							(GLfloat)(tup.z) });
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
							(GLfloat)(tup.y + 1),
							(GLfloat)(tup.z) });
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
							{ (GLfloat)(tup.x + 1),
							(GLfloat)(tup.y),
							(GLfloat)(tup.z) });
						this->vertices.insert(this->vertices.end(),
							{ (GLfloat)(tup.x + 1),
							(GLfloat)(tup.y),
							(GLfloat)(tup.z + 1) });

						this->vertices.insert(this->vertices.end(),
							{ (GLfloat)(tup.x + 1),
							(GLfloat)(tup.y),
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

void Chunk::moveAndRebuildMesh() {

}