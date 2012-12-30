#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <Eigen/SVD>
#include <Tensor.hpp>
#include <tensorOperations.hpp>

#include "MathIO.hpp"
#include "draw.hpp"

using namespace std;
using namespace Eigen;


void pushList(MatrixXd &list, int &nbRows, int x, int y){
	list.conservativeResize(nbRows+1,3);
	++nbRows;
	  
	list(nbRows-1,0) = x;
	list(nbRows-1,1) = y;
	list(nbRows-1,2) = 1;
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
  SDL_WM_SetCaption("Trifocal Tensor by Aurelien & Cedric", NULL);

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


  MatrixXd myList1(1,3); int nbRows1 = 0;
  MatrixXd myList2(1,3); int nbRows2 = 0;
  MatrixXd myList3(1,3); int nbRows3 = 0;

  // save a list
  kn::saveMatrix(list1,"/tmp/myList.mat");

  // some colors
  Uint32 red  = 0xffff0000;
  Uint32 blue = 0xff0000ff;
  Uint32 yellow = 0xffffff00;
  
  //~ kn::loadMatrix(myList1,"myList1.list");
  //~ nbRows1 = myList1.rows(); // don't forget to update if a matrix is loaded
  
  bool done = false;
  while(!done){
	  
	  // draw points on image1
	  if(nbRows1 != 0){
		for(int i=0; i<myList1.rows(); ++i)
			fill_circle(screen, myList1(i,0), myList1(i,1), 3, red);
	  }

	  // draw points on image2
	  if(nbRows2 != 0){
		for(int i=0; i<myList2.rows(); ++i)
			fill_circle(screen, myList2(i,0)+image1->w, myList2(i,1), 3, blue);
	  }

	  // draw points on image3
	  if(nbRows3 != 0){
		for(int i=0; i<myList3.rows(); ++i)
			fill_circle(screen, myList3(i,0)+image1->w+image2->w, myList3(i,1), 3, yellow);
	  }

	  // display everything
	  SDL_Flip(screen);
	  //~ pause();
	  
	  SDL_Event e;
	  while(SDL_PollEvent(&e)) {
		  if(e.type == SDL_QUIT){
			  done = true;
			  break;
		  }
		  
		  if(e.type == SDL_KEYUP){
			  if(e.key.keysym.sym == SDLK_ESCAPE){
				  done = true;
				  break;
			  }
			  if(e.key.keysym.sym == SDLK_SPACE){
				  cout << endl;
				  cout << "list1" << endl;
				  cout << myList1 << endl;
				  cout << endl;
				  cout << "list2" << endl;
				  cout << myList2 << endl;
				  cout << endl;
				  cout << "list3" << endl;
				  cout << myList3 << endl;
			  }
			  if(e.key.keysym.sym == SDLK_s){
				  if(nbRows1 != 0){
					  kn::saveMatrix(myList1, "myList1.list");
				  }
				  if(nbRows2 != 0){
					  kn::saveMatrix(myList2, "myList2.list");
				  }
				  if(nbRows3 != 0){
					  kn::saveMatrix(myList3, "myList3.list");
				  }
			  }
		  } // end SDL_KEYUP
		  
		  if(e.type == SDL_MOUSEBUTTONUP){
			  if(e.button.x < image1->w)
				pushList(myList1, nbRows1, e.button.x % 400, e.button.y);
				
			  if(e.button.x >= image1->w && e.button.x < image1->w + image2->w)
				pushList(myList2, nbRows2, e.button.x % 400, e.button.y);
				
			  if(e.button.x >= image1->w + image2->w)
				pushList(myList3, nbRows3, e.button.x % 400, e.button.y);
		  } // end SDL_MOUSEBUTTONUP
		  
	  } // end events
	  
  } // end display
  
  



  // quit sdl
  SDL_FreeSurface(image1); 
  SDL_FreeSurface(image2); 
  SDL_FreeSurface(image3); 
  IMG_Quit();
  SDL_Quit();

  return (EXIT_SUCCESS);
}



