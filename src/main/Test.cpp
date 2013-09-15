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
#include "PlayerFigure.h"
#include "GrabbableFigure.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>

#include "Editor/Editor.h"

using namespace std;

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 550;

const int FRAMERATE = 30;

const int DOT_WIDTH = 20;
const int DOT_HEIGHT = 20;

const int LEVEL_WIDTH = 1191;
const int LEVEL_HEIGHT = 670;

const double FS = 200;
const double CS = 1200;
const double CJS = 11;
const double FJS = 6;
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

	//editor test
	Editor editor;
	editor.setFile("resources/level.txt", Editor::read);
	Header* info = editor.readHeader();

	//initialize screen, video mode, SDL, ttf, audio, etc.
	SDL_Surface* screen = init(info->screen_w, info->screen_h, "SDL Scrolling");

	//initialize all images
	Surface bgnd(info->bg_path.c_str());
	Surface dot("images/dot.png", Surface::CYAN);
	Surface foo("images/Cyan_Final.png", Surface::BLACK);
	Surface rect("images/rectangle.png");
	Surface coin("images/coin.png", Surface::CYAN);
	Surface cloud("images/grab.png", Surface::CYAN);

	Surface red("images/red.bmp", Surface::CYAN);
	Surface blue("images/blue.bmp", Surface::CYAN);
	Surface green("images/green.bmp", Surface::CYAN);
	Surface shimmer("images/shimmer.bmp", Surface::CYAN);

	PlayerFigure rf(100, LEVEL_HEIGHT - foo.getSDL_Surface()->h / 2, foo,
			screen, FS, G, FJS, FNC, LEVEL_WIDTH, LEVEL_HEIGHT, &red, &green,
			&blue, &shimmer);

	//Prepare music to be played within level
	Music m("resources/tristam.mp3");

	//collision vector - contains all the Figures (pointers to Figures) that
	//must be taken into account in regards to collision detection
	vector<Figure*>* collisions;

	collisions = editor.decode();

	// add player object in after editor decoding
	collisions->push_back(&rf);

	//Prepare bool quit variable, SDL_Event event, and Timer timer. All of these
	//variables will be used in the event loop
	bool quit = false;
	SDL_Event event;
	Timer timer;
	timer.start();

	//Start music and set volume
	if (Mix_PlayingMusic() == 0) if (Mix_PlayMusic(m.getMix_Music(), -1) < 0) throw SoundException();

	Mix_VolumeMusic(32); //0 to 128

	//Start event loop
	while (!quit) {
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) quit = true;

			//receive input for Player Figure
			rf.handleInput(event);
		}

		// Move player to new position
		rf.move(*collisions, timer.getTicks());

		//filter out temp Figs
		vector<Figure*> filtered;	//everyone else
		vector<Figure*> tempFigs;	//tempFigs

		for (unsigned int i = 0; i < collisions->size(); i++) {
			if (typeid(*(collisions->at(i))) == typeid(TempFigure)) {
				tempFigs.push_back(collisions->at(i));
			}
			else if (typeid(*(collisions->at(i))) == typeid(PlayerFigure)) {
				//ignore the player Fig!
				continue;
			}
			else {
				filtered.push_back(collisions->at(i));
			}
		}

		//Gravity processing here
		for(unsigned int i = 0; i < filtered.size(); i++){
			if(filtered[i]->getGravEnable() == Figure::GRAVITY_ENABLED){
				filtered[i]->move(*collisions,timer.getTicks());
			}
		}

		//restart timer since movement is time-based and independent of framerate
		timer.start();

		//blit background image to screen with respect to the camera following Player Figure
		applySurface(0, 0, bgnd, screen, rf.getCameraClip());

		//draw all figures but temp Figs and player
		for (unsigned int i = 0; i < filtered.size(); i++) {
			filtered[i]->show(rf.getCameraClip());
		}

		//apply temp Figure processing here
		for(unsigned int i = 0; i < tempFigs.size();i++){
			tempFigs[i]->move(*collisions,0); //move em
			tempFigs[i]->show(rf.getCameraClip()); //draw em
		}

		//draw player at end
		rf.show();

		//update the screen by swapping video buffers
		flip(screen);
	}

//stop music
	Mix_HaltMusic();

//quit out of SDL, close audio, ttf, etc.
	cleanUp();

	return 0;
}
