#include "graphics/meshloader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "util/tiny_obj_loader.h"

#include <iostream>

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QFileInfo>
#include <iostream>
#include <set>

using namespace Eigen;

bool MeshLoader::loadTriMesh(const std::string &filepath, std::vector<Eigen::Vector3f> &vertices,  std::vector<Eigen::Vector3f> &normals, std::vector<Eigen::Vector3i> &faces)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    QFileInfo info(QString((filepath).c_str()));
    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err,
                                info.absoluteFilePath().toStdString().c_str(), (info.absolutePath().toStdString() + "/").c_str(), true);

    if(!ret || !err.empty() || attrib.vertices.size() == 0) {
        std::cerr << "Failed to load/parse .obj file" << std::endl;
        std::cerr << err << std::endl;
        return false;
    }

    const int numVerts = attrib.vertices.size() / 3;
    vertices.resize(numVerts);
    normals.resize(numVerts);

    for (unsigned int i = 0; i < shapes[0].mesh.indices.size(); i+=3) {
        faces.push_back(Vector3i(shapes[0].mesh.indices[i].vertex_index, shapes[0].mesh.indices[i+1].vertex_index, shapes[0].mesh.indices[i+2].vertex_index));
    }

    std::set<int> verts;
    for (unsigned int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        const int vind = 3*shapes[0].mesh.indices[i].vertex_index;
        if (verts.find(vind) == verts.end()) {
            vertices[shapes[0].mesh.indices[i].vertex_index] = Vector3f(attrib.vertices[vind], attrib.vertices[vind+1], attrib.vertices[vind+2]);
            verts.emplace(vind);
        }
    }

    return true;
}

MeshLoader::MeshLoader()
{

}
