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

	addMenuButtons(); 
	addLayerButtons();
	addLinLayerButtons();
	addConvLayerButtons();
	addRunButtons();
	addShowButtons();

	// models created and trainer for running training
	model = new Model;
	trainer = new Trainer;
}

bool Manager::manage()
{

	FsPollDevice();
	key = FsInkey();
	mouseEvent = FsGetMouseEvent(leftButton, middleButton,
		rightButton, screenX, screenY);

	
	// defining window
	int wid, hei;
	FsGetWindowSize(wid, hei);
	glViewport(0, 0, wid, hei);

    glClearColor(0.53f, 0.81f, 0.92f, 1.0f); // RGBA, A is for alpha (opacity)


	// draw all the stuff
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


	// reset error messages when creating layer on click
	if (mouseEvent == FSMOUSEEVENT_LBUTTONDOWN)
	{
		poolError = false;
		lastLayerSizeError = false;
		noModelFoundError = false;
		deleteLayerMode = false;

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
			buttonKey = RunButtons.checkClick(screenX, screenY);
			buttonKeytest = menuButtons.checkClick(screenX, screenY);
			if (buttonKey != FSKEY_NULL)
				key = buttonKey;  // pretend the user pressed a key 
		}
		manageMenuScreen(wid, hei);
		manageRunScreen(wid, hei);
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
	// display linear options
	if (LinSelected) {
		manageLinLayerScreen();
	}
	// display conv options
	if (ConvSelected) {
		manageConvLayerScreen();
	}

	if (ConvSelected) {
		manageMenuScreen(wid, hei);
	}

	if (CancelSelected) {
		arial.setColorHSV(0, 1, 1, 1); // Red color
		arial.drawText("CANCELLED RUN ! ! !", 825, 430, 0.25, 0.1, true); // Adjust the position and size as needed
		trainer->changeContinueRun(false);
	}
	// display help screen and buttons
	if (HelpSelected) {

		manageHelpScreen(wid, hei);

	}
	if (HelpSelected == true) {

		disableHelpButtons();
	}
	if (HelpSelected == false) {
		enableHelpButtons();

	}
	// switches the current plot being displayed
	if (SwitchSelected == true && currentState == ShowPlot) {
		LossPlot();
	}
	else if (SwitchSelected == false && currentState == ShowPlot) {
		AccPlot();
	}
	
	// Cancels run when cancel button is selected
	if (CancelSelected && currentState == ShowPlot) {
		arial.setColorHSV(0, 1, 1, 1); // Red color
		arial.drawText("CANCELLED RUN ! ! !", 825, 430, 0.25, 0.1, true); // Adjust the position and size as needed
	}

	if (CancelSelected1 && currentState == MenuScreen) {
		arial.setColorHSV(0, 1, 1, 1); // Red color
		arial.drawText("CANCELLED RUN ! ! !", 825, 430, 0.25, 0.1, true); // Adjust the position and size as needed
	}

	if (StartSelected && currentState == OptScreen) {

		arial.setColorHSV(0, 1, 1, 1); // Red color
		arial.drawText("STARTED RUN ! ! !", 825, 450, 0.25, 0.1, true); // Adjust the position and size as needed
	}

	// Checking if pooling is allowed in next layer
	if (poolError)
	{
		arial.setColorHSV(0, 1, 1, 1); // Red color
		arial.drawText("Pooling only allowed if", 825, 425, 0.18, 0.1, true); // Adjust the position and size as needed
		arial.drawText("input height*width is odd.", 825, 425+(arial.getLetterHeight()+5)*0.18, 0.18, 0.1, true); // Adjust the position and size as needed

	}

	// Checks if input of first layer and output of last layer is correct
	if (lastLayerSizeError)
	{
		arial.setColorHSV(0, 1, 1, 1); // Red color
		arial.drawText("First layer input size has to be 1.", 835, 430-(arial.getLetterHeight() + 5) * 0.2, 0.18, 0.1, true); // Adjust the position and size as needed
		arial.drawText("Last layer output size has to be 10.", 835, 430, 0.18, 0.1, true); // Adjust the position and size as needed
		arial.drawText("Pooling also not allowed.", 835, 430 + (arial.getLetterHeight() + 5) * 0.2, 0.18, 0.1, true); // Adjust the position and size as needed

	}

	// Checks if model was even built
	if (noModelFoundError)
	{
		arial.setColorHSV(0, 1, 1, 1); // Red color
		arial.drawText("No model found.", 825, 430, 0.25, 0.1, true); // Adjust the position and size as needed
	}

	// Clears the built model
	if (key == FSKEY_D)
	{
		myVisualLayersManager.clearLayers();
	}

	return true;
}

