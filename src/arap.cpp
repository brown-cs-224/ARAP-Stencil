#include "arap.h"
#include "graphics/meshloader.h"

#include <iostream>
#include <set>
#include <map>
#include <vector>

using namespace std;
using namespace Eigen;

ARAP::ARAP() {}

void ARAP::init(Eigen::Vector3f &coeffMin, Eigen::Vector3f &coeffMax)
{
    vector<Vector3f> vertices;
    vector<Vector3i> triangles;

    // If this doesn't work for you, remember to change your working directory
    if (MeshLoader::loadTriMesh("meshes/cactus.obj", vertices, triangles)) {
        m_shape.init(vertices, triangles);
    }

    // Students, please don't touch this code: get min and max for viewport stuff
    MatrixX3f all_vertices = MatrixX3f(vertices.size(), 3);
    int i = 0;
    for (unsigned long i = 0; i < vertices.size(); ++i) {
        all_vertices.row(i) = vertices[i];
    }
    coeffMin = all_vertices.colwise().minCoeff();
    coeffMax = all_vertices.colwise().maxCoeff();
}

// Move an anchored vertex, defined by its index, to targetPosition
void ARAP::move(int vertex, Vector3f targetPosition)
{
    std::vector<Eigen::Vector3f> new_vertices = m_shape.getVertices();
    const std::unordered_set<int>& anchors = m_shape.getAnchors();

    // TODO: implement ARAP here
    new_vertices[vertex] = targetPosition;

    // Here are some helpful controls for the application
    //
    // - You start in first-person camera mode
    //   - WASD to move, left-click and drag to rotate
    //   - R and F to move vertically up and down
    //
    // - C to change to orbit camera mode
    //
    // - Right-click (and, optionally, drag) to anchor/unanchor points
    //   - Left-click an anchored point to move it around
    //
    // - Minus and equal keys (click repeatedly) to change the size of the vertices

    m_shape.setVertices(new_vertices);
}
