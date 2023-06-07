#include "Hud.hpp"

HudView::HudView() : dirty(false) {

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