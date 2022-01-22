#include "arap.h"

#include <iostream>
#include <set>
#include <map>
#include "graphics/MeshLoader.h"
#include<vector>
using namespace Eigen;

ARAP::ARAP(){}

void ARAP::init(Eigen::Vector3f &min, Eigen::Vector3f &max)
{
    // STUDENTS: This code loads up the tetrahedral mesh in 'example-meshes/single-tet.mesh'
    //    (note: your working directory must be set to the root directory of the starter code
    //    repo for this file to load correctly). You'll probably want to instead have this code
    //    load up a tet mesh based on e.g. a file path specified with a command line argument.
    std::vector<Vector3f> vertices;
    std::vector<Vector3f> normals;
    std::vector<Vector3i> tets;
    if(MeshLoader::loadTriMesh("/Users/blinnbryce/Documents/GitHub/ARAP-stencil/meshes/cow.obj", vertices, normals,tets)) {
        m_shape.init(vertices, tets);
    }
    
    MatrixX3f all_vertices = MatrixX3f(vertices.size(),3);
    std::vector<Vector3d> double_verts;
    int i = 0;
    for(auto particle: vertices) {
        double_verts.push_back(Vector3d((double)particle[0], (double)particle[1], (double)particle[2]));
        all_vertices.row(i) = particle;
        i++;
    }
    min = all_vertices.colwise().minCoeff();
    max = all_vertices.colwise().maxCoeff();
}


void ARAP::move(int vertex, Vector3f pos)
{
    std::vector<Eigen::Vector3f> new_vertices = m_shape.getVertices();
    const std::unordered_set<int>& anchors = m_shape.getAnchors();

    //TODO: implement ARAP here
    new_vertices[vertex] = pos;

    m_shape.setVertices(new_vertices);

}
//////////////////// No need to edit after this /////////////////////////
int ARAP::getClosestVertex(Eigen::Vector3f start, Eigen::Vector3f ray){
    return m_shape.getClosestVertex(start, ray);
}


void ARAP::draw(Shader *shader, GLenum mode)
{
    m_shape.draw(shader, mode);
}

void ARAP::select(Shader *shader, int vertex)
{
    m_shape.select(shader, vertex);
}


void ARAP::toggleWire()
{
    m_shape.toggleWireframe();
}

bool ARAP::getAnchorPos(int lastSelected, Eigen::Vector3f& pos, Eigen::Vector3f ray, Eigen::Vector3f start){
    return m_shape.getAnchorPos(lastSelected, pos, ray, start);
}
