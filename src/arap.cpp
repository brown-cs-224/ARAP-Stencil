#include "arap.h"

#include <iostream>
#include <set>
#include <map>
#include "graphics/MeshLoader.h"
#include <vector>
#include <filesystem>
using namespace Eigen;

ARAP::ARAP(){}

void ARAP::init(Eigen::Vector3f &min, Eigen::Vector3f &max)
{
    std::vector<Vector3f> vertices;
    std::vector<Vector3f> normals;
    std::vector<Vector3i> tets;
    if(MeshLoader::loadTriMesh("/Users/blinnbryce/Documents/GitHub/arap/meshes/armadillo.obj", vertices, normals,tets)) {
        m_shape.init(vertices, tets);
    } else {
        exit(1);
    }
    std::vector<Vector3d> double_verts;

    MatrixX3f all_vertices = MatrixX3f(vertices.size(),3);
    int i = 0;
    for(auto particle: vertices) {
        double_verts.push_back(Vector3d((double)particle[0], (double)particle[1], (double)particle[2]));
        all_vertices.row(i) = particle;
        i++;
    }
    min = all_vertices.colwise().minCoeff();
    max = all_vertices.colwise().maxCoeff();

    m_weights = SparseMatrix<float>(vertices.size(),vertices.size());
    setWeights();
}

void ARAP::setWeights() {
    std::vector<std::unordered_set<int>> rings = m_shape.getRings();
    std::vector<std::vector<Cell>> cells = m_shape.getCells();
    std::vector<Vector3i> triangles = m_shape.getTriangles();
    std::vector<Vector3f> vertices = m_shape.getVertices();

    std::vector<Triplet<float>> triplets;
    std::vector<std::unordered_set<int>> visited(vertices.size());
    for (int tri_i = 0; tri_i < triangles.size(); tri_i++) {
        Vector3i face_i = triangles[tri_i];
        for (Cell cell: cells[tri_i]) {
            std::pair<int,int> edge = cell.first;
            if (visited[edge.first].find(edge.second) != visited[edge.first].end()) continue;
            Vector3i face_j = cell.second;

            int a = oppositeVertex(face_i,edge);
            int b = oppositeVertex(face_j,edge);

            float cos_alpha = innerCos(vertices[edge.first],vertices[a],vertices[edge.second]);
            float cos_beta = innerCos(vertices[edge.first],vertices[b],vertices[edge.second]);

            float weight_alpha = abs(cos_alpha) / sqrt(1 - cos_alpha * cos_alpha);
            float weight_beta = abs(cos_beta) / sqrt(1 - cos_beta * cos_beta);
            float weight = (weight_alpha + weight_beta) / 2.;
            if (abs(weight) < 1E-6) continue;

            triplets.push_back(Triplet<float>(edge.first,edge.second,weight));
            triplets.push_back(Triplet<float>(edge.second,edge.first,weight));
            visited[edge.first].insert(edge.second);
        }
    }
    m_weights.setFromTriplets(triplets.begin(), triplets.end());
}

SparseMatrix<float> ARAP::getIdReduction(std::unordered_set<int> anchors) {
    int nVertices = m_shape.getVertices().size();
    std::vector<Triplet<float>> triplets; int idx = 0;
    for (int i = 0; i < nVertices; i++) {
        if (anchors.find(i) != anchors.end()) continue;
        triplets.push_back(Triplet<float>(idx,i,1.f));
        idx++;
    }
    SparseMatrix<float> idReduction(nVertices - anchors.size(), nVertices);
    idReduction.setFromTriplets(triplets.begin(),triplets.end());
    return idReduction;
}

SparseMatrix<float> ARAP::getConstrainedL(std::unordered_set<int> anchors, SparseMatrix<float> L) {
    SparseMatrix<float> idReduction = getIdReduction(anchors);
    return idReduction * L * idReduction.transpose();
}

MatrixX3f ARAP::getCMatrix(std::vector<Vector3f> v_prime, std::unordered_set<int> anchors, SparseMatrix<float> L) {
    int nVertices = m_shape.getVertices().size();
    MatrixX3f P_anchor = MatrixX3f::Zero(nVertices,3);
    for (int i = 0; i < nVertices; i++) {
        if (anchors.find(i) == anchors.end()) continue;
        P_anchor.row(i) = v_prime[i];
    }
    SparseMatrix<float> idReduction = getIdReduction(anchors);
    return idReduction * L * P_anchor;
}

