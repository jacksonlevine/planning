#include "Hud.hpp"

HudView::HudView() : dirty(true) {

}

void HudView::updateAmalgam()
{
	std::vector<GLfloat>().swap(this->amalgam.uvs);
	std::vector<GLfloat>().swap(this->amalgam.verts);
	for (HudRect& rect : this->rects)
	{
		this->amalgam.verts.insert(this->amalgam.verts.end(), rect.verts.begin(), rect.verts.end());
		this->amalgam.uvs.insert(this->amalgam.uvs.end(), rect.uvs.begin(), rect.uvs.end());
	}
	this->dirty = false;
}


Button::Button(float x, float y)
{
	float z = 0.5;
	float width = 0.25;
	float height = 0.25;
	this->verts.insert(this->verts.end(), {
			x, y, z,
			x + width, y, z,
			x + width, y + height, z,

			x + width, y + height, z,
			x, y + height, z,
			x, y, z
		});
	this->uvs.insert(this->uvs.end(), {
			0.0f            , (1.0f / 16.0f),
			1.0f / 8.0f,           (1.0f / 16.0f),
			1.0f / 8.0f,             0.0f,

			1.0f / 8.0f,           0.0f,
			0.0f,                  0.0f,
			0.0f,             (1.0f / 16.0f)
		});
}
