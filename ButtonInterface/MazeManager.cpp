#include <iostream>
#include <string>
#include "fssimplewindow.h"
#include "DrawingUtilNG.h"
#include "StringPlus.h"
#include "MazeManager.h"

using namespace std;

//


MazeManager::MazeManager() {
	arial.setColorHSV(0, 0, 1, 1); // white

	// buttons
	addHomeButtons();
	buttonFont.setColorRGB(1, 0, 0); // black
}

bool MazeManager::manage()
{
	Cell modelMouseLoc;

	FsPollDevice();
	key = FsInkey();
	mouseEvent = FsGetMouseEvent(leftButton, middleButton,
		rightButton, screenX, screenY);
	buttonKey = FSKEY_NULL;  // this resets the buttonKey if no buttons are clicked
	// check if a button was clicked
	if (key == FSKEY_NULL && mouseEvent == FSMOUSEEVENT_LBUTTONDOWN) {
		buttonKey = myButtons.checkClick(screenX, screenY);

		if (buttonKey != FSKEY_NULL)
			key = buttonKey;  // pretend the user pressed a key 
	}

	

	// take care of window resizing ?
	int wid, hei;
	FsGetWindowSize(wid, hei);
	glViewport(0, 0, wid, hei);


	//glLoadIdentity();
	// draw all the stuff
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	

	switch (currentState) {
	case MainMenu:
		manageMainMenu(wid, hei);
		break;
	case CreateScreen:
		manageCreateScreen(wid, hei);
		break;
	case LoadScreen:
		manageLoadScreen(wid,hei);
		break;
	case GraphScreen:
		manageGraphScreen(wid, hei);
		break;
	case InputLinearLayer:
		manageInputLinearLayer();
		break;
	case InputConvLayer:
		manageInputConvLayer();
		break;
	}



	return true;
}


void MazeManager::manageMainMenu(int wid, int hei) {
	// Draw the main menu UI elements, like buttons
	glLoadIdentity();

	// Draw the title
	arial.setColorHSV(0, 1, 1, 1); // Red color
	int textX = (wid / 2) - (10 / 2);
	arial.drawText("Deep Learning Model Simulator", textX, 350, 0.6, 0.0, true);// Adjust position and size as needed

	// Check for button clicks
	if (buttonKey == FSKEY_C) {
		currentState = CreateScreen;
	}
	else if (buttonKey == FSKEY_L) {
		currentState = LoadScreen;
	}


	myButtons.paint();
	myButtons.checkHover(screenX, screenY);


}

void MazeManager::manageCreateScreen(int wid, int hei) {
	 //Create screen specific rendering and logic
	arial.setColorHSV(0, 1, 1, 1); // Red color
	int textX = (wid / 2) - (10 / 2);
	arial.drawText("Create Screen: Implement your creation logic here",textX,350, 0.6, 0.0, true);

	//drawLoadButtons();
	// Implement your creation logic here
	// ...
}

