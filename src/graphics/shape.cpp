#include "shape.h"

#include <iostream>
#include "graphics/Shader.h"

using namespace Eigen;
using namespace std;

Shape::Shape()
    :
      m_numSurfaceVertices(),
      m_verticesSize(),
      m_modelMatrix(Eigen::Matrix4f::Identity()),
      m_wireframe(false)
{
}


Eigen::Vector3f Shape::getNormal(const Eigen::Vector3i& face){
    Vector3f& v1 = m_vertices[face[0]];
    Vector3f& v2 = m_vertices[face[1]];
    Vector3f& v3 = m_vertices[face[2]];
    Vector3f e1 = v2 - v1;
    Vector3f e2 = v3 - v1;
    Vector3f n = e1.cross(e2);
    return  n/n.norm();
}

void Shape::updateMesh(const std::vector<Eigen::Vector3i> &triangles,
                       const std::vector<Eigen::Vector3f> &vertices,
                       std::vector<Eigen::Vector3f>& verts,
                       std::vector<Eigen::Vector3f>& normals,
                       std::vector<Eigen::Vector3f>& colors){
    verts.reserve(triangles.size() * 3);
    normals.reserve(triangles.size() * 3);
    for(const Eigen::Vector3i& f : triangles) {
        Vector3f n = getNormal(f);

        for (auto& v: {f[0], f[1], f[2]}) {
            normals.push_back(n);
            verts.push_back(vertices[v]);

            if (m_anchors.find(v) == m_anchors.end()){
                colors.push_back(Eigen::Vector3f(1.f, 0.f, 0.f));
            } else {
                colors.push_back(Eigen::Vector3f(0.f, 0.f, 1.f));
            }
        }
    }
}



void Shape::init(const std::vector<Eigen::Vector3f> &vertices, const std::vector<Eigen::Vector3i> &triangles)
{
    m_vertices.clear();
    copy(vertices.begin(), vertices.end(), back_inserter(m_vertices));

    std::vector<Eigen::Vector3f> verts;
    std::vector<Eigen::Vector3f> normals;
    std::vector<Eigen::Vector3f> colors;
    std::vector<Eigen::Vector3i> faces;
    faces.reserve(triangles.size());

    for(int s = 0; s < triangles.size() * 3; s+=3) faces.push_back(Eigen::Vector3i(s, s + 1, s + 2));
    updateMesh(triangles, vertices, verts, normals, colors);

    glGenBuffers(1, &m_surfaceVbo);
    glGenBuffers(1, &m_surfaceIbo);
    glGenVertexArrays(1, &m_surfaceVao);

    glBindBuffer(GL_ARRAY_BUFFER, m_surfaceVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ((verts.size() * 3) + (normals.size() * 3) + (colors.size() * 3)), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * verts.size() * 3, static_cast<const void *>(verts.data()));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * verts.size() * 3, sizeof(float) * normals.size() * 3, static_cast<const void *>(normals.data()));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * ((verts.size() * 3) + (normals.size() * 3)), sizeof(float) * colors.size() * 3, static_cast<const void *>(colors.data()));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_surfaceIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 3 * faces.size(), static_cast<const void *>(faces.data()), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(m_surfaceVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_surfaceVbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, static_cast<GLvoid *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid *>(sizeof(float) * verts.size() * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid *>(sizeof(float) * (verts.size() * 3 + colors.size() * 3)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_surfaceIbo);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_numSurfaceVertices = faces.size() * 3;
    m_verticesSize = vertices.size();
    m_faces = triangles;
    m_red = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    m_blue = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    m_green = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    m_alpha = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}


void Shape::setVertices(const std::vector<Eigen::Vector3f> &vertices)
{
    m_vertices.clear();
    copy(vertices.begin(), vertices.end(), back_inserter(m_vertices));

    std::vector<Eigen::Vector3f> verts;
    std::vector<Eigen::Vector3f> normals;
    std::vector<Eigen::Vector3f> colors;

    updateMesh(m_faces, vertices, verts, normals, colors);

    glBindBuffer(GL_ARRAY_BUFFER, m_surfaceVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ((verts.size() * 3) + (normals.size() * 3) + (colors.size() * 3)), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * verts.size() * 3, static_cast<const void *>(verts.data()));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * verts.size() * 3, sizeof(float) * normals.size() * 3, static_cast<const void *>(normals.data()));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * ((verts.size() * 3) + (normals.size() * 3)), sizeof(float) * colors.size() * 3, static_cast<const void *>(colors.data()));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool Shape::getAnchorPos(int lastSelected, Eigen::Vector3f& pos,
                                    Eigen::Vector3f ray, Eigen::Vector3f start){
    bool isAnchor = m_anchors.find(lastSelected) != m_anchors.end();
    if(isAnchor) {
        Eigen::Vector3f oldPos = m_vertices[lastSelected];
        Eigen::ParametrizedLine line = ParametrizedLine<float, 3>::Through(start, start+ray);
        pos = line.projection(oldPos);
    }
    return isAnchor;
}

