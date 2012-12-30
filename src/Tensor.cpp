#include <Tensor.hpp>
#include <Eigen/Core>
#include <iostream>

using namespace Eigen;

Tensor::Tensor(){
    m_tensorCoord = VectorXf::Zero(27);
}

Tensor::~Tensor(){}

float Tensor::operator()(size_t i, size_t j, size_t k) {
    return m_tensorCoord[9*i + 3*j + k];
}

VectorXf Tensor::getCoord() const {
    return m_tensorCoord;
}

void Tensor::setCoord(VectorXf coord) {
    m_tensorCoord = coord;
}

void print(const Tensor& t) {
    std::cout << t.getCoord() << std::endl;
}