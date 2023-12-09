#pragma once
#include "yssimplesound.h"
#include "GraphicFont.h"
#include "VisualLayerManager.h"
#include "ButtonCollection.h"
#include "model.h"
#include "Trainer.h"

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
	AppState currentState = HomeScreen;

	ArialFont arial;

	// for buttons
	int buttonKey = FSKEY_NULL;
	int buttonKeytest = FSKEY_NULL;
	ControlCollection menuButtons;
	ControlCollection homeButtons;
	ControlCollection layerButtons;
	ControlCollection LinearlayerButtons;
	ControlCollection ConvlayerButtons;
	ControlCollection RunButtons;
	ControlCollection ShowButtons;
	ControlCollection HelpButtons;
	ControlCollection deleteButton;
	ComboBox* myComboBox;
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

	// for handling layer visualizer
	VisualLayerManager myVisualLayersManager;
	VisualLayer* clickedVisLayer = nullptr;
	VisualLayer* selectedVislayer = nullptr;

	bool justChangedCombo = false;
	bool poolError = false;
	bool lastLayerSizeError = false;
	bool noModelFoundError = false;
	bool deleteLayerMode = false;
	bool SwitchSelected = false;

	Optim finalOptim = ADAM;
	int finalEpoch = 2;
	double finalLR = 0.001;
	bool CancelSelected1 = false;


public:

	Trainer* trainer;
	Model* model;

	Manager();

	// function displaying each new screen
	void manageHomeScreen(int wid, int hei);
	void manageMenuScreen(int wid, int hei);
	void manageLayerScreen(int wid, int hei);
	void manageConvLayerScreen();
	void manageLinLayerScreen();
	void manageRunScreen(int wid, int hei);
	void manageShowScreen(int wid, int hei);
	void manageHelpScreen(int wid, int hei);

	bool manage();
	void ShowHelpMenu();


	~Manager() {}

protected:
	// add buttons
	void addHomeButtons();
	void addMenuButtons();
	void addLayerButtons();
	void addLinLayerButtons();
	void addConvLayerButtons();
	void addRunButtons();
	void addShowButtons();
	void enableHelpButtons();
	void disableHelpButtons();

	// functions for visual interface
	void drawVisualLayers();
	void seeIfVisualLayerWasClicked();

	// check if model made is acceptable
	bool checkModelReqs(std::vector<Layer> finalModel);

	void addDeleteLayerButton();
	void deleteLastLayerOnly();
	void AccPlot();
	void LossPlot();

};