// Draw the start screen
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

// Draw the menu screen
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

	
	 if (buttonKey== FSKEY_H) {
		//cout << currentState << endl;
		HelpSelected = !HelpSelected;
		//CancelSelected1 = false;
	 }

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
			CancelSelected = false;
			CancelSelected1 = false;

		}

	}
	else if (buttonKeytest == FSKEY_H) {
		//cout << HelpSelected << endl;
		HelpSelected = !HelpSelected;
		CancelSelected1 = false;
	}

	else if (key == FSKEY_X) {
		//cout << currentState << endl;
		CancelSelected1 = !CancelSelected1;
	}
	else if (buttonKeytest == FSKEY_D)
	{
		myVisualLayersManager.clearLayers();

	}

	// see if a visual layer was clicked
	seeIfVisualLayerWasClicked();
	
	drawVisualLayers();
}

// Manage the layer option screen
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

	if (key == FSKEY_D) // delete layer
	{
		// clear model
		myVisualLayersManager.clearLayers();
		// Layer creation area
	}
	else
	{
		drawVisualLayers();
	}

}

// Manage options screen after pressing run
void Manager::manageRunScreen(int wid, int hei) {

	// You can adjust these values based on your screen layout and button sizes
	int startX = 100; // X position where buttons start (adjust based on window size)
	int startY = 520; // Y position where buttons start
	int spacing = 50; // Space between op


	// Draw the DATASET subheading
	arial.setColorHSV(0.7, 1, 0, 1);// Choose a color for the text
	arial.drawText("Choose Optmizer:",startX,startY, 0.25, 0.1, true); // Adjust the position and size as needed
	
	startX += spacing + arial.getWordWidth("Choose Optmizer :", 0.25)+20;
	arial.drawText("Choose Epochs to Train:", startX, startY, 0.25, 0.1, true); // Adjust the position and size as needed
	startX += arial.getWordWidth("Choose Epochs to Train :", 0.25)+20;
	arial.drawText("Choose Learning Rate:", startX, startY, 0.25, 0.1, true); // Adjust the position and size as needed
	// Enable more buttons as needed
	// Draw the buttons
	RunButtons.paint();
	RunButtons.checkHover(screenX, screenY);

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



	if (buttonKey >= 10999 && buttonKey <= 11999)
	{
		ComboBox* optTypeComboBox = RunButtons.getControl<ComboBox*>(11000);

		int currVal = -1;
		optTypeComboBox->setIsExpanded(true);
		optTypeComboBox->paint();
		Optim currOptimizer = ADAM;

		leftButton = 0;
		while (!leftButton)
		{
			FsPollDevice();
			mouseEvent = FsGetMouseEvent(leftButton, middleButton,
				rightButton, screenX, screenY);

			if (leftButton == 1)
			{
				optTypeComboBox->getVal(screenX, screenY);
				currVal = optTypeComboBox->getCurrSelection();
				optTypeComboBox->setIsExpanded(false);
				optTypeComboBox->paint();

				break;
			}
		}

		if (currVal == 0)
		{
			finalOptim = ADAM;
		}
		else if (currVal == 1)
		{
			finalOptim = SGD;
		}
	}
	else if (buttonKey >= 12999 && buttonKey <= 13999)
	{
		ComboBox* epochComboBox = RunButtons.getControl<ComboBox*>(13000);

		int currVal = -1;
		epochComboBox->setIsExpanded(true);
		epochComboBox->paint();

		leftButton = 0;
		while (!leftButton)
		{
			FsPollDevice();
			mouseEvent = FsGetMouseEvent(leftButton, middleButton,
				rightButton, screenX, screenY);

			if (leftButton == 1)
			{
				epochComboBox->getVal(screenX, screenY);
				currVal = epochComboBox->getCurrSelection();
				epochComboBox->setIsExpanded(false);
				epochComboBox->paint();

				break;
			}
		}

		switch (currVal)
		{
		case 0:
			finalEpoch = 4;
			break;
		case 1:
			finalEpoch = 6;
			break;
		case 2: 
			finalEpoch = 8;
			break;
		case 3:
			finalEpoch = 10;
			break;
		}
	}
	else if (buttonKey >= 14999 && buttonKey <= 16999)
	{
		ComboBox* lrComboBox = RunButtons.getControl<ComboBox*>(15000);

		int currVal = -1;
		lrComboBox->setIsExpanded(true);
		lrComboBox->paint();

		leftButton = 0;
		while (!leftButton)
		{
			FsPollDevice();
			mouseEvent = FsGetMouseEvent(leftButton, middleButton,
				rightButton, screenX, screenY);

			if (leftButton == 1)
			{
				lrComboBox->getVal(screenX, screenY);
				currVal = lrComboBox->getCurrSelection();
				lrComboBox->setIsExpanded(false);
				lrComboBox->paint();

				break;
			}
		}

		
		switch (currVal)
		{
		case 0:
			finalLR = 0.0005;
			break;
		case 1:
			finalLR = 0.001;
			break;
		case 2:
			finalLR = 0.002;
			break;
		case 3:
			finalLR = 0.001;
			break;
		}
	}

	if (buttonKeytest == FSKEY_O) {
		currentState = MenuScreen;
	}

	if (key== FSKEY_G)  // to start training;
	{

		// get the values
		// finalOptim, finalEpoch, finalLR

		std::vector<Layer> finalModel = myVisualLayersManager.getFinalModel();
		
		if (checkModelReqs(finalModel)) // passes test
		{
			delete trainer;
			delete model;
			trainer = new Trainer;
			model = new Model;
			CancelSelected = false;

			std::cout << "NICE" << std::endl;

			std::vector<int> convOutSize;
			int size = 28;
			int depth;
			for (auto layer : finalModel) {
				if (layer.type == CONV2D) {
					size -= (layer.kernelSize - 1);
					depth = layer.outputSize;
					if (layer.pool) {
						size *= 0.5;
					}
				}
				if (layer.type == LINEAR) {
					break;
				}
			}

			convOutSize = { depth, int(size), int(size) };

			model->newModel(finalModel, convOutSize);
			trainer->newTrainer(finalLR, finalOptim, finalEpoch, 64, false);

			std::thread* train = new std::thread(Trainer::ThreadEntry1, trainer, model);
			}
		}
		else
		{
			std::cout << "UH OH" << std::endl;
		}

		
	
}

