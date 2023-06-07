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
public:
	HudRect amalgam;
	GLuint vbov;
	GLuint vbouv;
	bool dirty;
	std::vector<HudRect> rects;
	HudView();
	void updateAmalgam();
};
