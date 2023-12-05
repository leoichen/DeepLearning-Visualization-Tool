#include <iostream>
#include <string>
#include "fssimplewindow.h"
#include "DrawingUtilNG.h"
#include "StringPlus.h"
#include "Manager.h"

using namespace std;

Manager::Manager() {
	arial.setColorHSV(1, 0, 0, 1); // white

	// buttons
	addHomeButtons();
	buttonFont.setColorRGB(1, 0, 0); // black

	// if

	addMenuButtons(); 
	addLayerButtons();
	addLinLayerButtons();
	addConvLayerButtons();
	addOptButtons();
	addShowButtons();
}

bool Manager::manage()
{
	Cell modelMouseLoc;

	FsPollDevice();
	key = FsInkey();
	mouseEvent = FsGetMouseEvent(leftButton, middleButton,
		rightButton, screenX, screenY);

	
	// take care of window resizing ?
	int wid, hei;
	FsGetWindowSize(wid, hei);
	glViewport(0, 0, wid, hei);

    glClearColor(0.53f, 0.81f, 0.92f, 1.0f); // RGBA, A is for alpha (opacity)


	//glLoadIdentity();
	// draw all the stuff
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


	// reset error messages when creating layer on click
	if (mouseEvent == FSMOUSEEVENT_LBUTTONDOWN)
	{
		poolError = false;
		lastLayerSizeError = false;
		noModelFoundError = false;

	}

	switch (currentState) {
	case HomeScreen:
	buttonKey = FSKEY_NULL;  // this resets the buttonKey if no buttons are clicked
	buttonKeytest = FSKEY_NULL;
	// check if a button was clicked
	if (key == FSKEY_NULL && mouseEvent == FSMOUSEEVENT_LBUTTONDOWN) {
		buttonKey = homeButtons.checkClick(screenX, screenY);

		if (buttonKey != FSKEY_NULL)
			key = buttonKey;  // pretend the user pressed a key 
	}
		manageHomeScreen(wid, hei);// open screen
		break;
	case MenuScreen: // main menu 
	buttonKey = FSKEY_NULL;  // this resets the buttonKey if no buttons are clicked
	buttonKeytest = FSKEY_NULL;
	// check if a button was clicked
	if (buttonKeytest == FSKEY_NULL && key == FSKEY_NULL && mouseEvent == FSMOUSEEVENT_LBUTTONDOWN) {
		buttonKeytest = menuButtons.checkClick(screenX, screenY);

		if (buttonKeytest != FSKEY_NULL)
			key = buttonKeytest;  // pretend the user pressed a key 
	}
		manageMenuScreen(wid, hei);
		break;
	case LayerScreen: // main menu 
		buttonKey = FSKEY_NULL;  // this resets the buttonKey if no buttons are clicked
		buttonKeytest = FSKEY_NULL;
		// check if a button was clicked
		if (buttonKeytest == FSKEY_NULL&&key == FSKEY_NULL && mouseEvent == FSMOUSEEVENT_LBUTTONDOWN) {
			buttonKey = layerButtons.checkClick(screenX, screenY);
			buttonKeytest = menuButtons.checkClick(screenX, screenY);
			if (buttonKey != FSKEY_NULL)
				key = buttonKey;  // pretend the user pressed a key 
		}
		manageMenuScreen(wid, hei);
		manageLayerScreen(wid, hei);
		break;
	case OptScreen:
		buttonKey = FSKEY_NULL;  // this resets the buttonKey if no buttons are clicked
		buttonKeytest = FSKEY_NULL;
		if (buttonKeytest == FSKEY_NULL && key == FSKEY_NULL && mouseEvent == FSMOUSEEVENT_LBUTTONDOWN) {
			buttonKey = OptButtons.checkClick(screenX, screenY);
			buttonKeytest = menuButtons.checkClick(screenX, screenY);
			if (buttonKey != FSKEY_NULL)
				key = buttonKey;  // pretend the user pressed a key 
		}
		manageMenuScreen(wid, hei);
		manageOptScreen(wid, hei);
		break;
	case ShowPlot:
		buttonKey = FSKEY_NULL;  // this resets the buttonKey if no buttons are clicked
		buttonKeytest = FSKEY_NULL;
		// check if a button was clicked
		if (key == FSKEY_NULL && mouseEvent == FSMOUSEEVENT_LBUTTONDOWN) {
			buttonKey = ShowButtons.checkClick(screenX, screenY);
			if (buttonKey != FSKEY_NULL)
				key = buttonKey;  // pretend the user pressed a key 
		}
		manageShowScreen(wid, hei);// open screen
		break;
	}

	if (LinSelected) {
		manageLinLayerScreen();
	}

	if (ConvSelected) {
		manageConvLayerScreen();
	}

	if (ConvSelected) {
		manageMenuScreen(wid, hei);
	}

	if (CancelSelected) {
		arial.setColorHSV(0, 1, 1, 1); // Red color
		arial.drawText("CANCELLED RUN ! ! !", 825, 430, 0.25, 0.1, true); // Adjust the position and size as needed
	}

	if (HelpSelected) {

		manageHelpScreen(wid, hei);

	}
	if (HelpSelected == true) {

		disableHelpButtons();
	}
	if (HelpSelected == false) {
		enableHelpButtons();

	}

	if (StartSelected) {
		
		arial.setColorHSV(0, 1, 1, 1); // Red color
		arial.drawText("STARTED RUN ! ! !", 825, 430, 0.25, 0.1, true); // Adjust the position and size as needed
	}

	if (poolError)
	{
		arial.setColorHSV(0, 1, 1, 1); // Red color
		arial.drawText("Pooling only allowed if", 825, 425, 0.18, 0.1, true); // Adjust the position and size as needed
		arial.drawText("input height*width is odd.", 825, 425+(arial.getLetterHeight()+5)*0.18, 0.18, 0.1, true); // Adjust the position and size as needed

	}

	if (lastLayerSizeError)
	{
		arial.setColorHSV(0, 1, 1, 1); // Red color
		arial.drawText("Last layer output size has to be 10.", 835, 425, 0.18, 0.1, true); // Adjust the position and size as needed
		arial.drawText("Pooling also not allowed.", 835, 425 + (arial.getLetterHeight() + 5) * 0.2, 0.18, 0.1, true); // Adjust the position and size as needed

	}

	if (noModelFoundError)
	{
		arial.setColorHSV(0, 1, 1, 1); // Red color
		arial.drawText("No model found.", 825, 430, 0.25, 0.1, true); // Adjust the position and size as needed
	}

	return true;
}