void MazeManager::manageLoadScreen(int wid, int hei) {
	

	// You can adjust these values based on your screen layout and button sizes
	int startX = 730; // X position where buttons start (adjust based on window size)
	int startY = 50; // Y position where buttons start
	int spacing = 20; // Space between buttons

	// Draw the DATASET subheading
	arial.setColorHSV(0.7, 1, 0, 1);// Choose a color for the text
	
	arial.drawText("DATASET", startX+10, startY - 10, 0.3, 0.1, true); // Adjust the position and size as needed

	// Shift startY down to make room for the subheading
	startY += 10;

	// Enable the buttons (if you have a method for this in your button collection)
	myButtons.remove(FSKEY_L);
	myButtons.remove(FSKEY_C);

	// Clear previous buttons specific to this screen or create a separate button collection
	// myLoadScreenButtons.clear();

	// Adding buttons for the Load Screen
	myButtons.add(startX - 70, startY, buttonWidth, buttonHeight, FSKEY_1, "Data 1", &buttonFont, "select Dataset 1");
	startY += buttonHeight + spacing;
	myButtons.add(startX - 70, startY, buttonWidth, buttonHeight, FSKEY_2, "Data 2", &buttonFont, "Select Dataset 2");
	startY += buttonHeight + spacing + 20;

	arial.drawText("LAYERS", startX +10 , startY, 0.3, 0.1, true); // Adjust the position and size as needed
	startY += 20;

	myButtons.add(startX - 70, startY, buttonWidth, buttonHeight, FSKEY_F, "Linear", &buttonFont, "Add Linear Layer");
	startY += buttonHeight + spacing;
	myButtons.add(startX - 70, startY, buttonWidth, buttonHeight, FSKEY_A, "Activation", &buttonFont, "Add Activation Layer");
	startY += buttonHeight + spacing;
	myButtons.add(startX - 70, startY, buttonWidth, buttonHeight, FSKEY_G, "Convultional", &buttonFont, "Add Convulutional Layer");
	startY += buttonHeight + spacing;
	myButtons.add(startX - 70, startY, buttonWidth, buttonHeight, FSKEY_P, "Pooling", &buttonFont, "Add Pooling Layer");
	startY += buttonHeight + spacing;
	myButtons.add(startX - 70, startY, buttonWidth, buttonHeight, FSKEY_N, "Batch Norm", &buttonFont, "Add Batch Norm Layer");
	startY += buttonHeight + spacing;
	myButtons.add(startX - 70, startY, buttonWidth, buttonHeight, FSKEY_D, "Dropout", &buttonFont, "Add Dropout Layer");
	startY += buttonHeight + spacing + 20;


	arial.drawText("MENU", startX +10 , startY, 0.3, 0.1, true); // Adjust the position and size as needed
	startY += 20;

	myButtons.add(startX - 70, startY, buttonWidth, buttonHeight, FSKEY_T, "Train", &buttonFont, "Run Train");
	startY += buttonHeight + spacing;
	myButtons.add(startX - 70, startY, buttonWidth, buttonHeight, FSKEY_V, "Show", &buttonFont, "Display Model Architecture");
	startY += buttonHeight + spacing;
	myButtons.add(startX - 70, startY, buttonWidth, buttonHeight, FSKEY_X, "Return", &buttonFont, "Return to Main Menu");

	// Enable the buttons (if you have a method for this in your button collection)
	myButtons.enableButton(FSKEY_1);
	myButtons.enableButton(FSKEY_2);
	myButtons.enableButton(FSKEY_F);
	myButtons.enableButton(FSKEY_A);
	myButtons.enableButton(FSKEY_G);
	myButtons.enableButton(FSKEY_P);
	myButtons.enableButton(FSKEY_N);
	myButtons.enableButton(FSKEY_D);

	// Enable more buttons as needed
	// Draw the buttons
	myButtons.paint();
	myButtons.checkHover(screenX, screenY);


	if (buttonKey == FSKEY_V) { // Assuming FSKEY_D is the key for the "Show" button
		currentState = GraphScreen;
	}
	else if (buttonKey == FSKEY_F) {
		currentState = InputLinearLayer;
	}
	
	else if (buttonKey == FSKEY_G) {
		currentState = InputConvLayer;

	}
	
}

void MazeManager::manageGraphScreen(int wid, int hei) {
	// Clear the screen or set up the view
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();


	// Draw the title
	arial.setColorHSV(0, 1, 1, 1); // Red color
	int textX = (wid / 2) - (10 / 2);
	arial.drawText("Perfomance Graph", textX, 350, 0.6, 0.0, true);// Adjust position and size as needed
	// Render the graph
	// This is where you would draw the graph based on your data
	// ...

	// You can also draw other UI elements, such as a back button
	// ...
}