// draw the linear layer options screen
void Manager::manageLinLayerScreen() {

	// Draw the buttons
	LinearlayerButtons.paint();
	LinearlayerButtons.checkHover(screenX, screenY);

	if (leftButton){
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

							currComboBox->setIsExpanded(false);
							break;
						}
					}
					currComboBox->setIsExpanded(false);
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

// draw the convolutional layer option screen
void Manager::manageConvLayerScreen() {

	// Draw the buttons
	ConvlayerButtons.paint();
	ConvlayerButtons.checkHover(screenX, screenY);


	if (leftButton) {//key == FSKEY_NULL && mouseEvent == FSMOUSEEVENT_LBUTTONDOWN) {
		buttonKey = ConvlayerButtons.checkClick(screenX, screenY);

		// check if selected, if so change the edit bar values
		selectedVislayer = myVisualLayersManager.checkSelected();
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
			ConvlayerButtons.getControl<ComboBox*>(6000)->setCurrSelection(currPool);


			int currKernel = selectedVislayer->getLayer().kernelSize;
			int currKernelVal = -1;
			switch (currKernel)
			{
			case 3:
				currKernelVal = 0;
				break;
			case 5:
				currKernelVal = 1;
				break;
			case 7:
				currKernelVal = 2;
				break;
			}
			ConvlayerButtons.getControl<ComboBox*>(9000)->setCurrSelection(currKernelVal);


			std::string newText;
			if (buttonKey >= 1000)
			{

				if (buttonKey >= 5999 && buttonKey <= 6998 && selectedVislayer->getFixedPool())
				{
					poolError = true;
				}
				// deal with combo boxes
				else if ((buttonKey >= 3000 && buttonKey <= 3999) 
					|| (buttonKey >= 5999 && buttonKey <= 6998 && !selectedVislayer->getFixedPool())
					|| (buttonKey >= 8999 && buttonKey <= 9998))
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
					else if (buttonKey >= 8999 && buttonKey <= 9998)
					{
						currComboBox = ConvlayerButtons.getControl<ComboBox*>(9000);
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
								selectedVislayer->setPool(currComboBox->getCurrSelection()); // yes == 0, no == 1;
							}
							else if (buttonKey >= 8999 && buttonKey <= 9998)
							{
								selectedVislayer->setKernelSizeFromComboBox((currComboBox->getCurrSelection()));
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
			ConvlayerButtons.getControl<ComboBox*>(9000)->setCurrSelection(-1);

		}
		if (buttonKey != FSKEY_NULL)
			key = buttonKey;  // pretend the user pressed a key 
	}

}

// draw screen that shows training plots
void Manager::manageShowScreen(int wid, int hei) {
	// You can adjust these values based on your screen layout and button sizes
	int startX = 840; // X position where buttons start (adjust based on window size)
	int startY = 50;// Y position where buttons start
	int spacing = 40; // Space between buttons

	arial.setColorHSV(0, 1, 1, 1); // Red color
	arial.drawText("P R O P E R T I E S", startX, startY, 0.3, 0.1, true); // Adjust the position and size as needed
	startY += spacing;
	int epoch = trainer->getCurrEpoch(); 
	string epochstring = "current epoch: " + std::to_string(epoch);
	arial.setColorHSV(0.7, 1, 0, 1);// Choose a color for the text
	arial.drawText(epochstring, startX, startY, 0.25, 0.1, true); // Adjust the position and size as needed
	startY += spacing;
	float learningRate = trainer->getCurrLR();
	string lrstring = "current LR: " + std::to_string(learningRate);
	arial.drawText(lrstring, startX, startY, 0.25, 0.1, true); // Adjust the position and size as needed
	startY += spacing;
	
	float Accuracy = 10000*trainer->getCurrAcc();
	int acc = Accuracy / 100;
	string acstring = "current accuracy: " + std::to_string(acc) + "%";
	arial.drawText(acstring, startX, startY, 0.25, 0.1, true); // Adjust the position and size as needed
	startY += spacing;

	float Loss = trainer->getCurrLoss();
	int loss = Loss * 100;
	string lossString = "current loss: " + std::to_string(loss/100);
	arial.drawText(lossString, startX, startY, 0.25, 0.1, true); // Adjust the position and size as needed

	ShowButtons.paint();
	ShowButtons.checkHover(screenX, screenY);
	int rectStartY = 50;

	// Width and height of the rectangle
	int rectWidth = 30; // full width of the window
	int rectHeight = 600;

	// Set the outline color to blue
	glColor3f(1.0f, 1.0f, 1.0f); // Blue color

	 //Draw the rectangle outline
	glLineWidth(2.0f); // Set the line width for the outline
	glBegin(GL_QUADS);
	glVertex2f(700, 50);                     
	glVertex2f(700, 650); //        
	glVertex2f(30, 650 );// 
	glVertex2f(30, 50);   //           
	glEnd();

	if (buttonKey == FSKEY_R) {

		if (currentState != MenuScreen) {
			currentState = MenuScreen;
		}

		if (ReturnSelected = true) {
			ReturnSelected = !ReturnSelected;
		}

		CancelSelected = false;
	}

	else if (key == FSKEY_S) {
		SwitchSelected = !SwitchSelected;
	}

	else if (key == FSKEY_X) {
		CancelSelected = ! CancelSelected;
		StartSelected = true;

	}

}

// draw help menu screen
void Manager::manageHelpScreen(int wid, int hei) {

	int startX = 820; // X position where buttons start (adjust based on window size)
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

	
	//textX 
	arial.setColorHSV(0, 1, 1, 1); // Red color
	arial.drawText("Help Guide", (10 + 720) / 2, 50, 0.5, 0.1, true); // Adjust the position and size as needed


	ShowHelpMenu();
}

// adds home buttons
void Manager::addHomeButtons()
{

	int spacing = 100;
	int currY = 500;
	int currX = (910 / 2) - (buttonWidth/4) -40;
	homeButtons.add(currX, currY, buttonWidth, buttonHeight, FSKEY_L, "Start", &buttonFont,"Start the Application");

}

// adds show buttons
void Manager::addShowButtons()
{

	
	int startX = 780; // X position where buttons start (adjust based on window size)
	int startY = 200; // Y position where buttons start
	int spacing = 20; // Space between buttons
    
	startY += buttonHeight + spacing;
	ShowButtons.add(startX, startY, buttonWidth, buttonHeight, FSKEY_X, "Cancel", &buttonFont, "Stop Training");
	startY += buttonHeight + spacing;
	ShowButtons.add(startX, startY, buttonWidth, buttonHeight, FSKEY_S, "Switch plot", &buttonFont, "Switch");
	startY += buttonHeight + spacing;
	ShowButtons.add(startX, startY, buttonWidth, buttonHeight, FSKEY_R, "Return", &buttonFont, "Return");


}

// adds menu buttons
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
	startY += buttonHeight + spacing;
	menuButtons.add(startX - 70, startY, buttonWidth, buttonHeight, FSKEY_D, "CLEAR", &buttonFont, "Clear Model");
}

