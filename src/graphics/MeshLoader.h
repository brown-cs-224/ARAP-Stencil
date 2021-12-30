#ifndef MESHLOADER_H
#define MESHLOADER_H

#include <vector>
#include <Eigen/Dense>
#include <Eigen/StdVector>

EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Matrix4i)

class MeshLoader
{
public:
    static bool loadTriMesh(const std::string &filepath, std::vector<Eigen::Vector3f> &vertices,  std::vector<Eigen::Vector3f> &normals, std::vector<Eigen::Vector3i> &faces);
private:
    MeshLoader();
};

#endif // MESHLOADER_H
