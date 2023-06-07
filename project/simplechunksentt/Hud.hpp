#include <vector>
#include <GL/glew.h>
class HudRect
{
public:
	std::vector<GLfloat> verts;
	std::vector<GLfloat> uvs;
};

class HudView
{
private:
	HudRect amalgam;
public:
	GLuint vbov;
	GLuint vbouv;
	bool dirty;
	std::vector<HudRect> rects;
	HudView();
	HudRect getAmalgam();
};

HudRect HudView::getAmalgam()
{
	std::vector<GLfloat>().swap(this->amalgam.uvs);
	std::vector<GLfloat>().swap(this->amalgam.verts);
	for (HudRect& rect : this->rects)
	{
		this->amalgam.verts.insert(this->amalgam.verts.end(), rect.verts.begin(), rect.verts.end());
		this->amalgam.uvs.insert(this->amalgam.uvs.end(), rect.uvs.begin(), rect.uvs.end());
	}
}