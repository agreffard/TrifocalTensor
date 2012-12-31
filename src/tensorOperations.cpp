#include <Eigen/SVD>
#include <Tensor.hpp>

// Fonction finding the tensor from the 3 lists of points
void fillTensor(Tensor& tensor, MatrixXf& list1, MatrixXf& list2, MatrixXf& list3) {

  assert(list1.rows()==list2.rows());
  assert(list1.rows()==list3.rows());

  int nbPoints = list1.rows();
  //cout << "nbPoints " << nbPoints << endl;

  MatrixXf A = MatrixXf::Zero(nbPoints * 4, 27);

  // filling A
  for (int p=0; p<nbPoints; ++p) {
    for (int i=0; i<2; ++i) {
      for (int l=0; l<2; ++l) {
        for (int k=0; k<3; ++k) {
          A(4*p + 2*i + l, 3*3*2 + 3*l + k) += list1(p, k) * list2(p, i) * list3(p, 2);
          A(4*p + 2*i + l, 3*3*i + 3*l + k) -= list1(p, k) * list2(p, 2) * list3(p, 2);
          A(4*p + 2*i + l, 3*3*2 + 3*2 + k) -= list1(p, k) * list2(p, i) * list3(p, l);
          A(4*p + 2*i + l, 3*3*i + 3*2 + k) += list1(p, k) * list2(p, 2) * list3(p, l);
        }
      }
    }
  }

  // Decomposition SVD of the Matrix A
  JacobiSVD<MatrixXf> svdA(A, ComputeThinU | ComputeThinV);

  // we have our tensor
  tensor.setCoord( svdA.matrixV().col(26) );

}


// Fonction doing the transfert and finding the correspounding point from the 2 others
VectorXf transfert(VectorXf x1, VectorXf x2, Tensor tensor, int unknownImage) {

  MatrixXf MatB = MatrixXf::Zero(4, 2);
  VectorXf Vecb = VectorXf::Zero(4); // second member of the equation
  float factor;

  // filling B & Vecb
      for (int i=0; i<2; ++i) {
        for (int l=0; l<2; ++l) {
          for (int k=0; k<3; ++k) {
            switch (unknownImage) {
              case 1:
                factor = x1(i) * x2(2) * tensor(2, l, k) - x1(2) * x2(2) * tensor(i, l, k) - x1(i) * x2(l) * tensor(2, 2, k) + x1(2) * x2(l) * tensor(i, 2, k);
                if (k==2) {
                  // Soustraction because Vecb goes on the other side of the equation
                  Vecb(2*i + l) -= factor;
                }
                else {
                  MatB(2*i + l, k) += factor;
                }
                break;
              case 2:
                MatB(2*i + l, i) += x1(k) * ( x2(2) * tensor(2, l, k) - x2(i) * tensor(2, 2, k) );
                Vecb(2*i + l) -= x1(k) * ( x2(l) * tensor(i, 2, k) - x2(2) * tensor(i, l, k) );
                break;
              case 3:
                MatB(2*i + l, l) += x1(k) * ( x2(2) * tensor(i, 2, k) - x2(i) * tensor(2, 2, k) );
                Vecb(2*i + l) -= x1(k) * ( x2(i) * tensor(2, l, k) - x2(2) * tensor(i, l, k) );
                break;
            }

          }
        }
      }

  JacobiSVD<MatrixXf> svdB(MatB, ComputeThinU | ComputeThinV);
  VectorXf solution2d = svdB.solve(Vecb);

  VectorXf homogeneSolution(3);
  homogeneSolution << solution2d(0), solution2d(1), 1;

  return homogeneSolution;
}