void Manager::manageHomeScreen(int wid, int hei) {
	// Draw the main menu UI elements, like buttons
	glLoadIdentity();

	// Draw the title
	arial.setColorHSV(0, 1, 1, 1); // Red color
	int textX = (wid / 2) - (10 / 2);
	arial.drawText("Deep Learning Model Simulator", textX, 300, 0.6, 0.0, true);// Adjust position and size as needed


	if (buttonKey == FSKEY_L) {
		currentState = MenuScreen;
	}

	homeButtons.paint();
	homeButtons.checkHover(screenX, screenY);

}

void Manager::manageMenuScreen(int wid, int hei) {


	// You can adjust these values based on your screen layout and button sizes
	int startX = 820; // X position where buttons start (adjust based on window size)
	int startY = 50; // Y position where buttons start
	int spacing = 20; // Space between buttons

	// Draw the DATASET subheading
	arial.setColorHSV(0.7, 1, 0, 1);// Choose a color for the text

	arial.drawText("M E N U", startX+5, startY, 0.3, 0.1, true); // Adjust the position and size as needed
	startY += 20;


	// Enable more buttons as needed
	// Draw the buttons
	menuButtons.paint();
	menuButtons.checkHover(screenX, screenY);

	int rectStartY = 500;
	// Width and height of the rectangle
	int rectWidth = wid; // full width of the window
	int rectHeight = (hei - rectStartY) - 20;

	// Set the outline color to blue
	glColor3f(0.0f, 0.0f, 0.0f); // Blue color

	// Draw the rectangle outline
	glLineWidth(2.0f); // Set the line width for the outline
	glBegin(GL_LINE_LOOP);
	glVertex2f(10, rectStartY);                     // Bottom Left
	glVertex2f(10, rectStartY + rectHeight);        // Top Left
	glVertex2f(rectWidth - 10, rectStartY + rectHeight);// Top Right
	glVertex2f(rectWidth - 10, rectStartY);             // Bottom Right
	glEnd();

	// Layer creation area
	glColor3f(1.0f, 1.0f, 1.0f); // white color
	glBegin(GL_QUADS);
	glVertex2f(10., 10);                     // Top Left
	glVertex2f(startX - 100, 10.);        // Top Right
	glVertex2f(startX - 100, rectStartY - 25);       // Bottom Right
	glVertex2f(10., rectStartY - 25);             // Bottom Left
	glEnd();

	drawVisualLayers();

	if (buttonKeytest == FSKEY_I) {
		if (currentState != LayerScreen) {
			currentState = LayerScreen;
		}
	}
	else if (buttonKeytest == FSKEY_O) {

		if (currentState != OptScreen) {
			currentState = OptScreen;
			LinSelected = false;
			ConvSelected = false;
		}

	}
	else if (buttonKeytest == FSKEY_V) {

		if (currentState != ShowPlot) {
			currentState = ShowPlot;
			LinSelected = false;
			ConvSelected = false;

		}

	}
	else if (buttonKeytest == FSKEY_H) {
		//cout << currentState << endl;
		HelpSelected = !HelpSelected;
	}

	else if (key == FSKEY_X) {
		//cout << currentState << endl;
		CancelSelected = !CancelSelected;
	}
	
	// see if a visual layer was clicked
	seeIfVisualLayerWasClicked();


}

