#ifndef SHAPE_H
#define SHAPE_H

#include <GL/glew.h>
#include <vector>

#define EIGEN_DONT_VECTORIZE
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#include <Eigen/StdVector>
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Matrix2f)
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Matrix3f)
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Matrix3i)
#include <Eigen/Dense>

class Shader;

class Shape
{
public:
    Shape();

    void init(const std::vector<Eigen::Vector3f> &vertices, const std::vector<Eigen::Vector3i> &triangles);
    void setVertices(const std::vector<Eigen::Vector3f> &vertices);

    void setModelMatrix(const Eigen::Affine3f &model);

    void toggleWireframe();

    void draw(Shader *shader, GLenum mode);
    void select(Shader *shader,  Eigen::Vector3f start, Eigen::Vector3f ray, bool isAnchor);
    bool move(Eigen::Vector3f ray,Eigen::Vector3f start);

private:
    GLuint m_surfaceVao;
    GLuint m_surfaceVbo;
    GLuint m_surfaceIbo;

    unsigned int m_numSurfaceVertices;
    unsigned int m_numTetVertices;
    unsigned int m_verticesSize;
    float m_red;
    float m_blue;
    float m_green;
    float m_alpha;

    std::vector<Eigen::Vector3i> m_faces;
    std::vector<Eigen::Vector3f> m_vertices;
    std::vector<int> anchors;

    Eigen::Matrix4f m_modelMatrix;
    int lastSelected = -1;
    bool m_wireframe;

};

#endif // SHAPE_H