std::vector<Eigen::Vector3f> Shape::getVertices(){
    return m_vertices;
}

const std::unordered_set<int>& Shape::getAnchors(){
    return m_anchors;
};


int Shape::getClosestVertex(Eigen::Vector3f start, Eigen::Vector3f ray, float threshold){
    int closest_vertex = -1;
    int i = 0;
    float dist = std::numeric_limits<float>::max();
    Eigen::ParametrizedLine line = ParametrizedLine<float, 3>::Through(start, start + ray);
    for(auto& v : m_vertices) {
        float d = line.distance(v);
        if(d<dist) {
            dist = d;
            closest_vertex = i;
        }
        i++;
    }

    if(dist >= threshold) {
        closest_vertex = -1;
    }

    return closest_vertex;
}

void Shape::select(Shader *shader, int closest_vertex) {

    if(m_anchors.find(closest_vertex) != m_anchors.end()) {
        m_anchors.erase(closest_vertex);
    } else {
        m_anchors.insert(closest_vertex);
    }

    std::vector<Eigen::Vector3f> verts;
    std::vector<Eigen::Vector3f> normals;
    std::vector<Eigen::Vector3f> colors;
    updateMesh(m_faces, m_vertices, verts, normals, colors);

    glBindBuffer(GL_ARRAY_BUFFER, m_surfaceVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ((verts.size() * 3) + (normals.size() * 3) + (colors.size() * 3)), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * verts.size() * 3, static_cast<const void *>(verts.data()));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * verts.size() * 3, sizeof(float) * normals.size() * 3, static_cast<const void *>(normals.data()));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * ((verts.size() * 3) + (normals.size() * 3)), sizeof(float) * colors.size() * 3, static_cast<const void *>(colors.data()));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}


void Shape::setModelMatrix(const Eigen::Affine3f &model)
{
    m_modelMatrix = model.matrix();
}

void Shape::toggleWireframe()
{
    m_wireframe = !m_wireframe;
}

void Shape::draw(Shader *shader, GLenum mode)
{
    switch(mode){
    case GL_TRIANGLES:
        shader->setUniform("wire", 0);
        shader->setUniform("m", m_modelMatrix);
        shader->setUniform("red", m_red);
        shader->setUniform("green", m_green);
        shader->setUniform("blue", m_blue);
        shader->setUniform("alpha", m_alpha);
        glBindVertexArray(m_surfaceVao);
        glDrawElements(mode, m_numSurfaceVertices, GL_UNSIGNED_INT, reinterpret_cast<GLvoid *>(0));
        glBindVertexArray(0);
        break;
    case GL_POINTS:
        shader->setUniform("m", m_modelMatrix);
        glBindVertexArray(m_surfaceVao);
        glDrawElements(mode, m_numSurfaceVertices, GL_UNSIGNED_INT, reinterpret_cast<GLvoid *>(0));
        glBindVertexArray(0);
        break;
    }

}