// adds layer buttons
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

// adds linear layer buttons
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

// adds convolutional layer buttons
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
	ComboBox* kernelComboBox = new ComboBox(startX, startY, buttonWidth, buttonHeight, 9000, "Kernel Size", &buttonFont, "Select Kernel Size");
	kernelComboBox->addItem("3");
	kernelComboBox->addItem("5");
	kernelComboBox->addItem("7");
	ConvlayerButtons.add(kernelComboBox);

	startX += buttonWidth + spacing;
	ComboBox* poolComboBox = new ComboBox(startX, startY, buttonWidth, buttonHeight, 6000, "Max Pooling", &buttonFont, "Only Allowed if Input Size is Even");
	poolComboBox->addItem("No Pooling");
	poolComboBox->addItem("Yes Pooling");
	ConvlayerButtons.add(poolComboBox);

}

// adds run buttons
void Manager::addRunButtons()
{
	int startX = 30; // X position where buttons start (adjust based on window size)
	int startY = 540; // Y position where buttons start
	int spacing = 50; // Space between buttons


	ComboBox* OptType = new ComboBox(startX, startY, buttonWidth, buttonHeight, 11000, "Type", &buttonFont, "Select Type of Optimizer ");
	OptType->addItem("Adam");
	OptType->addItem("SGD");
	RunButtons.add(OptType);

	startX += buttonWidth + spacing + 10;
	ComboBox* EpochNumber = new ComboBox(startX, startY, buttonWidth, buttonHeight, 13000, "Type", &buttonFont, "Select # of epochs ");
	EpochNumber->addItem("4");
	EpochNumber->addItem("6");
	EpochNumber->addItem("8");
	EpochNumber->addItem("10");
	RunButtons.add(EpochNumber);

	startX += buttonWidth + spacing + 60;
	ComboBox* LR = new ComboBox(startX, startY, buttonWidth, buttonHeight, 15000, "Type", &buttonFont, "Select Starting LR ");
	LR->addItem("0.0005");
	LR->addItem("0.001");
	LR->addItem("0.002");
	LR->addItem("0.01");
	RunButtons.add(LR);

	startX += buttonWidth + spacing + 40;
	RunButtons.add(startX , startY, buttonWidth, buttonHeight, FSKEY_G, "START", &buttonFont, "Start Training");

}

