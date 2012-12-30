#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <Eigen/SVD>
#include <Tensor.hpp>

#include "MathIO.hpp"
#include "draw.hpp"

using namespace std;
using namespace Eigen;


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
  MatrixXd list1;
  MatrixXd list2;
  MatrixXd list3;
  kn::loadMatrix(list1,"input/list1.list");
  kn::loadMatrix(list2,"input/list2.list");
  kn::loadMatrix(list3,"input/list3.list");





 /********************************************************************

  First step : find the tensor from the three lists

  *********************************************************************/


MatrixXf A = MatrixXf::Zero(28, 27);

  // filling A
  for (int p=0; p<7; ++p) {
    for (int i=0; i<2; ++i) {
      for (int l=0; l<2; ++l) {
        for (int k=0; k<3; ++k) {
          A(4*p + 2*i + l, 3*3*2 + 3*l + k) = list1(p, k) * list2(p, i) * list3(p, 2);
          A(4*p + 2*i + l, 3*3*i + 3*l + k) = - list1(p, k) * list2(p, 2) * list3(p, 2);
          A(4*p + 2*i + l, 3*3*2 + 3*2 + k) = - list1(p, k) * list2(p, i) * list3(p, l);
          A(4*p + 2*i + l, 3*3*i + 3*2 + k) = list1(p, k) * list2(p, 2) * list3(p, l);
        }
      }
    }
  }

cout << "This is the Matrix A : " << endl << A << endl;

// Decomposition SVD of the Matrix A
JacobiSVD<MatrixXf> svd(A, ComputeThinU | ComputeThinV);

cout << "Its singular values are:" << endl << svd.singularValues() << endl;
//cout << "Its left singular vectors are the columns of the thin U matrix:" << endl << svd.matrixU() << endl;
cout << "Its right singular vectors are the columns of the thin V matrix:" << endl << svd.matrixV() << endl;


//VectorXf zeros = VectorXf::Zero(28);

// we have our tensor
Tensor tensor;
tensor.setCoord( svd.matrixV().transpose().col(26) );
print(tensor);


/********************************************************************

  Second step : transfert

*********************************************************************/

MatrixXf B = MatrixXf::Zero(4, 3);

// filling B
    for (int i=0; i<2; ++i) {
      for (int l=0; l<2; ++l) {
        for (int k=0; k<3; ++k) {
          B(2*i + l, 2) += list1(1, k) * list2(1, i) * tensor(2, l, k);
          B(2*i + l, 2) += - list1(1, k) * list2(1, 2) * tensor(i, l, k);
          B(2*i + l, l) += - list1(1, k) * list2(1, i) * tensor(2, 2, k);
          B(2*i + l, l) += list1(1, k) * list2(1, 2) * tensor(i, 2, k);
        }
      }
    }


// Decomposition SVD of the Matrix B
JacobiSVD<MatrixXf> svdB(B, ComputeThinU | ComputeThinV);

VectorXf solution(3);
VectorXf homogenSolution(3);
solution = svdB.matrixV().transpose().col(2);

for(int i=0; i<3; ++i) {
  homogenSolution[i] = solution[i] / solution[2];
}

cout << "Solution : " << endl << solution << endl;
cout << "Solution homogene : " << endl << homogenSolution << endl;




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


