#ifndef ARAP_H
#define ARAP_H

#include "graphics/shape.h"
#include <Eigen/StdList>
#include <Eigen/StdVector>
class Shader;

class ARAP
{
public:
    ARAP();

    void init();
    void draw(Shader *shader, GLenum mode);
    void select(Shader *shader, Eigen::Vector3f start, Eigen::Vector3f click, bool isAnchor);
    void toggleWire();
    bool move(Eigen::Vector3f ray, Eigen::Vector3f start);
private:
    Shape m_shape;
};

#endif // ARAP_H