std::vector<Matrix3f> ARAP::getRotations(std::vector<Vector3f> v_prime) {
    std::vector<Vector3f> v = m_shape.getVertices();
    std::vector<std::unordered_set<int>> rings = m_shape.getRings();

    std::vector<Matrix3f> rotations;
    for (int i = 0; i < rings.size(); i++) {
        std::unordered_set<int> ring = rings[i];
        std::vector<Triplet<float>> tripletsP, tripletsP_prime, tripletsDiagonal;

        for (int j : ring) {
            Vector3f e_ij = v[i] - v[j];
            Vector3f e_ij_prime = v_prime[i] - v_prime[j];

            for (int idx = 0; idx < 3; idx++) {
                tripletsP.push_back(Triplet<float>(idx,j,e_ij[idx]));
                tripletsP_prime.push_back(Triplet<float>(j,idx,e_ij_prime[idx]));
            }
            tripletsDiagonal.push_back(Triplet<float>(j,j,m_weights.coeff(i,j)));
        }

        SparseMatrix<float> P(3,v.size());
        P.setFromTriplets(tripletsP.begin(),tripletsP.end());
        SparseMatrix<float> P_prime(v.size(),3);
        P_prime.setFromTriplets(tripletsP_prime.begin(),tripletsP_prime.end());
        SparseMatrix<float> D(v.size(),v.size());
        D.setFromTriplets(tripletsDiagonal.begin(),tripletsDiagonal.end());

        Matrix3f S = P * D * P_prime;

        JacobiSVD<Matrix3f> svd(S, ComputeFullU | ComputeFullV);
        Matrix3f R = svd.matrixV() * svd.matrixU().transpose();
        R = (R.array().abs() < 1e-5).select(0.,R);
        R /= R.determinant();

        rotations.push_back(R);
    }
    return rotations;
}

MatrixX3f ARAP::getBMatrix(std::vector<Matrix3f> rotations, std::unordered_set<int> anchors) {
    std::vector<Vector3f> v = m_shape.getVertices();
    std::vector<std::unordered_set<int>> rings = m_shape.getRings();

    MatrixX3f B(v.size() - anchors.size(),3);
    int idx = 0;
    for (int i = 0; i < v.size(); i++) {
        if (anchors.find(i) != anchors.end()) continue;
        std::unordered_set<int> ring = rings[i];
        std::vector<Triplet<float>> triplets;

        for (int j : ring) {
            Vector3f r_e_ij = (rotations[i] + rotations[j]) / 2. * (v[i] - v[j]);

            for (int k = 0; k < 3; k++) {
                if (abs(r_e_ij[k]) < 1E-6) continue;
                triplets.push_back(Triplet<float>(j,k,r_e_ij[k]));
            }
        }

        SparseMatrix<float> RP(v.size(),3);
        RP.setFromTriplets(triplets.begin(),triplets.end());

        B.row(idx) = m_weights.row(i) * RP;
        idx++;
    }
    return B;
}

void ARAP::move(int vertex, Vector3f pos)
{
    std::vector<Vector3f> new_vertices = m_shape.getVertices();
    const std::unordered_set<int>& anchors = m_shape.getAnchors();

    //TODO: implement ARAP here
    new_vertices[vertex] = pos;
    SparseMatrix<float> L(new_vertices.size(),new_vertices.size());
    std::vector<Triplet<float>> triplets;
    VectorXf diagonal = m_weights * VectorXf::Ones(L.cols());
    for (int i = 0; i < L.rows(); i++) triplets.push_back(Triplet<float>(i,i,diagonal[i]));
    L.setFromTriplets(triplets.begin(),triplets.end());
    L -= m_weights;

    MatrixX3f X(new_vertices.size()-anchors.size(),3), B(new_vertices.size()-anchors.size(),3), C(new_vertices.size()-anchors.size(),3);
    C = getCMatrix(new_vertices,anchors,L);

    SparseMatrix<float> uL = getConstrainedL(anchors,L);
    SimplicialLDLT<SparseMatrix<float>> solver(uL);

    // Iteration
    float prev = INFINITY;
    float curr = 0.;

    for (int its = 0; its < MAXITERS; its++) {
        std::vector<Matrix3f> rotations = getRotations(new_vertices);
        B = getBMatrix(rotations,anchors) - C;
        X = solver.solve(B);
        X = (X.array().abs() < 1e-5).select(0.,X);

        int idx = 0;
        for (int i = 0; i < new_vertices.size(); i++) {
            if (anchors.find(i) != anchors.end()) continue;
            curr += (new_vertices[i] - X.row(idx).transpose()).norm();
            new_vertices[i]= X.row(idx);
            idx++;
        }

        if (((prev - curr) < ATOL) || (curr < BTOL)) break;
        prev = curr;
        curr = 0;
    }

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
