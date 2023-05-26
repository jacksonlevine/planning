#pragma once
#include "Tree.hpp"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "BlockTypes.hpp"
#include "Maths.hpp"
enum TreeFlags {
    NO_LEAVES = false,
    LEAVES = true
};
glm::vec3 rotateAroundPoint(const glm::vec3& point, const glm::vec3& center, float angle)
{
    glm::vec3 translated = point - center;

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 rotated = glm::vec3(rotationMatrix * glm::vec4(translated, 1.0f));

    glm::vec3 finalPoint = rotated + center;

    return finalPoint;
}
void extrudeQuadFromPoints(std::vector<GLfloat>& vs, std::vector<GLfloat>& cs, std::vector<GLfloat>& uv, glm::vec3 lastVert, glm::vec3 nextLastVert, glm::vec3 direc, TextureFace stickFace) {
    TextureFace appleFace(7, 0);
    vs.insert(vs.end(), {
        lastVert.x, lastVert.y, lastVert.z,
        nextLastVert.x, nextLastVert.y, nextLastVert.z,
        nextLastVert.x + direc.x, nextLastVert.y + direc.y, nextLastVert.z + direc.z,

        nextLastVert.x + direc.x, nextLastVert.y + direc.y, nextLastVert.z + direc.z,
        lastVert.x + direc.x, lastVert.y + direc.y, lastVert.z + direc.z,
        lastVert.x, lastVert.y, lastVert.z,
        });

    for (int i = 0; i < 6; i++) {
        cs.insert(cs.end(), {
            1.0, 1.0, 1.0
            });
    }
    uv.insert(uv.end(), {
           stickFace.bl.x, stickFace.bl.y,
           stickFace.br.x, stickFace.br.y,
           stickFace.tr.x, stickFace.tr.y,

           stickFace.tr.x, stickFace.tr.y,
           stickFace.tl.x, stickFace.tl.y,
           stickFace.bl.x, stickFace.bl.y
        });

    glm::vec3 end = nextLastVert + glm::vec3(0.0f, -0.8, 0.0f) - (direc * 0.5f);

    vs.insert(vs.end(), {
        end.x + direc.x - 0.5f, end.y + direc.y, end.z + direc.z,
        end.x + direc.x + 0.5f, end.y + direc.y, end.z + direc.z,
        end.x + direc.x + 0.5f, end.y + direc.y + 1.0f, end.z + direc.z,

        end.x + direc.x + 0.5f, end.y + direc.y + 1.0f, end.z + direc.z,
        end.x + direc.x - 0.5f, end.y + direc.y + 1.0f, end.z + direc.z,
        end.x + direc.x - 0.5f, end.y + direc.y, end.z + direc.z,

        end.x + direc.x, end.y + direc.y, end.z + direc.z - 0.5f,
        end.x + direc.x, end.y + direc.y, end.z + direc.z + 0.5f,
        end.x + direc.x , end.y + direc.y + 1.0f, end.z + direc.z + 0.5f,

        end.x + direc.x, end.y + direc.y + 1.0f, end.z + direc.z + 0.5f,
        end.x + direc.x, end.y + direc.y + 1.0f, end.z + direc.z - 0.5f,
        end.x + direc.x, end.y + direc.y, end.z + direc.z - 0.5f,
        });
    for (int i = 0; i < 12; i++) {
        cs.insert(cs.end(), {
            1.0, 1.0, 1.0
            });
    }
    uv.insert(uv.end(), {
           appleFace.bl.x, appleFace.bl.y,
           appleFace.br.x, appleFace.br.y,
           appleFace.tr.x, appleFace.tr.y,

           appleFace.tr.x, appleFace.tr.y,
           appleFace.tl.x, appleFace.tl.y,
           appleFace.bl.x, appleFace.bl.y
        });
    uv.insert(uv.end(), {
           appleFace.bl.x, appleFace.bl.y,
           appleFace.br.x, appleFace.br.y,
           appleFace.tr.x, appleFace.tr.y,

           appleFace.tr.x, appleFace.tr.y,
           appleFace.tl.x, appleFace.tl.y,
           appleFace.bl.x, appleFace.bl.y
        });

}


