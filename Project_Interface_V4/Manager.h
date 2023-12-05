#pragma once
#include "yssimplesound.h"
#include "GraphicFont.h"
#include "VisualLayerManager.h"
#include "ButtonCollection.h"

struct Cell {
	int row;
	int col;
};
enum AppState {
	HomeScreen,
	MenuScreen,
	LayerScreen,
	OptScreen,
	ShowPlot,
	ShowHelp
    
};

class Manager {

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
	int buttonKeytest = FSKEY_NULL;
	//ButtonCollection myButtons;
	ControlCollection menuButtons;
	ControlCollection homeButtons;
	ControlCollection layerButtons;
	ControlCollection LinearlayerButtons;
	ControlCollection ConvlayerButtons;
	ControlCollection OptButtons;
	ControlCollection ShowButtons;
	ControlCollection HelpButtons;
	ComboBox* myComboBox;  // Add this line
	ComicSansFont buttonFont;
	ComicSansFont* textFont;
	int buttonX = 280;
	int buttonWidth = 140;
	int buttonHeight = 30;
	bool LayerSelected = false;
	bool ConvSelected = false;
	bool LinSelected = false;
	bool ReturnSelected = false;
	bool CancelSelected = false;
	bool HelpSelected = false;
	bool StartSelected = false;
	TextBox* inputtextbox = nullptr;

	VisualLayerManager myVisualLayersManager;
	VisualLayer* clickedVisLayer = nullptr;

	bool justChangedCombo = false;
	bool poolError = false;
	bool lastLayerSizeError = false;
	bool noModelFoundError = false;

public:
	// PS07  PS07  PS07  PS07  PS07  
	// asks user for width and height and asks Maze object 
	// to regenerate a random maze
	// 
	//void createNewMaze();

	Manager();

	void manageHomeScreen(int wid, int hei);

	void manageMenuScreen(int wid, int hei);
	void manageLayerScreen(int wid, int hei);
	void manageConvLayerScreen();
	void manageLinLayerScreen();
	void manageOptScreen(int wid, int hei);
	void manageShowScreen(int wid, int hei);
	void manageHelpScreen(int wid, int hei);
	//void drawLayerSections(int wid, int hei, int startY, int height);
	bool manage();

	/*void loadFile();
	void saveFile();*/


	~Manager() {
		//delete theEntity;
	}

	//void zoomAll();
	
	

protected:
	void addHomeButtons();
	void addMenuButtons();
	void addLayerButtons();
	void addLinLayerButtons();
	void addConvLayerButtons();
	void addOptButtons();
	void addShowButtons();
	void enableHelpButtons();
	void disableHelpButtons();

	void drawVisualLayers();
	void seeIfVisualLayerWasClicked();
	bool checkModelReqs(std::vector<Layer> finalModel);

};