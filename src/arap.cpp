#include "arap.h"

#include <iostream>
#include <set>
#include <map>
#include "graphics/MeshLoader.h"
#include<vector>
using namespace Eigen;

ARAP::ARAP(){}

void ARAP::init()
{
    // STUDENTS: This code loads up the tetrahedral mesh in 'example-meshes/single-tet.mesh'
    //    (note: your working directory must be set to the root directory of the starter code
    //    repo for this file to load correctly). You'll probably want to instead have this code
    //    load up a tet mesh based on e.g. a file path specified with a command line argument.
    std::vector<Vector3f> vertices;
    std::vector<Vector3f> normals;
    std::vector<Vector3i> tets;
    if(MeshLoader::loadTriMesh("/Users/gene/Desktop/arap/meshes/cow.obj", vertices, normals,tets)) {
        m_shape.init(vertices, tets);
    }
    //m_shape.setModelMatrix(Affine3f(Eigen::Translation3f(0, 5, 0)));
    std::vector<Vector3d> double_verts;
    for(auto particle: vertices) {
        double_verts.push_back(Vector3d((double)particle[0], (double)particle[1], (double)particle[2]));
    }
}


void ARAP::draw(Shader *shader, GLenum mode)
{
    m_shape.draw(shader, mode);
}

void ARAP::select(Shader *shader, Eigen::Vector3f start, Vector3f ray, bool isAnchor)
{
    m_shape.select(shader, start, ray, isAnchor);
}

bool ARAP::move(Vector3f ray, Eigen::Vector3f start)
{
    return m_shape.move(ray, start);
}

void ARAP::toggleWire()
{
    m_shape.toggleWireframe();
}