void Manager::manageLayerScreen(int wid, int hei) {


	// You can adjust these values based on your screen layout and button sizes
	int startX = 800; // X position where buttons start (adjust based on window size)
	int startY = 50; // Y position where buttons start
	int spacing = 20; // Space between buttons

	// Draw the DATASET subheading
	arial.setColorHSV(0.7, 1, 0, 1);// Choose a color for the text

	// Enable more buttons as needed
	// Draw the buttons
	layerButtons.paint();
	layerButtons.checkHover(screenX, screenY);

	int lastButtonY = startY;
	int rectStartY = 500;


	// Width and height of the rectangle
	int rectWidth = wid; // full width of the window
	int rectHeight = (hei - rectStartY) - 20;

	// Set the outline color to blue
	glColor3f(0.0f, 0.0f, 0.0f); // Blue color

	// Draw the rectangle outline
	glLineWidth(2.0f); // Set the line width for the outline
	glBegin(GL_LINE_LOOP);
	glVertex2f(10, rectStartY);                     // Bottom Left
	glVertex2f(10, rectStartY + rectHeight);        // Top Left
	glVertex2f(rectWidth - 10, rectStartY + rectHeight);// Top Right
	glVertex2f(rectWidth - 10, rectStartY);             // Bottom Right
	glEnd();


	// linear
	if (key == FSKEY_L)
	{

		ConvSelected = false;
		LinSelected = true;//; LinSelected; // click once to show and again to unshow
		myVisualLayersManager.add(100, 200, "linear");
	}
	// conv
	else if (key == FSKEY_C) {

		{
			LinSelected = false;
			ConvSelected = true;
			myVisualLayersManager.add(100, 200, "conv");
		}

		if (buttonKeytest == FSKEY_I) {
			currentState = MenuScreen;
		}

	}
}

void Manager::manageOptScreen(int wid, int hei) {


	// You can adjust these values based on your screen layout and button sizes
	int startX = 100; // X position where buttons start (adjust based on window size)
	int startY = 520; // Y position where buttons start
	int spacing = 50; // Space between buttons


	// Draw the DATASET subheading
	arial.setColorHSV(0.7, 1, 0, 1);// Choose a color for the text
	arial.drawText("Choose Optmizer:",startX,startY, 0.25, 0.1, true); // Adjust the position and size as needed
	
	startX += spacing + arial.getWordWidth("Choose Optmizer :", 0.25)+20;
	arial.drawText("Choose Epochs to Train:", startX, startY, 0.25, 0.1, true); // Adjust the position and size as needed
	startX += arial.getWordWidth("Choose Epochs to Train :", 0.25)+20;
	arial.drawText("Choose Learning Rate:", startX, startY, 0.25, 0.1, true); // Adjust the position and size as needed
	// Enable more buttons as needed
	// Draw the buttons
	OptButtons.paint();
	OptButtons.checkHover(screenX, screenY);

	int rectStartY = 500;
	// Width and height of the rectangle
	int rectWidth = wid; // full width of the window
	int rectHeight = (hei - rectStartY) - 20;

	// Set the outline color to blue
	glColor3f(0.0f, 0.0f, 0.0f); // Blue color

	// Draw the rectangle outline
	glLineWidth(2.0f); // Set the line width for the outline
	glBegin(GL_LINE_LOOP);
	glVertex2f(10, rectStartY);                     // Bottom Left
	glVertex2f(10, rectStartY + rectHeight);        // Top Left
	glVertex2f(rectWidth - 10, rectStartY + rectHeight);// Top Right
	glVertex2f(rectWidth - 10, rectStartY);             // Bottom Right
	glEnd();


	if (buttonKeytest == FSKEY_O) {
		currentState = MenuScreen;
	}

	if (key== FSKEY_G)  // to start training;
	{
		std::vector<Layer> finalModel = myVisualLayersManager.getFinalModel();
		
		if (checkModelReqs(finalModel)) // passes test
		{
			std::cout << "NICE" << std::endl;
			StartSelected = !StartSelected;
		}
		else
		{
			std::cout << "UH OH" << std::endl;
		};

		
	}



}

