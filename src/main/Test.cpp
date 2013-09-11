//============================================================================
// Name        : SDLScrolling.cpp
// Author      : Kevin Navero
// Version     :
// Copyright   : 
// Description : SDL Scrolling Test
//============================================================================
#include <iostream>
#include "SDLAbstractionLayer.h"
#include "Exception.h"
#include "Figure.h"
#include "RectBoundaryFigure.h"
#include "CircBoundaryFigure.h"
#include "TempFigure.h"
<<<<<<< HEAD
#include "Editor/Editor.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
=======
#include "PlayerFigure.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
>>>>>>> 91ae34208de03c46554e1ba6bad2d8e87c33675d

using namespace std;

const int FRAMERATE = 30;

const int DOT_WIDTH = 20;
const int DOT_HEIGHT = 20;

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 550;
const int LEVEL_WIDTH = 1191;
const int LEVEL_HEIGHT = 670;

const double FS = 200;
const double CS = 1200;
const double CJS = 11;
const double FJS = 7;
const double G = 4;
const int CNC = 1;
const int FNC = 4;

const bool OTHER_LEADER = false;
const double OTHER_SPEED = 0;
const double OTHER_GRAVITY = 0;
const double OTHER_JUMPSTRENGTH = 0;
const int OTHER_NUMCLIPS = 1;

const string TTF_PATH = "fonts/lazy.ttf";
const int FONT_SIZE = 28;
const Surface::Color FONT_COLOR = Surface::BLACK;

const Figure::Gravity gravEnDis = Figure::GRAVITY_ENABLED;
//const Figure::Gravity gravEnDis = Figure::GRAVITY_DISABLED;

/*
 * Description: This client simply tests the API and its capabilities of creating a
 * simple 2d scrolling platform game. This will serve as an example for now.
 */
