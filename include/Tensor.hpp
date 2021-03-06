#ifndef TENSOR_HPP
#define TENSOR_HPP

#include <cstring>
#include <Eigen/Core>

using namespace Eigen;

class Tensor {

private:
    VectorXf m_tensorCoord;

public:
    Tensor();
    ~Tensor();
    float operator()(size_t i, size_t j, size_t k) const;

    VectorXf getCoord() const;
    void setCoord(VectorXf coord);

    void print() const;
};

#endif