#pragma once
#include "yssimplesound.h"
#include "GraphicFont.h"
//#include "Maze.h"
//#include "Entity.h"
#include "ButtonCollection.h"
struct Cell {
	int row;
	int col;
};
enum AppState {
	MainMenu,
	CreateScreen,
	LoadScreen,
	GraphScreen,// New state for graph display
	InputLinearLayer,
	InputConvLayer
};

class MazeManager {

protected:
	
	bool showVisited = false;
	bool pathIsFound = false;
	bool pathIsPossible = false;

	YsSoundPlayer theSoundPlayer;

	YsSoundPlayer::SoundData yesMoveSound;
	YsSoundPlayer::SoundData cantMoveSound;

	int key = FSKEY_NULL;

	// for handling mouse events
	int mouseEvent;
	int leftButton, middleButton, rightButton;
	int prevScreenX, prevScreenY, screenX, screenY;

	// for zooming and panning
	float originX = 0., originY = 0., scale = 1.;
	float scaleFactor = 1.05;

	bool soundIsOn = true;
	AppState currentState;

	ArialFont arial;

	// stuff for buttons
	int buttonKey = FSKEY_NULL;
	ButtonCollection myButtons;
	ComicSansFont buttonFont;
	int buttonX = 330;
	int buttonWidth = 150;
	int buttonHeight = 40;


public:
	// PS07  PS07  PS07  PS07  PS07  
	// asks user for width and height and asks Maze object 
	// to regenerate a random maze
	// 
	//void createNewMaze();

	MazeManager();

	void manageMainMenu(int wid, int hei);
	void manageCreateScreen(int wid, int hei);
	void manageLoadScreen(int wid, int hei);
	void manageGraphScreen(int wid, int hei);
	void manageInputLinearLayer();
	void manageInputConvLayer();

	bool manage();

	/*void loadFile();
	void saveFile();*/
	void showMenu();

	~MazeManager() {
		//delete theEntity;
	}

	//void zoomAll();
	Cell getModelCoords(int screenX, int screenY);

protected:
	void addHomeButtons();
	void drawLoadButtons();
	void setButtonStates();
};