int main(int argc, char* argv[]) {
<<<<<<< HEAD
	if (TEST_GRAPHICS) {

		//collision map container (objects in scene)
		//vector<Figure*>* collisions = NULL;

		SDL_Surface* screen = init(SCREEN_WIDTH, SCREEN_HEIGHT, "SDL Scrolling");

		vector<Figure*>* collisions = NULL;

		printf("Creating editor...");
		//start up editor
		Editor myLevelMap;
		printf("Done!\n");

		printf("Attempting to read file...");
		myLevelMap.setFile("resources/level.txt", Editor::read);
		collisions = myLevelMap.decode();
		printf("Size: %d\n",collisions->size());

		Surface bgnd("images/bgnd.jpg");
		Surface dot("images/dot.png", Surface::CYAN);
		Surface foo("images/Cyan_Final.png", Surface::BLACK);
		Surface rect("images/rectangle.png");
		Surface coin("images/coin.png", Surface::CYAN);

		Surface red("images/red.bmp", Surface::CYAN);
		Surface blue("images/blue.bmp", Surface::CYAN);
		Surface green("images/green.bmp", Surface::CYAN);
		Surface shimmer("images/shimmer.bmp", Surface::CYAN);

		RectFigure rf1(300, 525, rect, screen, Figure::GRAVITY_DISABLED, LEVEL_WIDTH, LEVEL_HEIGHT, OTHER_LEADER,
				OTHER_SPEED, OTHER_GRAVITY, OTHER_JUMPSTRENGTH, OTHER_NUMCLIPS, Figure::BOUNDARY, &red, &shimmer);
		RectFigure rf2(500, 125, rect, screen, Figure::GRAVITY_DISABLED, LEVEL_WIDTH, LEVEL_HEIGHT, OTHER_LEADER,
				OTHER_SPEED, OTHER_GRAVITY, OTHER_JUMPSTRENGTH, OTHER_NUMCLIPS);
		CircFigure cf1(700, 525, dot, screen, Figure::GRAVITY_DISABLED, LEVEL_WIDTH, LEVEL_HEIGHT, OTHER_LEADER,
				OTHER_SPEED, OTHER_GRAVITY, OTHER_JUMPSTRENGTH, OTHER_NUMCLIPS);
		CircFigure cf2(900, 350, dot, screen, Figure::GRAVITY_DISABLED, LEVEL_WIDTH, LEVEL_HEIGHT, OTHER_LEADER,
				OTHER_SPEED, OTHER_GRAVITY, OTHER_JUMPSTRENGTH, OTHER_NUMCLIPS, Figure::BOUNDARY, &red, &shimmer);
		TempFigure coin1(600, 325, coin, screen, LEVEL_WIDTH, LEVEL_HEIGHT);

		RectFigure rf;
		CircFigure cf;

		if (FOO) {
			rf.setFigure(100, 300, foo, screen, gravEnDis, LEVEL_WIDTH, LEVEL_HEIGHT, true, FS, G, FJS, FNC,
					Figure::PLAYER, &red, &green, &blue, &shimmer);

			(*collisions).push_back(&rf);
		}
		else {
			cf.setFigure(100, 300, dot, screen, gravEnDis, LEVEL_WIDTH, LEVEL_HEIGHT, true, CS, G, CJS, CNC);

			(*collisions).push_back(&cf);
		}

		//Test constructors
		/*
		 RectFigure rf(100, 300, foo, screen, gravEnDis, true, FS, G, FJS, FNC,
		 LEVEL_WIDTH, LEVEL_HEIGHT, &red, &green, &blue, &shimmer);
		 CircFigure cf(100, 300, dot, screen, gravEnDis, true, CS, G, CJS, CNC,
		 LEVEL_WIDTH, LEVEL_HEIGHT, &red, &green, &blue, &shimmer);
		 */

		bool quit = false;
		SDL_Event event;
		Timer timer;

		/*
		(*collisions).push_back(&rf1);
		(*collisions).push_back(&rf2);
		(*collisions).push_back(&cf1);
		(*collisions).push_back(&cf2);
		(*collisions).push_back(&coin1);
		*/

		Music m("resources/tristam.mp3");

		timer.start();

		if (Mix_PlayingMusic() == 0) if (Mix_PlayMusic(m.getMix_Music(), -1) < 0) throw SoundException();

		Mix_VolumeMusic(32); //0 to 128

		while (!quit) {
			if (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) quit = true;

				if (FOO) rf.handleInput(event);
				else cf.handleInput(event);
			}

			if (FOO) {
				rf.move((*collisions), timer.getTicks());
				timer.start();

				applySurface(0, 0, bgnd, screen, rf.getCameraClip());
				rf.show(rf.getCameraClip());

				//TODO Kevin - commented this for now
				for (int i = 0; i < (int) (*collisions).size(); i++) {
					(*collisions)[i]->show(rf.getCameraClip());
				}
			}
			else {
				cf.move(*collisions, timer.getTicks());
				timer.start();

				applySurface(0, 0, bgnd, screen, cf.getCameraClip());
				cf.show();

				rf1.show(cf.getCameraClip());
				rf2.show(cf.getCameraClip());
				cf1.show(cf.getCameraClip());
				cf2.show(cf.getCameraClip());
			}

			flip(screen);
		}

		Mix_HaltMusic();

		if (TEST_STRING_INPUT) {
			quit = false;
			bool nameEntered = false;

			StringInput name(TTF_PATH, FONT_SIZE, FONT_COLOR, screen);
			Surface msg(TTF_PATH, FONT_SIZE, FONT_COLOR, "New High Score! Enter Name: ");

			fillScreen(screen, Surface::WHITE);
			applySurface(getHorizontalMiddlePosition(msg, screen), 100, msg, screen);
			flip(screen);

			while (!quit) {
				if (SDL_PollEvent(&event)) {
					if (event.type == SDL_QUIT) {
						quit = true;
						break;
					}

					if (!nameEntered) {
						name.handleInput(event);

						if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
							nameEntered = true;
							msg.setSDL_Surface(TTF_PATH, FONT_SIZE, FONT_COLOR, "Rank 1st: ");
							quit = true;
						}
					}

					fillScreen(screen, Surface::WHITE);
					applySurface(getHorizontalMiddlePosition(msg, screen), 100, msg, screen);
					name.showCentered();

					flip(screen);

					if (nameEntered) SDL_Delay(500);
				}
			}
		}
	}

	cleanUp();

	return 0;