void throwAStickOnHere(std::vector<GLfloat>& vs, std::vector<GLfloat>& cs, std::vector<GLfloat>& uv)
{
    TextureFace stickFace(4, 0);


    int posOrNeg = rando() < 0.5 ? -1 : 1;

    glm::vec3 direc(rando() * posOrNeg, rando(), rando() * posOrNeg);
    direc *= 1.5f;
    if (vs.size() >= 36) {
        glm::vec3 lastVert(vs[vs.size() - 3], vs[vs.size() - 2], vs[vs.size() - 1]);
        glm::vec3 nextLastVert(vs[vs.size() - 36], vs[vs.size() - 35], vs[vs.size() - 34]);
        extrudeQuadFromPoints(vs, cs, uv, lastVert, nextLastVert, direc, stickFace);
    }
}

void topItOffYogurt(std::vector<GLfloat>& vs, std::vector<GLfloat>& cs, std::vector<GLfloat>& uv)
{
    if (vs.size() > 6) {
        glm::vec3 top(vs[vs.size() - 6], vs[vs.size() - 5], vs[vs.size() - 4]);

        vs.insert(vs.end(), {
            top.x - 2, top.y, top.z,
            top.x + 2, top.y, top.z,
            top.x + 2, top.y + 4, top.z,

            top.x + 2, top.y + 4, top.z,
            top.x - 2, top.y + 4, top.z,
            top.x - 2, top.y, top.z,

            top.x, top.y,     top.z - 2,
            top.x, top.y,     top.z + 2,
            top.x, top.y + 4, top.z + 2,

            top.x, top.y + 4, top.z + 2,
            top.x, top.y + 4, top.z - 2,
            top.x, top.y,     top.z - 2
            });
        for (int i = 0; i < 12; i++)
        {
            cs.insert(cs.end(), {
                1.0, 1.0, 1.0
                });
        }
        TextureFace tl(5, 1);
        TextureFace bl(5, 0);
        TextureFace br(6, 0);
        TextureFace tr(6, 1);

        for (int i = 0; i < 2; i++)
        {
            uv.insert(uv.end(), {
                    bl.tl.x, bl.tl.y,
                    br.tr.x, br.tr.y,
                    tr.br.x, tr.br.y,

                    tr.br.x, tr.br.y,
                    tl.bl.x, tl.bl.y,
                    bl.tl.x, bl.tl.y,
                });
        }
    }
}
class TrianglePen {
public:
    glm::vec3 front;
    glm::vec3 bl;
    glm::vec3 br;
    TrianglePen(
        glm::vec3 front,
        glm::vec3 bl,
        glm::vec3 br
    ) :
        front(front),
        bl(bl),
        br(br)
    {

    }
    void move(glm::vec3 d)
    {
        front += d;
        bl += d;
        br += d;

    }
    void moveToPen(TrianglePen& pen)
    {
        front = pen.front;
        bl = pen.bl;
        br = pen.br;
    }
    void rotate(float ang)
    {
        glm::vec3 center = (front + bl + br) / 3.0f;
        float angle = glm::radians(ang);

        front = rotateAroundPoint(front, center, angle);
        bl = rotateAroundPoint(bl, center, angle);
        br = rotateAroundPoint(br, center, angle);
    }
};
void threadThesePens(
    TrianglePen& pen,
    TrianglePen& penBack,
    std::vector<GLfloat>& verts,
    std::vector<GLfloat>& cols,
    std::vector<GLfloat>& uvs,
    int length,
    glm::vec3 currentDirection,
    bool leaves
)
{

    TextureFace trunkFace(3, 0);
    for (int b = 0; b < length; b++)
    {

        int posOrNeg = rando() > 0.5 ? -1 : 1;

        currentDirection += glm::vec3((rando() * -posOrNeg) / 10, 0, (rando() * posOrNeg) / 10);

        pen.move(currentDirection * 2.0f);
        pen.rotate(-10.0f);


        verts.insert(verts.end(), {
            //left side
            penBack.bl.x, penBack.bl.y, penBack.bl.z,
            penBack.front.x, penBack.front.y, penBack.front.z,
            pen.front.x, pen.front.y, pen.front.z,

            pen.front.x, pen.front.y, pen.front.z,
            pen.bl.x, pen.bl.y, pen.bl.z,
            penBack.bl.x, penBack.bl.y, penBack.bl.z,

            //right side
            penBack.front.x, penBack.front.y, penBack.front.z,
            penBack.br.x, penBack.br.y, penBack.br.z,
            pen.br.x, pen.br.y, pen.br.z,

            pen.br.x, pen.br.y, pen.br.z,
            pen.front.x, pen.front.y, pen.front.z,
            penBack.front.x, penBack.front.y, penBack.front.z,

            //back side
            penBack.br.x, penBack.br.y, penBack.br.z,
            penBack.bl.x, penBack.bl.y, penBack.bl.z,
            pen.bl.x, pen.bl.y, pen.bl.z,

            pen.bl.x, pen.bl.y, pen.bl.z,
            pen.br.x, pen.br.y, pen.br.z,
            penBack.br.x, penBack.br.y, penBack.br.z,

            });


        for (int i = 0; i < 18; i++)
        {
            //glm::vec2 centerOfLast = (glm::vec2(penBack.front.x, penBack.front.z) + glm::vec2(penBack.bl.x, penBack.bl.z) + glm::vec2(penBack.br.x, penBack.br.z)) / 3.0f;

            //float xzoffset = glm::distance(glm::vec2(verts[i], verts[i + 2]), centerOfLast);

            //float minY = std::min(pen.front.y)
            cols.insert(cols.end(), { 1.0, 1.0, 1.0 });

        }
        penBack.moveToPen(pen);
        for (int i = 0; i < 3; i++)
        {
            uvs.insert(uvs.end(), {
                trunkFace.bl.x, trunkFace.bl.y,
                trunkFace.br.x, trunkFace.br.y,
                trunkFace.tr.x, trunkFace.tr.y,

                trunkFace.tr.x, trunkFace.tr.y,
                trunkFace.tl.x, trunkFace.tl.y,
                trunkFace.bl.x, trunkFace.bl.y,

                });

        }
        if (b == length - 1)
        {
            if (leaves == LEAVES)
            {
                topItOffYogurt(verts, cols, uvs);
            }
        }
        else
        {
            if (verts.size() > 50 && leaves == true && rando() < 0.05)
            {

                throwAStickOnHere(verts, cols, uvs);

            }
        }
    }

}

