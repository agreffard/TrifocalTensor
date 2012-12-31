#include <iostream>
#include <set>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <Eigen/SVD>
#include <Tensor.hpp>
#include <tensorOperations.hpp>

#include "MathIO.hpp"
#include "draw.hpp"

using namespace std;
using namespace Eigen;

enum State {FILL_LISTS, TRANSFERT, SOLUTION};
State state;

void pushList(MatrixXf &list, int &nbRows, int x, int y){
	list.conservativeResize(nbRows+1,3);
	++nbRows;
	  
	list(nbRows-1,0) = x;
	list(nbRows-1,1) = y;
	list(nbRows-1,2) = 1;
}

void printHelp(){
	cout << endl << "------------- HELP --------------" << endl << endl;
	cout << "--- options ---" << endl;
	cout << "[-d] : load default point lists" << endl;
	cout << "[-l] : load last saved point lists" << endl;
	cout << "[file.list] | [file.mat] : load external point lists (previously saved)" << endl;
	cout << "[file.jpg] | [file.png] | [file.gif] : load external images -- 3 exactly needed, or the program loads default images" << endl;
	cout << endl;
	cout << "--- runtime commands ---" << endl;
	cout << "press s on the keyboard to save the current point lists (if not empty) in three files" << endl;
	cout << endl;
}

/*************************/
/*         MAIN          */
/*************************/