void Manager::manageLinLayerScreen() {

	// Draw the buttons
	LinearlayerButtons.paint();
	LinearlayerButtons.checkHover(screenX, screenY);
	//inputtextbox->getString(key);

	if (leftButton){//key == FSKEY_NULL && mouseEvent == FSMOUSEEVENT_LBUTTONDOWN) {
		buttonKey = LinearlayerButtons.checkClick(screenX, screenY);

		// check if selected, if so change the edit bar values
		VisualLayer* selectedVislayer = myVisualLayersManager.checkSelected();
		if (selectedVislayer != nullptr)
		{
			int currIn = selectedVislayer->getLayer().inputSize;
			LinearlayerButtons.getControl<TextBox*>(1000)->setCurrText(std::to_string(currIn));

			int currOut = selectedVislayer->getLayer().outputSize;
			LinearlayerButtons.getControl<TextBox*>(2000)->setCurrText(std::to_string(currOut));

			double currDropout = selectedVislayer->getLayer().dropOut;
			std::string dropoutString;
			std::string formatDropoutValue;
			size_t decimalPosition;
			formatDropoutValue = std::to_string(currDropout);
			decimalPosition = formatDropoutValue.find(".");
			dropoutString = formatDropoutValue.substr(0, decimalPosition + 3);
			LinearlayerButtons.getControl<TextBox*>(4000)->setCurrText(formatDropoutValue.substr(0, decimalPosition + 3));

			actType currActivation = selectedVislayer->getLayer().activation;
			int currActivationVal = selectedVislayer->getActivationVal(currActivation);
			LinearlayerButtons.getControl<ComboBox*>(3000)->setCurrSelection(currActivationVal);

		

			//TextBox* dummyTextBox = new TextBox();
			std::string newText;
			if (buttonKey >= 1000)
			{
				// deal with combo boxes
				if (buttonKey >= 3000 && buttonKey <= 3999)
				{
					int currActivationValChange = currActivationVal;
					ComboBox* currComboBox = LinearlayerButtons.getControl<ComboBox*>(3000);
					currComboBox->getVal(screenX, screenY);
					currComboBox->setIsExpanded(true);
					currComboBox->paint();

					leftButton = 0;
					while (!leftButton)
					{

						FsPollDevice();
						mouseEvent = FsGetMouseEvent(leftButton, middleButton,
							rightButton, screenX, screenY);

						if (leftButton == 1)
						{
							currComboBox->getVal(screenX, screenY);
							selectedVislayer->setActivation(currComboBox->getCurrSelection());

							currActivationValChange = currComboBox->getCurrSelection();

							std::cout << "\nCurrS: " << currComboBox->getCurrSelection() << std::endl;
							//std::cout << "Val: " << currComboBox->getVal(screenX, screenY) << std::endl;

							currComboBox->setIsExpanded(false);
							break;
						}
					}
					currComboBox->setIsExpanded(false);

					//std::cout << selectedVislayer->getLayer().activation << std::endl;
				}


				else if ((buttonKey == 1000 && !selectedVislayer->getFixedInput())
					|| buttonKey == 2000 || buttonKey == 4000)
				{
					TextBox* currTextBox = LinearlayerButtons.getControl<TextBox*>(buttonKey);

					while (key != FSKEY_ENTER && key != FSKEY_ESC)
					{
						FsPollDevice();
						key = FsInkey();

						newText = currTextBox->getString(key);
						currTextBox->setCurrText(newText);
						currTextBox->paint();

						FsSwapBuffers();
						FsSleep(25);
					}


					switch (buttonKey)
					{
					case 1000:
						selectedVislayer->setInputSize(std::stoi(newText));
						break;
					case 2000:
						selectedVislayer->setOutputSize(std::stoi(newText));
						break;
					case 4000:
						selectedVislayer->setDropout(std::stod(newText));
						break;
					}
				}
			}
		}
		else
		{
			LinearlayerButtons.getControl<TextBox*>(1000)->setCurrText("");
			LinearlayerButtons.getControl<TextBox*>(2000)->setCurrText("");
			LinearlayerButtons.getControl<TextBox*>(4000)->setCurrText("");

			LinearlayerButtons.getControl<ComboBox*>(3000)->setCurrSelection(-1);

		}
		if (buttonKey != FSKEY_NULL)
			key = buttonKey;  // pretend the user pressed a key 
	}

}

