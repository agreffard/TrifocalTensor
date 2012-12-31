#ifndef TENSOR_OPERATIONS_HPP
#define TENSOR_OPERATIONS_HPP

#include <Tensor.hpp>

void fillTensor(Tensor& tensor, MatrixXf& list1, MatrixXf& list2, MatrixXf& list3);
VectorXf transfert(VectorXf x1, VectorXf x2, Tensor tensor, int unKnownImage);

#endif