#include "shape.h"

#include <iostream>
#include "graphics/Shader.h"

using namespace Eigen;

Shape::Shape()
    :
      m_numSurfaceVertices(),
      m_verticesSize(),
      m_modelMatrix(Eigen::Matrix4f::Identity()),
      m_wireframe(false)
{
}

void Shape::init(const std::vector<Eigen::Vector3f> &vertices, const std::vector<Eigen::Vector3i> &triangles)
{
    m_vertices.clear();
    copy(vertices.begin(), vertices.end(), back_inserter(m_vertices));

    std::vector<Eigen::Vector3f> verts;
    std::vector<Eigen::Vector3f> normals;
    std::vector<Eigen::Vector3f> colors;
    std::vector<Eigen::Vector3i> faces;
    verts.reserve(triangles.size() * 3);
    normals.reserve(triangles.size() * 3);
    for(auto& f : triangles) {
        auto& v1 = vertices[f[0]];
        auto& v2 = vertices[f[1]];
        auto& v3 = vertices[f[2]];
        auto& e1 = v2 - v1;
        auto& e2 = v3 - v1;
        auto n = e1.cross(e2);
        int s = verts.size();
        faces.push_back(Eigen::Vector3i(s, s + 1, s + 2));
        normals.push_back(n);
        normals.push_back(n);
        normals.push_back(n);
        verts.push_back(v1);
        verts.push_back(v2);
        verts.push_back(v3);
        colors.push_back(Eigen::Vector3f(1.f, 0.f, 0.f));
        colors.push_back(Eigen::Vector3f(1.f, 0.f, 0.f));
        colors.push_back(Eigen::Vector3f(1.f, 0.f, 0.f));
    }
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

    verts.reserve(m_faces.size() * 3);
    normals.reserve(m_faces.size() * 3);
    for(auto& f : m_faces) {
        Vector3f v1 = Vector3f(vertices[f[0]].x(), vertices[f[0]].y(), vertices[f[0]].z());
        Vector3f v2 = Vector3f(vertices[f[1]].x(), vertices[f[1]].y(), vertices[f[1]].z());
        Vector3f v3 = Vector3f(vertices[f[2]].x(), vertices[f[2]].y(), vertices[f[2]].z());
        auto& e1 = v2 - v1;
        auto& e2 = v3 - v1;
        auto n = e1.cross(e2);
        normals.push_back(n);
        normals.push_back(n);
        normals.push_back(n);
        verts.push_back(v1);
        verts.push_back(v2);
        verts.push_back(v3);
        if(std::find(anchors.begin(), anchors.end(), f[0]) != anchors.end()) {
            colors.push_back(Eigen::Vector3f(0.f, 0.f, 1.f));
        } else {
            colors.push_back(Eigen::Vector3f(1.f, 0.f, 0.f));
        }
        if(std::find(anchors.begin(), anchors.end(), f[1]) != anchors.end()) {
            colors.push_back(Eigen::Vector3f(0.f, 0.f, 1.f));
        } else {
            colors.push_back(Eigen::Vector3f(1.f, 0.f, 0.f));
        }
        if(std::find(anchors.begin(), anchors.end(), f[2]) != anchors.end()) {
            colors.push_back(Eigen::Vector3f(0.f, 0.f, 1.f));
        } else {
            colors.push_back(Eigen::Vector3f(1.f, 0.f, 0.f));
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_surfaceVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ((verts.size() * 3) + (normals.size() * 3) + (colors.size() * 3)), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * verts.size() * 3, static_cast<const void *>(verts.data()));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * verts.size() * 3, sizeof(float) * normals.size() * 3, static_cast<const void *>(normals.data()));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * ((verts.size() * 3) + (normals.size() * 3)), sizeof(float) * colors.size() * 3, static_cast<const void *>(colors.data()));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool Shape::move(Eigen::Vector3f ray, Eigen::Vector3f start) {
    if(lastSelected > -1) {
        Eigen::Vector3f oldPos = m_vertices[lastSelected];
        Eigen::ParametrizedLine line = ParametrizedLine<float, 3>::Through(start, start+ray);

        // p
        std::vector<Eigen::Vector3f> new_vertices;// = arap.move(m_verticies, line.projection(oldPos));

        // comment for ARAP
        new_vertices.clear();
        copy(m_vertices.begin(), m_vertices.end(), back_inserter(new_vertices));
        new_vertices[lastSelected] = line.projection(oldPos);

        // p'
        std::vector<Eigen::Vector3f> old_vertices;
        old_vertices.clear();
        copy(m_vertices.begin(), m_vertices.end(), back_inserter(old_vertices));

        setVertices(new_vertices);
        return true;
    }
    return false;
}



