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

    void init(Eigen::Vector3f &min, Eigen::Vector3f &max);
    void draw(Shader *shader, GLenum mode);
    void select(Shader *shader, int vertex);
    void toggleWire();
    void move(int vertex, Eigen::Vector3f pos);
    int getClosestVertex(Eigen::Vector3f ray, Eigen::Vector3f start);
    bool getAnchorPos(int lastSelected, Eigen::Vector3f& pos,
                                 Eigen::Vector3f ray, Eigen::Vector3f start);
private:
    Shape m_shape;
};

#endif // ARAP_H