void Manager::manageConvLayerScreen() {

	// Draw the buttons
	ConvlayerButtons.paint();
	ConvlayerButtons.checkHover(screenX, screenY);


	if (leftButton) {//key == FSKEY_NULL && mouseEvent == FSMOUSEEVENT_LBUTTONDOWN) {
		buttonKey = ConvlayerButtons.checkClick(screenX, screenY);

		// check if selected, if so change the edit bar values
		VisualLayer* selectedVislayer = myVisualLayersManager.checkSelected();
		if (selectedVislayer != nullptr)
		{
			int currIn = selectedVislayer->getLayer().inputSize;
			ConvlayerButtons.getControl<TextBox*>(1000)->setCurrText(std::to_string(currIn));

			int currOut = selectedVislayer->getLayer().outputSize;
			ConvlayerButtons.getControl<TextBox*>(2000)->setCurrText(std::to_string(currOut));

			double currDropout = selectedVislayer->getLayer().dropOut;
			std::string dropoutString;
			std::string formatDropoutValue;
			size_t decimalPosition;
			formatDropoutValue = std::to_string(currDropout);
			decimalPosition = formatDropoutValue.find(".");
			dropoutString = formatDropoutValue.substr(0, decimalPosition + 3);
			ConvlayerButtons.getControl<TextBox*>(4000)->setCurrText(formatDropoutValue.substr(0, decimalPosition + 3));

			actType currActivation = selectedVislayer->getLayer().activation;
			int currActivationVal = selectedVislayer->getActivationVal(currActivation);
			ConvlayerButtons.getControl<ComboBox*>(3000)->setCurrSelection(currActivationVal);

			selectedVislayer->checkPool();
			bool currPool = selectedVislayer->getLayer().pool;
			//std::cout << currPool << std::endl;

			ConvlayerButtons.getControl<ComboBox*>(6000)->setCurrSelection(currPool);

			std::string newText;
			if (buttonKey >= 1000)
			{

				if (buttonKey >= 5999 && buttonKey <= 6998 && selectedVislayer->getFixedPool())
				{
					poolError = true;
				}
				// deal with combo boxes
				else if ((buttonKey >= 3000 && buttonKey <= 3999) 
					|| (buttonKey >= 5999 && buttonKey <= 6998 && !selectedVislayer->getFixedPool()))
				{
					int currActivationValChange = currActivationVal;
					ComboBox* currComboBox = nullptr;

					if (buttonKey >= 3000 && buttonKey <= 3999)
					{
						currComboBox = ConvlayerButtons.getControl<ComboBox*>(3000);
					}
					else if (buttonKey >= 5999 && buttonKey <= 6998)
					{
						currComboBox = ConvlayerButtons.getControl<ComboBox*>(6000);
					}

					currComboBox->getVal(screenX, screenY);
					currComboBox->setIsExpanded(true);
					currComboBox->paint();

					leftButton = 0;
					while (!leftButton)
					{

						FsPollDevice();
						mouseEvent = FsGetMouseEvent(leftButton, middleButton,
							rightButton, screenX, screenY);

						if (leftButton == 1)
						{
							currComboBox->getVal(screenX, screenY);

							if (buttonKey >= 3000 && buttonKey <= 3999)
							{
								selectedVislayer->setActivation(currComboBox->getCurrSelection());
							}
							else if (buttonKey >= 6000 && buttonKey <= 6999)
							{
								std::cout << "C: " << currComboBox->getCurrSelection() << std::endl;
								selectedVislayer->setPool(currComboBox->getCurrSelection()); // yes == 0, no == 1;
							}

							currActivationValChange = currComboBox->getCurrSelection();
							currComboBox->setIsExpanded(false);
							break;
						}
					}
					currComboBox->setIsExpanded(false);

					//std::cout << selectedVislayer->getLayer().activation << std::endl;
				}


				else if ((buttonKey == 1000 && !selectedVislayer->getFixedInput())
					|| (buttonKey == 2000 || buttonKey == 4000))
				{
					TextBox* currTextBox = ConvlayerButtons.getControl<TextBox*>(buttonKey);

					while (key != FSKEY_ENTER && key != FSKEY_ESC)
					{
						FsPollDevice();
						key = FsInkey();

						newText = currTextBox->getString(key);
						currTextBox->setCurrText(newText);
						currTextBox->paint();

						FsSwapBuffers();
						FsSleep(25);
					}

					switch (buttonKey)
					{
					case 1000:
						selectedVislayer->setInputSize(std::stoi(newText));
						break;
					case 2000:
						selectedVislayer->setOutputSize(std::stoi(newText));
						break;
					case 4000:
						selectedVislayer->setDropout(std::stod(newText));
						break;
					}
				}
			}
		}
		else
		{
			ConvlayerButtons.getControl<TextBox*>(1000)->setCurrText("");
			ConvlayerButtons.getControl<TextBox*>(2000)->setCurrText("");
			ConvlayerButtons.getControl<TextBox*>(4000)->setCurrText("");

			ConvlayerButtons.getControl<ComboBox*>(3000)->setCurrSelection(-1);
			ConvlayerButtons.getControl<ComboBox*>(6000)->setCurrSelection(-1);


		}
		if (buttonKey != FSKEY_NULL)
			key = buttonKey;  // pretend the user pressed a key 
	}

}