void Shape::select(Shader *shader, Eigen::Vector3f start, Eigen::Vector3f ray, bool isAnchor) {
    int closest_vertex = -1;
    int i=0;
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

    if(dist >= 0.03) {
        closest_vertex = -1;
    }
    auto pos = std::find(anchors.begin(), anchors.end(), closest_vertex);
    int index = pos - anchors.begin();
    if(isAnchor) {
        if(pos != anchors.end()) {
            anchors.erase(pos);
        } else if (closest_vertex != -1){
            anchors.push_back(closest_vertex);
        }
        std::vector<Eigen::Vector3f> verts;
        std::vector<Eigen::Vector3f> normals;
        std::vector<Eigen::Vector3f> colors;
        verts.reserve(m_faces.size() * 3);
        normals.reserve(m_faces.size() * 3);
        for(auto& f : m_faces) {
            Vector3f v1 = Vector3f(m_vertices[f[0]].x(), m_vertices[f[0]].y(), m_vertices[f[0]].z());
            Vector3f v2 = Vector3f(m_vertices[f[1]].x(), m_vertices[f[1]].y(), m_vertices[f[1]].z());
            Vector3f v3 = Vector3f(m_vertices[f[2]].x(), m_vertices[f[2]].y(), m_vertices[f[2]].z());
            auto& e1 = v2 - v1;
            auto& e2 = v3 - v1;
            auto n = e1.cross(e2);
            normals.push_back(n);
            normals.push_back(n);
            normals.push_back(n);
            verts.push_back(v1);
            verts.push_back(v2);
            verts.push_back(v3);
            if(std::find(anchors.begin(), anchors.end(), f[0]) != anchors.end()) {
                colors.push_back(Eigen::Vector3f(0.f, 0.f, 1.f));
            } else {
                colors.push_back(Eigen::Vector3f(1.f, 0.f, 0.f));
            }
            if(std::find(anchors.begin(), anchors.end(), f[1]) != anchors.end()) {
                colors.push_back(Eigen::Vector3f(0.f, 0.f, 1.f));
            } else {
                colors.push_back(Eigen::Vector3f(1.f, 0.f, 0.f));
            }
            if(std::find(anchors.begin(), anchors.end(), f[2]) != anchors.end()) {
                colors.push_back(Eigen::Vector3f(0.f, 0.f, 1.f));
            } else {
                colors.push_back(Eigen::Vector3f(1.f, 0.f, 0.f));
            }
        }
        glBindBuffer(GL_ARRAY_BUFFER, m_surfaceVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ((verts.size() * 3) + (normals.size() * 3) + (colors.size() * 3)), nullptr, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * verts.size() * 3, static_cast<const void *>(verts.data()));
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * verts.size() * 3, sizeof(float) * normals.size() * 3, static_cast<const void *>(normals.data()));
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * ((verts.size() * 3) + (normals.size() * 3)), sizeof(float) * colors.size() * 3, static_cast<const void *>(colors.data()));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    if(lastSelected == closest_vertex) {
         lastSelected = -1;
     } else {
        lastSelected = closest_vertex;
     }
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