Model Tree::getTreeModel(float x, float y, float z)
{
	Model model;
    TrianglePen pen(
        glm::vec3(0.0 + x, 0.0 + y, 0.0 + z),
        glm::vec3(-0.5 + x, 0.0 + y, 0.7 + z),
        glm::vec3(0.5 + x, 0.0 + y, 0.8 + z)
    );

    TrianglePen penBack(
        glm::vec3(0.0 + x, 0.0 + y, 0.0 + z),
        glm::vec3(-0.5 + x, 0.0 + y, 0.7 + z),
        glm::vec3(0.5 + x, 0.0 + y, 0.8 + z)
    );



    int posOrNeg2 = rando() > 0.5 ? -1 : 1;

    glm::vec3 currentDirection(0, 0.5f, 0);
    threadThesePens(pen, penBack, model.verts, model.cols, model.uvs, rando() * 4, currentDirection, NO_LEAVES);


    glm::vec3 tbCenter = (pen.bl + pen.br) / 2.0f;
    int leftBranches = rando() * 8.0;
    int rightBranches = rando() * 8.0;

    for (int z = 0; z < leftBranches; z++)
    {
        int branchLength = rando() * 7;
        glm::vec3 currentDirection2(-rando() / 3 + (rando() / 10) * posOrNeg2, 0.5f, -(rando() / 2));

        TrianglePen pen2(
            pen.front,
            pen.bl,
            tbCenter
        );

        TrianglePen pen2Back(
            pen.front,
            pen.bl,
            tbCenter
        );

        threadThesePens(pen2, pen2Back, model.verts, model.cols, model.uvs, branchLength, currentDirection2, LEAVES);
    }
    for (int z = 0; z < rightBranches; z++)
    {
        int branchLength = rando() * 7;
        glm::vec3 currentDirection3(rando() / 3 + (rando() / 10) * posOrNeg2, 0.5f, (rando() / 2));
        TrianglePen pen3(
            pen.front,
            tbCenter,
            pen.br
        );
        TrianglePen pen3Back(
            pen.front,
            tbCenter,
            pen.br
        );
        threadThesePens(pen3, pen3Back, model.verts, model.cols, model.uvs, branchLength, currentDirection3, LEAVES);
    }
    return model;
}