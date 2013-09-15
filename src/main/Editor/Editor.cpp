/*
 * Editor.cpp
 *
 *  Created on: Sep 7, 2013
 *      Author: Alles Rebel
 */

#include "Editor.h"

/*
 * TODO: Load setting for numbers 1 -> 9 objects
 * in a text file that will be loaded when editor is opened
 */

Editor::Editor() {
	headerInfo = NULL;
	file = NULL;
}

Editor::Editor(string filepath, state rw) {
	headerInfo = NULL;
	file = NULL;
	setFile(filepath, rw);
}

/*
 * Change the file being read/written to
 * If a previous file was opened, it will close
 * the previous file, and attempt to open the next
 */
void Editor::setFile(string filepath, state rw) {
	if (file != NULL) fclose(file);
	if (rw == Editor::read) file = fopen(filepath.c_str(), "r");
	else if (rw == Editor::write) file = fopen(filepath.c_str(), "w");
}

/*
 * Write the header information to a file
 * Not required, just for flexibility
 * TODO: Probably should put a line here so reading will work
 * 		 like header v2 or something
 */
void Editor::writeHeader(Header header) {
	checkFile();

	fprintf(file, "%d %d %d %d\n", header.bg_w, header.bg_h, header.screen_w,
			header.screen_h);
	fprintf(file, "%s\n", header.bg_path.c_str());
	fprintf(file, "%s\n", header.bgm_path.c_str());
}

/*
 * If the file contains header information, it
 * cane be read with this function
 * Previous Header info will be deleted
 * TODO: probably should put a check in (like header v2 or something)
 */
Header* Editor::readHeader() {
	checkFile();

	if (headerInfo != NULL) {
		delete headerInfo;
	}

	headerInfo = new Header;

	char buffer[100];

	int check = fscanf(file, "%d %d %d %d", &headerInfo->bg_w,
			&headerInfo->bg_h, &headerInfo->screen_w, &headerInfo->screen_h);
	if (check != 4) {
		printf("Error: No level.txt found!\n");
		return NULL;
	}
	fscanf(file, "%s", buffer);

	stringstream ss;
	ss << buffer;
	headerInfo->bg_path = ss.str();

	char buffer2[100];
	fscanf(file, "%s", buffer2);
	stringstream ss2;
	ss2 << buffer2;
	headerInfo->bgm_path = ss2.str();

	return headerInfo;
}

/*
 * Encode the figures stored in the vector given
 * to the file opened. File should be opened
 * with write permissions!
 * Uses header to get Lvl info
 * TODO: Should also encode and decode classnames
 * for support for temp figs and circles
 */
void Editor::encode(vector<Figure*>* container, Header lvlInfo) {
	checkFile();

	if (container == NULL) {
		printf("Error: A null container was given to encode!\n");
		exit(1);
	}

	for (int i = 0; i < (int) container->size(); i++) {
		Figure* pointer = container->at(i);
		encodeFigure(pointer, lvlInfo.bg_w, lvlInfo.bg_h);
	}

	//TODO: Add other things like gravity enabled
	//to do this, just add an get method in the figure class
	//for now we'll assume it's off

}

/*
 * Decode the file, and creates a vector container
 * with the result. File should be opened with
 * read permissions!
 */
vector<Figure*>* Editor::decode() {
	checkFile();

	//create space in memory for objects
	vector<Figure*>* result = new vector<Figure*>;

	//read figures until end of file
	while (!feof(file)) {
		Figure* decodedFig = decodeFigure();
		//if we read a figure from the file then store it in to the result
		if (decodedFig != NULL) result->push_back(decodedFig);
		else if (!feof(file)){
			printf("Invalid figure - header skipped?\n");
			exit(1);
		}
	}

	return result;
}

Editor::~Editor() {
	if (file != NULL) fclose(file);
}

void Editor::checkFile() {
	if (file == NULL) {
		printf("Error! File pointer isn't assigned!\n");
		exit(1);
	}
}

/*
 * Print out the information for a single figure
 * TODO: Does typeid differ between sessions?
 */
void Editor::encodeFigure(Figure* fig, int lvlWidth, int lvlHeight) {
	int x = fig->getX();
	int y = fig->getY();
	int gravEnable = fig->getGravEnable();
	int colorkey = fig->getColorKey();
	string filePath = fig->getFilePath();
	int classType = translateTypeID(fig);
	int numClips = fig->getNumClips();
	fprintf(file, "%d %d %d %d %d %d %d %d\n", x, y, lvlWidth, lvlHeight,
			gravEnable, colorkey, classType, numClips);
	fprintf(file, "%s\n", filePath.c_str());
}

Figure* Editor::decodeFigure() {
	int x, y, lvlWidth, lvlHeight, gravEnable, colorkey, classType, numClips;
	char buffer[100];

	stringstream ss;
	int check = fscanf(file, "%d %d %d %d %d %d %d %d", &x, &y, &lvlWidth,
			&lvlHeight, &gravEnable, &colorkey, &classType, &numClips);

	if (check != 8) {
		//Invalid figure - or end of file
		return NULL;
	}

	fscanf(file, "%s", buffer);
	ss << buffer;
	string filepath = ss.str();

	//begin by creating memory for the surface
	Surface* mysurf = new Surface(filepath, (Surface::Color) colorkey);

	//decode the class type and handle the object creation
	Figure* newFig = NULL;
	switch (classType) {
		case rectBoundaryFigure: {
			newFig = new RectBoundaryFigure(x, y, *mysurf,
					SDL_GetVideoSurface(), lvlWidth, lvlHeight, numClips);
			break;
		}
		case circBoundryFigure: {
			newFig = new CircBoundaryFigure(x, y, *mysurf,
					SDL_GetVideoSurface(), lvlWidth, lvlHeight, numClips);
			break;
		}
		case grabbableFigure: {
			newFig = new GrabbableFigure(x, y, *mysurf, SDL_GetVideoSurface(),
					lvlWidth, lvlHeight, numClips);
			break;
		}
		case tempFigure: {
			newFig = new TempFigure(x, y, *mysurf, SDL_GetVideoSurface(),
					lvlWidth, lvlHeight);
			break;
		}
		case cursorFigure: {
			newFig = new CursorFigure(x, y, *mysurf, SDL_GetVideoSurface(),
			NULL);
			break;
		}
	}

	newFig->setGravEnable((Figure::Gravity) gravEnable);

	return newFig;
}

Editor::types Editor::translateTypeID(Figure* obj) {
	if (typeid(*obj) == typeid(RectBoundaryFigure)) {
		return rectBoundaryFigure;
	}
	else if (typeid(*obj) == typeid(CircBoundaryFigure)) {
		return circBoundryFigure;
	}
	else if (typeid(*obj) == typeid(TempFigure)) {
		return tempFigure;
	}
	else if (typeid(*obj) == typeid(GrabbableFigure)) {
		return grabbableFigure;
	}
	else if (typeid(*obj) == typeid(CursorFigure)) {
		return cursorFigure;
	}
	else if (typeid(*obj) == typeid(PlayerFigure)) {
		printf("Found a player object, please don't send them in!\n");
		exit(1);
	}
	else {
		printf("Error translating typeid- unknown class?\n");
		exit(1);
	}
}

void Editor::closeFile() {
	fclose(file);
}
