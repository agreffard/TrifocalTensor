#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <Eigen/SVD>
#include <Tensor.hpp>

#include "MathIO.hpp"
#include "draw.hpp"

using namespace std;
using namespace Eigen;


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
VectorXf transfert(VectorXf x1, VectorXf x2, Tensor tensor) {

  MatrixXf MatB = MatrixXf::Zero(4, 2);
  VectorXf Vecb = VectorXf::Zero(4); // second member of the equation

  // filling B & Vecb
      for (int i=0; i<2; ++i) {
        for (int l=0; l<2; ++l) {
          for (int k=0; k<3; ++k) {
            MatB(2*i + l, l) += x1(k) * ( x2(2) * tensor(i, 2, k) - x2(i) * tensor(2, 2, k) );

            // - because Vecb goes on the other side of the equation
            Vecb(2*i + l) -= x1(k) * ( x2(i) * tensor(2, l, k) - x2(2) * tensor(i, l, k) );
          }
        }
      }

  JacobiSVD<MatrixXf> svdB(MatB, ComputeThinU | ComputeThinV);
  VectorXf solution2d = svdB.solve(Vecb);

  VectorXf homogeneSolution(3);
  homogeneSolution << solution2d(0), solution2d(1), 1;

  return homogeneSolution;
}


int main(int argc, char *argv[])
{
  // init SDL image
  if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == -1){
    std::cerr << "error IMG_Init" << std::endl;
    return EXIT_FAILURE;
  }

  // load some images
  SDL_Surface *image1 = IMG_Load("input/image1.jpg");
  SDL_Surface *image2 = IMG_Load("input/image2.jpg");
  SDL_Surface *image3 = IMG_Load("input/image3.jpg");
  if(image1 == 0 || image2 == 0 || image3 == 0){
    std::cerr << "error loading images" << std::endl;
    return 0;
  }

  // init screen surface
  if(SDL_Init(SDL_INIT_VIDEO) == -1){
    std::cerr << "error SDL_Init" << std::endl;
    return EXIT_FAILURE;
  }

  // create a screen surface that will include the 3 images
  SDL_Surface *screen = SDL_SetVideoMode(image1->w + image2->w + image3->w, image1->h, 32, SDL_HWSURFACE);
  SDL_WM_SetCaption("Trifocal Tensor", NULL);

  // bibd the images on the surface  
  SDL_Rect imageOffset;
  imageOffset.x = 0;
  imageOffset.y = 0;
  SDL_BlitSurface(image1, NULL, screen, &imageOffset);
  imageOffset.x = image1->w;
  SDL_BlitSurface(image2, NULL, screen, &imageOffset);
  imageOffset.x = image1->w + image2->w;
  SDL_BlitSurface(image3, NULL, screen, &imageOffset);

  // load the point lists
  MatrixXf list1;
  MatrixXf list2;
  MatrixXf list3;
  kn::loadMatrix(list1,"input/list1.list");
  kn::loadMatrix(list2,"input/list2.list");
  kn::loadMatrix(list3,"input/list3.list");





 /********************************************************************

  First step : find the tensor from the three lists

  *********************************************************************/
Tensor tensor;
fillTensor(tensor, list1, list2, list3);
tensor.print();

/********************************************************************

  Second step : transfert

*********************************************************************/

VectorXf solution = transfert(list1.row(0), list2.row(0), tensor);
cout << endl << "solution : " << endl << solution << endl;




/*
  // save a list
  kn::saveMatrix(list1,"/tmp/myList.mat");

  // some colors
  Uint32 red  = 0xffff0000;
  Uint32 blue = 0xff0000ff;
  Uint32 yellow = 0xffffff00;

  // draw points on image1
  for(int i=0; i<list1.rows(); ++i)
    fill_circle(screen, list1(i,0), list1(i,1), 3, red);

  // draw points on image2
  for(int i=0; i<list2.rows(); ++i)
    fill_circle(screen, list2(i,0)+image1->w, list2(i,1), 3, blue);

  // draw points on image3
  for(int i=0; i<list3.rows(); ++i)
    fill_circle(screen, list3(i,0)+image1->w+image2->w, list3(i,1), 3, yellow);

  // display everything
  SDL_Flip(screen);
  pause();


*/

  // quit sdl
  SDL_FreeSurface(image1); 
  SDL_FreeSurface(image2); 
  SDL_FreeSurface(image3); 
  IMG_Quit();
  SDL_Quit();

  return EXIT_SUCCESS;
}