void Manager::manageShowScreen(int wid, int hei) {
	// You can adjust these values based on your screen layout and button sizes
	int startX = 840; // X position where buttons start (adjust based on window size)
	int startY = 50;// Y position where buttons start
	int spacing = 40; // Space between buttons

	// Draw the DATASET subheading

	arial.setColorHSV(0, 1, 1, 1); // Red color
	arial.drawText("P R O P E R T I E S", startX, startY, 0.3, 0.1, true); // Adjust the position and size as needed
	startY += spacing;
	int epoch = 1; 
	string epochstring = "current epoch: " + std::to_string(epoch);
	arial.setColorHSV(0.7, 1, 0, 1);// Choose a color for the text
	arial.drawText(epochstring, startX, startY, 0.25, 0.1, true); // Adjust the position and size as needed
	startY += spacing;
	float learningRate = 0.0025;
	string lrstring = "current LR: " + std::to_string(learningRate);
	arial.drawText(lrstring, startX, startY, 0.25, 0.1, true); // Adjust the position and size as needed
	startY += spacing;
	int Accuracy = 25;
	string acstring = "current accuracy: " + std::to_string(Accuracy);
	arial.drawText(acstring, startX, startY, 0.25, 0.1, true); // Adjust the position and size as needed
	startY += spacing;

	//if (ReturnSelected) {
	//	arial.setColorHSV(0, 1, 1, 1); // Red color
	//	arial.drawText("CANCELLED", 840, 300, 0.3, 0.1, true); // Adjust the position and size as needed
	//}
	ShowButtons.paint();
	ShowButtons.checkHover(screenX, screenY);
	int rectStartY = 50;

	// Width and height of the rectangle
	int rectWidth = 30; // full width of the window
	int rectHeight = 600;

	// Set the outline color to blue
	glColor3f(0.0f, 0.0f, 0.0f); // Blue color

	// Draw the rectangle outline
	glLineWidth(2.0f); // Set the line width for the outline
	glBegin(GL_LINE_LOOP);
	glVertex2f(720, rectStartY);                     // Bottom Left
	glVertex2f(720, rectStartY + rectHeight);        // Top Left
	glVertex2f(rectWidth, rectStartY + rectHeight);// Top Right
	glVertex2f(rectWidth, rectStartY);             // Bottom Right
	glEnd();

	if (buttonKey == FSKEY_R) {
		//cout << currentState << endl;

		if (currentState != MenuScreen) {
			currentState = MenuScreen;
		}

		if (ReturnSelected = true) {
			ReturnSelected = !ReturnSelected;
		}
	}


	else if (key == FSKEY_X) {
		//cout << currentState << endl;
		CancelSelected = ! CancelSelected ;
	}

}

