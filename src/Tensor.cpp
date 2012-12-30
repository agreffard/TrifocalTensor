#include <Tensor.hpp>
#include <Eigen/Core>
#include <iostream>

using namespace Eigen;

Tensor::Tensor(){
    m_tensorCoord = VectorXf::Zero(27);
}

Tensor::~Tensor(){}

float Tensor::operator()(size_t i, size_t j, size_t k) const {
    return m_tensorCoord[9*i + 3*j + k];
}

VectorXf Tensor::getCoord() const {
    return m_tensorCoord;
}

void Tensor::setCoord(VectorXf coord) {
    m_tensorCoord = coord;
}

void Tensor::print() const {
    std::cout << "tensor : " << std::endl << m_tensorCoord << std::endl;
}