int main(int argc, char *argv[])
{
  
    // load the point lists
    /*MatrixXf list1;
    MatrixXf list2;
    MatrixXf list3;
    kn::loadMatrix(list1,"input/list1.list");
    kn::loadMatrix(list2,"input/list2.list");
    kn::loadMatrix(list3,"input/list3.list");*/

    MatrixXf myList1(1,3); int nbRows1 = 0;
    MatrixXf myList2(1,3); int nbRows2 = 0;
    MatrixXf myList3(1,3); int nbRows3 = 0;

    // points for the transfert
    MatrixXf myPoints = MatrixXf::Zero(3,3);
    set<int> pointsTransfert;
    set<int> unKnownImage;
    unKnownImage.insert(1);
    unKnownImage.insert(2);
    unKnownImage.insert(3);
    VectorXf solution = VectorXf::Zero(3);

    SDL_Surface * image1;
    SDL_Surface * image2;
    SDL_Surface * image3;

    Tensor tensor;

/*******************/
/*     OPTIONS     */
/*******************/
	uint8_t externalLists = 0;
	uint8_t externalImages = 0;
	
	cout << endl;
	
  if(argc > 1){
	  char** imagePath = new char*[argc];
	  string* tabArguments = new string[argc];
		
		
	  for(uint8_t i = 0; i < argc-1; ++i){
			tabArguments[i] = argv[i+1];
			imagePath[i] = argv[i+1];
			
			// help
			size_t findHelp = tabArguments[i].find("-help");
			if(findHelp != string::npos){
				printHelp();
				return(EXIT_SUCCESS);
			}
			
			// load default
			size_t findDefault = tabArguments[i].find("-d"); 	// defaults lists requested
			if(findDefault != string::npos){
				cout << "Try to load default lists" << endl;
				
				kn::loadMatrix(myList1,"input/list1.list"); nbRows1 = myList1.rows();
				kn::loadMatrix(myList2,"input/list2.list"); nbRows2 = myList2.rows();
				kn::loadMatrix(myList3,"input/list3.list"); nbRows3 = myList3.rows();
				
				cout << "Default lists loaded" << endl;
			}
			
			// load last saved point lists
			size_t findLast = tabArguments[i].find("-l"); 	// last saved point lists requested
			if(findLast != string::npos){
				cout << "Try to load last saved point lists" << endl;
				
				kn::loadMatrix(myList1,"myList1.list"); nbRows1 = myList1.rows();
				kn::loadMatrix(myList2,"myList2.list"); nbRows2 = myList2.rows();
				kn::loadMatrix(myList3,"myList3.list"); nbRows3 = myList3.rows();
				
				cout << "Last saved point lists loaded" << endl;
                cout << endl << "Calculation Tensor" << endl;
                fillTensor(tensor, myList1, myList2, myList3);
                tensor.print();
                cout << "truc " << endl;
                state = TRANSFERT;
			}
			
			// load external lists
			size_t findList = tabArguments[i].find(".list"); 	// external lists requested
			if(findList == string::npos){
				findList = tabArguments[i].find(".mat");				// another extension
			}
			if(findList != string::npos){
				cout << endl << "Try to load point list : " << tabArguments[i] << endl;
				if(externalLists == 3){
					cout << "Try to load too many lists !!" << endl;
				}	
				if(externalLists == 0){
					kn::loadMatrix(myList1,tabArguments[i]);
					nbRows1 = myList1.rows();
				}
				if(externalLists == 1){
					kn::loadMatrix(myList2,tabArguments[i]);
					nbRows2 = myList2.rows();
				}
				if(externalLists == 2){
					kn::loadMatrix(myList3,tabArguments[i]);
					nbRows3 = myList3.rows();
				}
				
				if(externalLists < 3){
					++externalLists;
				}
							
			} // end external lists loading
			
			// load images
			size_t findImages = tabArguments[i].find(".jpg");
			if(findImages == string::npos){
				findImages = tabArguments[i].find(".png");
			}
			if(findImages == string::npos){
				findImages = tabArguments[i].find(".gif");
			}
			if(findImages != string::npos){
				cout << endl << "Try to load image : " << tabArguments[i] << endl;
				if(externalImages == 3){
					cout << "Try to load too many images !!" << endl;
				}
				if(externalImages == 0){
					image1 = IMG_Load(imagePath[i]);
					if(image1 == 0){
						std::cerr << "error loading image" << std::endl;
						return 0;
					}
				}
				if(externalImages == 1){
					image2 = IMG_Load(imagePath[i]);
					if(image2 == 0){
						std::cerr << "error loading image" << std::endl;
						return 0;
					}
				}
				if(externalImages == 2){
					image3 = IMG_Load(imagePath[i]);
					if(image3 == 0){
						std::cerr << "error loading image" << std::endl;
						return 0;
					}
				}
				
				if(externalImages < 3){
					++externalImages;
				}
				
			} // end image loading
			
		} // end args browsing
		
		delete[] tabArguments;
		delete[] imagePath;
  } // end OPTIONS

	cout << endl;

/***** END OPTIONS *****/


/*******************/
/*       SDL       */
/*******************/

    // init SDL image
    if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == -1){
        std::cerr << "error IMG_Init" << std::endl;
        return EXIT_FAILURE;
    }

    // load some images

    if(externalImages != 3){
    	if(externalImages == 0){
    		cout << "loading default images" << endl;
    	}else{
    		cout << "3 images exactly needed, loading default images" << endl;
    	}
    	image1 = IMG_Load("input/image1.jpg");
    	image2 = IMG_Load("input/image2.jpg");
    	image3 = IMG_Load("input/image3.jpg");
    	if(image1 == 0 || image2 == 0 || image3 == 0){
    		std::cerr << "error loading images" << std::endl;
    		return 0;
    	}
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

/***** END SDL *****/

  
/*******************/
/*     DISPLAY     */
/*******************/


// some colors
Uint32 red  = 0xffff0000;
Uint32 blue = 0xff0000ff;
Uint32 yellow = 0xffffff00;

bool done = false;
while(!done){
  
    if (state==FILL_LISTS) {
        SDL_WM_SetCaption("Trifocal Tensor by Aurelien & Cedric - FILL_LISTS", NULL);
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
    }
    else if (state == TRANSFERT) {
        SDL_WM_SetCaption("Trifocal Tensor by Aurelien & Cedric - TRANSFERT", NULL);
        if (myPoints(0, 2)!=0) {
            fill_circle(screen, myPoints(0, 0), myPoints(0, 1), 3, red);
        }
        if (myPoints(1, 2)!=0) {
            fill_circle(screen, myPoints(1, 0)+image1->w, myPoints(1, 1), 3, red);
        }
        if (myPoints(2, 2)!=0) {
            fill_circle(screen, myPoints(2, 0)+image1->w+image2->w, myPoints(2, 1), 3, red);
        }
    }
    else if (state == SOLUTION) {
        SDL_WM_SetCaption("Trifocal Tensor by Aurelien & Cedric - SOLUTION", NULL);
        if (myPoints(0, 2)!=0) {
            fill_circle(screen, myPoints(0, 0), myPoints(0, 1), 3, red);
        }
        if (myPoints(1, 2)!=0) {
            fill_circle(screen, myPoints(1, 0)+image1->w, myPoints(1, 1), 3, red);
        }
        if (myPoints(2, 2)!=0) {
            fill_circle(screen, myPoints(2, 0)+image1->w+image2->w, myPoints(2, 1), 3, red);
        }
        int numImage = *(unKnownImage.begin())-1;
        fill_circle(screen, solution(0) + image1->w * numImage, solution(1), 3, red);
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
                cout << "writing lists..." << endl;
                if(nbRows1 != 0){
                    kn::saveMatrix(myList1, "myList1.list");
                }
                if(nbRows2 != 0){
                    kn::saveMatrix(myList2, "myList2.list");
                }
                if(nbRows3 != 0){
                    kn::saveMatrix(myList3, "myList3.list");
                }
                cout << "lists written" << endl;
            }
            if(e.key.keysym.sym == SDLK_RETURN){
                cout << endl << "Calculation Tensor" << endl;
                fillTensor(tensor, myList1, myList2, myList3);
                tensor.print();
                state = TRANSFERT;
            }
        } // end SDL_KEYUP

        if(e.type == SDL_MOUSEBUTTONUP){
            if (state==SOLUTION) {
                // we return to the transfert state so we reinitializate these variables :
                myPoints = MatrixXf::Zero(3, 3);
                pointsTransfert.clear();
                unKnownImage.clear();
                unKnownImage.insert(1);
                unKnownImage.insert(2);
                unKnownImage.insert(3);
                solution = VectorXf::Zero(3);
                state = TRANSFERT;
            }

            else if(e.button.x < image1->w) {
                if (state==FILL_LISTS) {
                    pushList(myList1, nbRows1, e.button.x % 400, e.button.y);
                }
                else if (state==TRANSFERT) {
                    // we insert the correspounding coordinates for the first point
                    myPoints.row(0) << e.button.x % 400, e.button.y, 1;
                    // we insert 1 in the known points and we remove 1 in the unkown images
                    pointsTransfert.insert(1);
                    unKnownImage.erase(1);

                    // if 2 points have been clicked we can do the transfert and display solution
                    if (pointsTransfert.size()==2) {
                        // the first point is the point at the first row of the matrix myPoints. 
                        // the seconc point is the point at the second (and last) row.
                        // the unknown image is the only one wich remain in the unKnownImage set.
                        solution = transfert(myPoints.row(*(pointsTransfert.begin())-1), myPoints.row(*(pointsTransfert.rbegin())-1), tensor, *(unKnownImage.begin()));
                        state = SOLUTION;
                    }
                }
            }

            else if(e.button.x >= image1->w && e.button.x < image1->w + image2->w) {
                if (state==FILL_LISTS) {
                    pushList(myList2, nbRows2, e.button.x % 400, e.button.y);
                }
                else if (state==TRANSFERT){
                    myPoints.row(1) << e.button.x % 400, e.button.y, 1;
                    pointsTransfert.insert(2);
                    unKnownImage.erase(2);

                    if (pointsTransfert.size()==2) {
                        solution = transfert(myPoints.row(*(pointsTransfert.begin())-1), myPoints.row(*(pointsTransfert.rbegin())-1), tensor, *(unKnownImage.begin()));
                        state = SOLUTION;
                    }
                }
            }

            else if(e.button.x >= image1->w + image2->w) {
                if (state==FILL_LISTS) {
                    pushList(myList3, nbRows3, e.button.x % 400, e.button.y);
                }
                else if (state==TRANSFERT) {
                    myPoints.row(2) << e.button.x % 400, e.button.y, 1;
                    pointsTransfert.insert(3);
                    unKnownImage.erase(3);

                    if (pointsTransfert.size()==2) {
                        solution = transfert(myPoints.row(*(pointsTransfert.begin())-1), myPoints.row(*(pointsTransfert.rbegin())-1), tensor, *(unKnownImage.begin()));
                        state = SOLUTION;
                    }
                }
            }
        } // end SDL_MOUSEBUTTONUP

    } // end events

} // end display

        /***** END DISPLAY *****/



  // quit sdl
  SDL_FreeSurface(image1); 
  SDL_FreeSurface(image2); 
  SDL_FreeSurface(image3); 
  IMG_Quit();
  SDL_Quit();

  return (EXIT_SUCCESS);
}