void Manager::manageHelpScreen(int wid, int hei) {

	int startX = 800; // X position where buttons start (adjust based on window size)
	int startY = 50;// Y position where buttons start
	int spacing = 20; // Space between buttons

	int rectStartY = 500;

	// Width and height of the rectangle
	int rectWidth = wid; // full width of the window
	int rectHeight = (hei -rectStartY)-20;

	glColor3f(1.0f, 1.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex2f(10, 10);                     // Top Left
	glVertex2f(startX-100, 10);        // Top right
	glVertex2f(startX - 100, rectStartY -25);// Bottom Right
	glVertex2f(10, rectStartY - 25);             // Bottom Left
	glEnd();

	

}

void Manager::addHomeButtons()
{

	int spacing = 100;
	int currY = 500;
	int currX = (910 / 2) - (buttonWidth/4) -40;
	homeButtons.add(currX, currY, buttonWidth, buttonHeight, FSKEY_L, "Start", &buttonFont,"Start the Application");

}

void Manager::addShowButtons()
{

	
	int startX = 780; // X position where buttons start (adjust based on window size)
	int startY = 200; // Y position where buttons start
	int spacing = 20; // Space between buttons
    
	startY += buttonHeight + spacing;
	ShowButtons.add(startX, startY, buttonWidth, buttonHeight, FSKEY_X, "Cancel", &buttonFont, "Stop Training");
	startY += buttonHeight + spacing;
	ShowButtons.add(startX, startY, buttonWidth, buttonHeight, FSKEY_R, "Return", &buttonFont, "Return");

}

void Manager::addMenuButtons() {
	int startX = 820; // X position where buttons start (adjust based on window size)
	int startY = 50; // Y position where buttons start
	int spacing = 20; // Space between buttons
	startY += 10;

	// Adding buttons for the Load Screen
	startY += buttonHeight + spacing;
	menuButtons.add(startX - 70, startY, buttonWidth, buttonHeight, FSKEY_I, "LAYER", &buttonFont, "Add Layer");
	startY += buttonHeight + spacing;
	menuButtons.add(startX - 70, startY, buttonWidth, buttonHeight, FSKEY_O, "RUN", &buttonFont, "Run Program");
	startY += buttonHeight + spacing;
	menuButtons.add(startX - 70, startY, buttonWidth, buttonHeight, FSKEY_V, "PLOT", &buttonFont, "Display Plot");
	startY += buttonHeight + spacing;
	menuButtons.add(startX - 70, startY, buttonWidth, buttonHeight, FSKEY_X, "CANCEL", &buttonFont, "Return to Main Menu");
	startY += buttonHeight + spacing;
	menuButtons.add(startX - 70, startY, buttonWidth, buttonHeight, FSKEY_H, "HELP", &buttonFont, "Show Help Box");
}

void Manager::addLayerButtons()
{
	int startX = 100; // X position where buttons start (adjust based on window size)
	int startY = 550; // Y position where buttons start
	int spacing = 30; // Space between buttons
	//startY += 20;

	layerButtons.add(startX, startY, buttonWidth, buttonHeight, FSKEY_C, "Conv", &buttonFont, "select Conv Layer");
	startY += buttonHeight + spacing;
	layerButtons.add(startX, startY, buttonWidth, buttonHeight, FSKEY_L, "Linear", &buttonFont, "select Linear Layer");
	cout << startX + buttonWidth << endl;

}

void Manager::addLinLayerButtons()
{
	int startX = 300; // X position where buttons start (adjust based on window size)
	int startY = 550; // Y position where buttons start
	int spacing = 50; // Space between buttons
	int spacingY = 60;

	int tempStartX = startX;
	int tempStartY = startY;


	arial.setColorRGB(0., 0., 0., 1.0);

	TextBox* inputTextBox = new TextBox(startX, startY, buttonWidth, buttonHeight, 1000, "Input Size", &buttonFont, "Select Type of Size");
	LinearlayerButtons.add(inputTextBox);


	startX += buttonWidth + spacing;
	TextBox* outputTextBox = new TextBox(startX, startY, buttonWidth, buttonHeight, 2000, "Output Size", &buttonFont, "Select Type of Size");
	LinearlayerButtons.add(outputTextBox);

	startX += buttonWidth + spacing;
	TextBox* dropoutTextBox = new TextBox(startX, startY, buttonWidth, buttonHeight, 4000, "Dropout", &buttonFont, "Select Type of Size");
	LinearlayerButtons.add(dropoutTextBox);


	// row 2
	startX = tempStartX;
	startY = tempStartY;
	startY += spacingY;
	ComboBox* activationComboBox = new ComboBox(startX, startY, buttonWidth, buttonHeight, 3000, "Activation", &buttonFont, "Select Type of Activation");
	activationComboBox->addItem("ReLU");
	activationComboBox->addItem("Sigmoid");
	activationComboBox->addItem("TanH");
	activationComboBox->addItem("None");
	LinearlayerButtons.add(activationComboBox);

	LinearlayerButtons.paint();

}

void Manager::addConvLayerButtons()
{
	int startX = 300; // X position where buttons start (adjust based on window size)
	int startY = 550; // Y position where buttons start
	int spacing = 50; // Space between buttons
	int spacingY = 60;
	
	int tempStartX = startX;
	int tempStartY = startY;


	arial.setColorRGB(0., 0., 0., 1.0);

	TextBox* inputTextBox = new TextBox(startX, startY, buttonWidth, buttonHeight, 1000, "Input Size", &buttonFont, "Select Type of Size");
	ConvlayerButtons.add(inputTextBox);


	startX += buttonWidth + spacing;
	TextBox* outputTextBox = new TextBox(startX, startY, buttonWidth, buttonHeight, 2000, "Output Size", &buttonFont, "Select Type of Size");
	ConvlayerButtons.add(outputTextBox);

	startX += buttonWidth + spacing;
	TextBox* dropoutTextBox = new TextBox(startX, startY, buttonWidth, buttonHeight, 4000, "Dropout", &buttonFont, "Select Type of Size");
	ConvlayerButtons.add(dropoutTextBox);


	// row 2
	startX = tempStartX;
	startY = tempStartY;
	startY += spacingY;
	ComboBox* activationComboBox = new ComboBox(startX, startY, buttonWidth, buttonHeight, 3000, "Activation", &buttonFont, "Select Type of Activation");
	activationComboBox->addItem("ReLU");
	activationComboBox->addItem("Sigmoid");
	activationComboBox->addItem("TanH");
	activationComboBox->addItem("None");
	ConvlayerButtons.add(activationComboBox);

	startX += buttonWidth + spacing;
	TextBox* kernelTextBox = new TextBox(startX, startY, buttonWidth, buttonHeight, 5000, "Kernel Size", &buttonFont, "Select Type of Size");
	ConvlayerButtons.add(kernelTextBox);

	startX += buttonWidth + spacing;
	ComboBox* poolComboBox = new ComboBox(startX, startY, buttonWidth, buttonHeight, 6000, "Max Pooling", &buttonFont, "Only Allowed if Input Size is Even");
	poolComboBox->addItem("No Pooling");
	poolComboBox->addItem("Yes Pooling");
	ConvlayerButtons.add(poolComboBox);

}

void Manager::addOptButtons()
{
	int startX = 30; // X position where buttons start (adjust based on window size)
	int startY = 540; // Y position where buttons start
	int spacing = 50; // Space between buttons


	ComboBox* OptType = new ComboBox(startX, startY, buttonWidth, buttonHeight, 3000, "Type", &buttonFont, "Select Type of Optimizer ");
	OptType->addItem("Adam");
	OptType->addItem("SGD");
	OptButtons.add(OptType);

	startX += buttonWidth + spacing + 10;
	ComboBox* EpochNumber = new ComboBox(startX, startY, buttonWidth, buttonHeight, 2000, "Type", &buttonFont, "Select # of epochs ");
	EpochNumber->addItem("4");
	EpochNumber->addItem("6");
	EpochNumber->addItem("8");
	EpochNumber->addItem("10");
	OptButtons.add(EpochNumber);

	startX += buttonWidth + spacing + 60;
	ComboBox* LR = new ComboBox(startX, startY, buttonWidth, buttonHeight, 1000, "Type", &buttonFont, "Select Starting LR ");
	LR->addItem("0.0005");
	LR->addItem("0.001");
	LR->addItem("0.002");
	LR->addItem("0.01");
	OptButtons.add(LR);

	startX += buttonWidth + spacing + 40;
	OptButtons.add(startX , startY, buttonWidth, buttonHeight, FSKEY_G, "START", &buttonFont, "Start Training");

}

void Manager::disableHelpButtons() {

	menuButtons.disableButton(FSKEY_I);
	menuButtons.disableButton(FSKEY_O);
	menuButtons.disableButton(FSKEY_V);
	menuButtons.disableButton(FSKEY_X);
}

void Manager::drawVisualLayers()
{
	myVisualLayersManager.drawVisualLayers();
}

void Manager::seeIfVisualLayerWasClicked()
{

	mouseEvent = FsGetMouseEvent(leftButton, middleButton,
		rightButton, screenX, screenY);

	// check if want to drag a block
	if (leftButton) {
		// check if a visual layer has been clicked
		clickedVisLayer = myVisualLayersManager.checkClick(screenX, screenY);

		// if it has been clicked move it to new center
		if (clickedVisLayer != nullptr)
		{
			myVisualLayersManager.changetoSelectMode(clickedVisLayer);
			myVisualLayersManager.moveVisualLayers(clickedVisLayer, screenX, screenY);

			currentState = LayerScreen;
			if (clickedVisLayer->getLayer().type == LINEAR)
			{
				ConvSelected = false;
				LinSelected = true;
			}
			else
			{
				LinSelected = false;
				ConvSelected = true;
			}

			// find nearby visual layers
			// change color of nearby visual layer to orange
			myVisualLayersManager.isNearVicinity(clickedVisLayer, "color");
		}
	}

	// update leftButton
	mouseEvent = FsGetMouseEvent(leftButton, middleButton,
		rightButton, screenX, screenY);

	// if leftButton has been released
	// set all visual layers to default colors
	// for the clicked visual layer, check if it should be
	// connected to another visual layer
	if (!leftButton)
	{
		// connect the visual layers if appropriate
		if (clickedVisLayer != nullptr)
		{
			//bool hasNearby;
			myVisualLayersManager.isNearVicinity(clickedVisLayer, "connect");

			// snap if already connected
			myVisualLayersManager.snapLayer(clickedVisLayer);
		}

		myVisualLayersManager.setAlltoDefaultColor();
	}
}

bool Manager::checkModelReqs(std::vector<Layer> finalModel)
{
	if (finalModel.size() == 0) 
	{
		noModelFoundError = true;
		return false;
	}
	else
	{
		if (finalModel.back().outputSize != 10 || finalModel.back().pool == true)
		{
			lastLayerSizeError = true;
			return false;
		}
	}

	return true;
}

void Manager::enableHelpButtons() {

	menuButtons.enableButton(FSKEY_I);
	menuButtons.enableButton(FSKEY_O);
	menuButtons.enableButton(FSKEY_V);
	menuButtons.enableButton(FSKEY_X);
}
