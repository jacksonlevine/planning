#include <vector>
#include <GL/glew.h>
class HudRect
{
public:
	std::vector<GLfloat> verts;
	std::vector<GLfloat> uvs;
};

class Button : public HudRect {
public:
	Button(float x, float y);
};

class HudView
{
public:
	HudRect amalgam;
	bool dirty;
	std::vector<HudRect> rects;
	HudView();
	void updateAmalgam();
};