void MazeManager::manageInputLinearLayer() {
	string userInput;
	string errorString = " ";
	vector<int> layerDimensions(3, 0); // To store the three dimensions

	bool continueAsking = true;
	while (continueAsking) {
		userInput = DrawingUtilNG::getStringFromScreen(errorString,
			"Enter three dimensions for the Linear layer (separated by space)",
			"Format: width height depth (ESC to cancel)");

		if (userInput.length() > 0) {
			vector<string> splitInput = StringPlus::split(userInput, " ,");
			if (splitInput.size() == 3) {
				try {
					for (int i = 0; i < 3; ++i) {
						layerDimensions[i] = stoi(splitInput[i]);
					}
					continueAsking = false;
					// Handle the dimensions here or store them for later use
				}
				catch (exception& e) {
					errorString = "ERROR: Unable to convert input into numbers";
				}
			}
			else {
				errorString = "Please enter exactly three numbers";
			}
		}
		else {
			continueAsking = false; // User pressed ESC or entered blank
		}
	}

	// After input, return to the previous state or proceed as needed
	currentState = GraphScreen; // For example, return to the graph screen
}

void MazeManager::manageInputConvLayer() {
	string userInput;
	string errorString = " ";
	int numFilters, kernelWidth, kernelHeight;

	bool continueAsking = true;
	while (continueAsking) {
		userInput = DrawingUtilNG::getStringFromScreen(errorString,
			"Enter numFilters, kernelWidth, kernelHeight (separated by space)",
			"Format: numFilters kernelWidth kernelHeight (ESC to cancel)");

		if (userInput.length() > 0) {
			vector<string> splitInput = StringPlus::split(userInput, " ,");
			if (splitInput.size() == 3) {
				try {
					numFilters = stoi(splitInput[0]);
					kernelWidth = stoi(splitInput[1]);
					kernelHeight = stoi(splitInput[2]);
					continueAsking = false;
					// Use these values to define a convolutional layer
				}
				catch (exception& e) {
					errorString = "ERROR: Unable to convert input into numbers";
				}
			}
			else {
				errorString = "Please enter exactly three numbers";
			}
		}
		else {
			continueAsking = false; // User pressed ESC or entered blank
		}
	}

	currentState = GraphScreen; // For example, return to the graph screen
}

Cell MazeManager::getModelCoords(int screenX, int screenY) {

	// note row is Y and column is X
	return { int(screenY / scale + originY), int(screenX / scale + originX) };
}



void MazeManager::addHomeButtons()
{

	int spacing = 100;
	int currY = 550;
	int currX = buttonX;


	myButtons.add(currX, currY, buttonWidth, buttonHeight, FSKEY_L, "Load", &buttonFont,
		"Load a maze from a file");

	currX += buttonWidth + spacing;
	myButtons.add(currX, currY, buttonWidth, buttonHeight, FSKEY_C, "Create", &buttonFont,
		"Erase current maze and start a random maze of given size");

	//setButtonStates();
	myButtons.enableButton(FSKEY_L);
	myButtons.enableButton(FSKEY_C);


	myButtons.paint();
	myButtons.checkHover(screenX, screenY);
}


void MazeManager::drawLoadButtons() {
	// You can adjust these values based on your screen layout and button sizes
	int startX = 700; // X position where buttons start (adjust based on window size)
	int startY = 100; // Y position where buttons start
	int spacing = 50; // Space between buttons

	// Draw the DATASET subheading
	arial.setColorHSV(0, 1, 1, 1); // Choose a color for the text
	arial.drawText("DATASET", startX, startY - 30, 0.6, 0.0, true); // Adjust the position and size as needed

	// Shift startY down to make room for the subheading
	startY += 40;

	// Enable the buttons (if you have a method for this in your button collection)
	myButtons.remove(FSKEY_L);
	myButtons.remove(FSKEY_C);

	// Clear previous buttons specific to this screen or create a separate button collection
	// myLoadScreenButtons.clear();

	// Adding buttons for the Load Screen
	myButtons.add(startX, startY, buttonWidth, buttonHeight, FSKEY_1, "Button 1", &buttonFont, "Tooltip for Button 1");
	startY += buttonHeight + spacing;
	myButtons.add(startX, startY, buttonWidth, buttonHeight, FSKEY_2, "Button 2", &buttonFont, "Tooltip for Button 2");
	// Add more buttons as needed


	// Enable the buttons (if you have a method for this in your button collection)
	myButtons.enableButton(FSKEY_1);
	myButtons.enableButton(FSKEY_2);

	// Enable more buttons as needed
	// Draw the buttons
	myButtons.paint();
	myButtons.checkHover(screenX, screenY);
}