// disables help screen buttons
void Manager::disableHelpButtons() {

	menuButtons.disableButton(FSKEY_I);
	menuButtons.disableButton(FSKEY_O);
	menuButtons.disableButton(FSKEY_V);
	menuButtons.disableButton(FSKEY_X);
}

// draw visualizer
void Manager::drawVisualLayers()
{
	myVisualLayersManager.drawVisualLayers();
}

// checks mouse location in visualizer
void Manager::seeIfVisualLayerWasClicked()
{

	mouseEvent = FsGetMouseEvent(leftButton, middleButton,
		rightButton, screenX, screenY);

	if (deleteLayerMode && false)
	{
		addDeleteLayerButton();

	
		if (buttonKey == FSKEY_D)
		{
			deleteLastLayerOnly();
		}
	}

	// check if want to drag a block
	if (leftButton) {
		// check if a visual layer has been clicked
		clickedVisLayer = myVisualLayersManager.checkClick(screenX, screenY);

		// if it has been clicked move it to new center
		if (clickedVisLayer != nullptr)
		{
			myVisualLayersManager.changetoSelectMode(clickedVisLayer);
			myVisualLayersManager.moveVisualLayers(clickedVisLayer, screenX, screenY);

			// set to delete mode;
			deleteLayerMode = true;
			//addDeleteLayerButton();

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

// checks if model is appropriate
bool Manager::checkModelReqs(std::vector<Layer> finalModel)
{ 
	//return true;
	if (finalModel.size() == 0) 
	{
		noModelFoundError = true;
		return false;
	}
	else
	{
		if (finalModel.back().outputSize != 10 || finalModel.front().inputSize != 1)
		{
			lastLayerSizeError = true;
			return false;
		}
	}

	return true;
}

// Adds a delete layer functionality (in development)
void Manager::addDeleteLayerButton()
{
	{
		int startX = 550; // X position where buttons start (adjust based on window size)
		int startY = 425; // Y position where buttons start

		//deleteButton.add(startX, startY, buttonWidth, buttonHeight, FSKEY_D, "Delete", &buttonFont, "Delete Layer");
		deleteButton.paint();
	}

}

void Manager::deleteLastLayerOnly()
{
	selectedVislayer = myVisualLayersManager.checkSelected();

	if (selectedVislayer != nullptr && selectedVislayer->getRightConnect() == nullptr)
	{
		myVisualLayersManager.deleteVisLayer(selectedVislayer);
	}
}

// re-enables the help screen buttons
void Manager::enableHelpButtons() {

	menuButtons.enableButton(FSKEY_I);
	menuButtons.enableButton(FSKEY_O);
	menuButtons.enableButton(FSKEY_V);
	menuButtons.enableButton(FSKEY_X);
	menuButtons.enableButton(FSKEY_H);
}

// plot accuracy graphs
void Manager::AccPlot() {

	glColor3f(1.0f, 1.0f, 1.0f); // black color

	// Draw the rectangle outline
	std::vector<float> accuracies = trainer->getAcc();
	float epochs = trainer->getEpochs();
	int graphWid = 400;

	glTranslated(0, 750, 0);
	glScaled(1, -1, 1);
	glTranslated(150, 250, 0);
	glLineWidth(2.0);

	glColor3f(0, 0, 0);

	glBegin(GL_LINE_STRIP);
	glVertex2f(graphWid, 0);
	glVertex2f(0, 0);
	glVertex2f(0, graphWid);
	glEnd();

	for (int i = 0; i < epochs; i++) {
		glBegin(GL_LINES);
		glVertex2f(i * (graphWid / (epochs - 1)), 0);
		glVertex2f(i * (graphWid / (epochs - 1)), 10);
		glEnd();
		arial.setColorRGB(0, 0, 0);
		arial.drawText(std::to_string(i + 1), i * (graphWid / (epochs - 1)), -40,
			-0.2, 180, true);


		glBegin(GL_LINES);
		glVertex2f(0, (i) * (graphWid - 20) / (epochs - 1));
		glVertex2f(10, (i) * (graphWid - 20) / (epochs - 1));
		glEnd();

	}

	if (!accuracies.empty()) {
		auto min = min_element(std::begin(accuracies), std::end(accuracies));

		for (int i = 0; i < accuracies.size(); i++) {
			float centerX = i * (graphWid / (epochs - 1));
			float centerY = (graphWid) * (accuracies[i]); // * (graphWid - 20);
			glColor3f(0.741, 0, 0);
			DrawingUtilNG::drawCircle(centerX, centerY, 10, 10, true);
			if (i > 0) {
				float PrevcenterX = (i - 1) * (graphWid / (epochs - 1));
				float PrevcenterY = (graphWid) * (accuracies[i-1]);
				glColor3f(0.941, 0.376, 0.376);
				glBegin(GL_LINES);
				glVertex2f(centerX, centerY);
				glVertex2f(PrevcenterX, PrevcenterY);
				glEnd();
			}
			
		}
	}
	arial.drawText("Epoch", int(graphWid / 2), -90,
		-0.3, 180, true);

	arial.drawText("Accuracy", -25, 200,
		-0.3, 90, true);

	glLoadIdentity();


}

// plot loss graphs
void Manager::LossPlot() {

	glColor3f(0.0f, 0.0f, 0.0f); // black color

	std::vector<float> losses = trainer->getLosses();
	
	float epochs = trainer->getEpochs();

	int graphWid = 400;

	glTranslated(0, 750, 0);
	glScaled(1, -1, 1);
	glTranslated(150, 250, 0);
	glLineWidth(2.0);

	glColor3f(0, 0, 0);

	glBegin(GL_LINE_STRIP);
	glVertex2f(graphWid, 0);
	glVertex2f(0, 0);
	glVertex2f(0, graphWid);
	glEnd();

	for (int i = 0;i<epochs ; i++) {
		glBegin(GL_LINES);
		glVertex2f(i* (graphWid/(epochs-1)),0);
		glVertex2f(i * (graphWid / (epochs - 1)), 10);
		glEnd();
		arial.setColorRGB(0, 0, 0);
		arial.drawText(std::to_string(i+1), i * (graphWid / (epochs-1)), -40,
		-0.2,  180, true);

	
		glBegin(GL_LINES);
		glVertex2f(0, (i)* (graphWid - 20) / (epochs-1));
		glVertex2f(10, (i)* (graphWid - 20) / (epochs-1));
		glEnd();

	}

	if (!losses.empty()) {
		auto max = max_element(std::begin(losses), std::end(losses));
			
		for (int i = 0; i < losses.size(); i++) {
			float centerX = i * (graphWid / (epochs - 1));
			float centerY = losses[i]/(*max) * (graphWid-20);
			
			if (i > 0) {
				float PrevcenterX = (i-1) * (graphWid / (epochs - 1));
				float PrevcenterY = losses[i-1] / (*max) * (graphWid - 20);
				glColor3f(0.941, 0.376, 0.376);
				glBegin(GL_LINES);
				glVertex2f(centerX, centerY);
				glVertex2f(PrevcenterX, PrevcenterY);
				glEnd();
			}
			glColor3f(0.741, 0, 0);
			DrawingUtilNG::drawCircle(centerX, centerY, 10, 10, true);
		}
	}
	arial.drawText("Epoch", int(graphWid / 2), -90,
		-0.3, 180, true);

	arial.drawText("Loss", -25, 200,
		-0.3, 90, true);

	glLoadIdentity();
}

// shows the help menu text
void Manager::ShowHelpMenu() {

	int textX = 15;
	int textY = 120;
	INT linspace = 25;

	arial.setColorHSV(0, 1, 1, 1); // Red color

	arial.drawText(" 1. List of Menu buttons on the right side provides user with different options to select", textX, textY, 0.2, 0.0, false); // Adjust the position and size as needed
	textY += linspace;
	arial.drawText(" 2. To begin Select LAYER -> Linear/Conv and then fill out  all associated properties shown in Selection box", textX, textY, 0.2, 0.0, false);
	textY += linspace;
	arial.drawText("at the bottom of the screen", textX, textY, 0.2, 0.0, false);
	textY += linspace;

	arial.drawText(" 3. The white screen displays the Model Architecture Block which user can Drag and Drop Conv or Linear ", textX, textY, 0.2, 0.0, false);
	textY += linspace;
	arial.drawText("layer blocks on the screen to connect with the previous layer", textX, textY, 0.2, 0.0, false);
	textY += linspace;
	arial.drawText(" 4. After adding the required layers the user has to RUN training using RUN button and select the  appropriate", textX, textY, 0.2, 0.0, false);
	textY += linspace;
	arial.drawText(" training properties", textX, textY, 0.2, 0.0, false);
	textY += linspace;
	arial.drawText(" 5. After hitting START button to start the training, the user can click on PLOT to observe the Accuracy and  ", textX, textY, 0.2, 0.0, false);
	textY += linspace;
	arial.drawText(" Loss graphs for current training", textX, textY, 0.2, 0.0, false);
	textY += linspace;
	arial.drawText(" 6. Note that when you enter the PLOT screen you can use the Switch Plot button to switch between the ", textX, textY, 0.2, 0.0, false);
	textY += linspace;
	arial.drawText(" Loss Graph or Accuracy Graph", textX, textY, 0.2, 0.0, false);
	textY += linspace;
	arial.drawText(" CAUTION : ", textX, textY + 15, 0.25, 0.0, false);
	textY += linspace + 15;
	arial.drawText(" Rememeber  to Hit CANCEL/Cancel button only when u plan on cancelling or stopping the training", textX, textY, 0.2, 0.0, false);
}