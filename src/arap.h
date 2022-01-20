#ifndef ARAP_H
#define ARAP_H

#define ATOL 1e-4
#define BTOL 1e-2
#define MAXITERS 100

#include "graphics/shape.h"
#include <Eigen/StdList>
#include <Eigen/StdVector>
#include <Eigen/Sparse>
#include <Eigen/SparseCholesky>
#include <iostream>
class Shader;

class ARAP
{
public:
    ARAP();

    inline int oppositeVertex(Eigen::Vector3i face, std::pair<int,int> edge) {
        for (int j = 0; j < 3; j++) if (!(edge.first == face[j] || edge.second == face[j])) return face[j];
        return -1;
    }
    inline float innerCos(Eigen::Vector3f e0, Eigen::Vector3f opp, Eigen::Vector3f e1) {
        Eigen::Vector3f v0 = e0 - opp;
        v0.normalize();
        Eigen::Vector3f v1 = e1 - opp;
        v1.normalize();
        return v0.dot(v1);
    }

    void init(Eigen::Vector3f &min, Eigen::Vector3f &max);
    void draw(Shader *shader, GLenum mode);
    void select(Shader *shader, int vertex);
    void toggleWire();
    void move(int vertex, Eigen::Vector3f pos);
    int getClosestVertex(Eigen::Vector3f ray, Eigen::Vector3f start);
    bool getAnchorPos(int lastSelected, Eigen::Vector3f& pos,
                                 Eigen::Vector3f ray, Eigen::Vector3f start);

   void setWeights();

   Eigen::SparseMatrix<float> getIdReduction(std::unordered_set<int> anchors);
   Eigen::SparseMatrix<float> getUnconstrainedWeights(std::unordered_set<int> anchors);
   Eigen::MatrixX3f getCMatrix(std::vector<Eigen::Vector3f> v_prime, std::unordered_set<int> anchors);
   std::vector<Eigen::Matrix3f> getRotations(std::vector<Eigen::Vector3f> v_prime);
   Eigen::MatrixX3f getBMatrix(std::vector<Eigen::Matrix3f> rotations, std::unordered_set<int> anchors);
private:
    Shape m_shape;
    Eigen::SparseMatrix<float> m_weights;
};

#endif // ARAP_H