=======
   //initialize screen, video mode, SDL, ttf, audio, etc.
   SDL_Surface* screen = init(SCREEN_WIDTH, SCREEN_HEIGHT, "SDL Scrolling");

   //initialize all images
   Surface bgnd("images/bgnd.jpg");
   Surface dot("images/dot.png", Surface::CYAN);
   Surface foo("images/Cyan_Final.png", Surface::BLACK);
   Surface rect("images/rectangle.png");
   Surface coin("images/coin.png", Surface::CYAN);

   Surface red("images/red.bmp", Surface::CYAN);
   Surface blue("images/blue.bmp", Surface::CYAN);
   Surface green("images/green.bmp", Surface::CYAN);
   Surface shimmer("images/shimmer.bmp", Surface::CYAN);

   //convert images to Figures
   RectBoundaryFigure rf1(300, 525, rect, screen, LEVEL_WIDTH, LEVEL_HEIGHT,
         OTHER_NUMCLIPS);
   RectBoundaryFigure rf2(500, 125, rect, screen, LEVEL_WIDTH, LEVEL_HEIGHT,
         OTHER_NUMCLIPS);
   CircBoundaryFigure cf1(700, 525, dot, screen, LEVEL_WIDTH, LEVEL_HEIGHT,
         OTHER_NUMCLIPS);
   CircBoundaryFigure cf2(900, 350, dot, screen, LEVEL_WIDTH, LEVEL_HEIGHT,
         OTHER_NUMCLIPS);

   TempFigure coin1(600, 325, coin, screen, LEVEL_WIDTH, LEVEL_HEIGHT);

   PlayerFigure rf(100, LEVEL_HEIGHT - foo.getSDL_Surface()->h / 2, foo, screen,
         FS, G, FJS, FNC, LEVEL_WIDTH, LEVEL_HEIGHT, &red, &green, &blue,
         &shimmer);

   //Prepare music to be played within level
   Music m("resources/tristam.mp3");

   //collision vector - contains all the Figures (pointers to Figures) that
   //must be taken into account in regards to collision detection
   vector<Figure*> collisions;

   collisions.push_back(&rf);
   collisions.push_back(&rf1);
   collisions.push_back(&rf2);
   collisions.push_back(&cf1);
   collisions.push_back(&cf2);
   collisions.push_back(&coin1);

   //Prepare bool quit variable, SDL_Event event, and Timer timer. All of these
   //variables will be used in the event loop
   bool quit = false;
   SDL_Event event;
   Timer timer;
   timer.start();

   //Start music and set volume
   if (Mix_PlayingMusic() == 0)
      if (Mix_PlayMusic(m.getMix_Music(), -1) < 0)
         throw SoundException();

   Mix_VolumeMusic(32); //0 to 128

   //Start event loop
   while (!quit) {
      if (SDL_PollEvent(&event)) {
         if (event.type == SDL_QUIT)
            quit = true;

         //receive input for Player Figure
         rf.handleInput(event);
      }

      //move Player Figure to new position based on input given
      rf.move(collisions, timer.getTicks());

      //restart timer since movement is time-based and independent of framerate
      timer.start();

      //blit background image to screen with respect to the camera following Player Figure
      applySurface(0, 0, bgnd, screen, rf.getCameraClip());

      //show the Player Figure (argument is optional here since the relative camera is
      //the Player Figure's camera
      rf.show(rf.getCameraClip());

      //show all other Figures with respect to the relative camera i.e. the Player
      //Figure's camera
      rf1.show(rf.getCameraClip());
      rf2.show(rf.getCameraClip());
      cf1.show(rf.getCameraClip());
      cf2.show(rf.getCameraClip());

      //we need to move coin1 so that collision detection is not
      //ignored (although actual movement of coin1 doesn't happen because no input
      //is received). First argument is the collision vector. Second argument is the
      //delta ticks from the timer which is irrelevant since coin1 does not actually
      //move. It just disappears when collided with the Player Figure
      coin1.move(collisions, 0);

      //show coin1 based on the update caused by move()
      coin1.show(rf.getCameraClip());

      //update the screen by swapping video buffers
      flip(screen);
   }

   //stop music
   Mix_HaltMusic();

   //quit out of SDL, close audio, ttf, etc.
   cleanUp();

   return 0;
>>>>>>> 91ae34208de03c46554e1ba6bad2d8e87c33675d
}
