#include "VisualLayerManager.h"
#include "fssimplewindow.h"

// need this for visualLayer check click 
const int layerAreaMinX = 10;
const int layerAreaMaxX = 810;
const int layerAreaMinY = 10;
const int layerAreaMaxY = 475;


VisualLayer::VisualLayer(int locX, int locY, std::string layerType)
{
	id = nextId++;
	centerX = locX;
	centerY = locY;
	if (layerType == "linear")
	{
		modelLayer.type = LINEAR;
	}
	else if (layerType == "conv")
	{
		modelLayer.type = CONV2D;
	}

	modelLayer.inputSize = 3;
	modelLayer.outputSize = 3;
	modelLayer.activation = RELU;
	modelLayer.dropOut = 1.;
	modelLayer.kernelSize = 2;
	modelLayer.pool = true;

	checkPool();
}


int VisualLayer::getActivationVal(actType currActType)
{
	switch (currActType)
	{
	case RELU:
		return 0;
	case SIGMOID:
		return 1;
	case TANH:
		return 2;
	case NONE:
		return 3;
	}
}

bool VisualLayer::operator==(VisualLayer* otherVisLayer)
{
	if ((centerX == otherVisLayer->getCenterX()) && (centerY == otherVisLayer->getCenterY())) 
	{
		return true;
	}

	return false;
}

void VisualLayer::draw()
{

	// draw the visual layer
	glColor3ub(color.r, color.g, color.b);
	glBegin(GL_QUADS);
	glVertex2f(centerX - xSize / 2, centerY - ySize / 2); // bottom left
	glVertex2f(centerX - xSize / 2, centerY + ySize / 2); // top left
	glVertex2f(centerX + xSize / 2, centerY + ySize / 2); // top right
	glVertex2f(centerX + xSize / 2, centerY - ySize / 2); // buttom right
	glEnd();


	// draw the specs of the layer in the visual layer

	// layer type, input size, dropout, activation function
	std::string layerTypeString;
	std::string inSizeString;
	std::string outSizeString;

	if (modelLayer.type == LINEAR)
	{
		layerTypeString = "Linear";
		inSizeString = "In: " + std::to_string(modelLayer.inputSize);
		outSizeString = "Out: " + std::to_string(modelLayer.outputSize);
	}
	else if (modelLayer.type == CONV2D)
	{
		layerTypeString = "Conv2D";
		inSizeString = "In: " + std::to_string(modelLayer.inputSize) + "x" + std::to_string(modelLayer.inputSize);
		outSizeString = "Out: " + std::to_string(modelLayer.outputSize) + "x" + std::to_string(modelLayer.outputSize);

	}

	std::string actFuncString;
	switch (modelLayer.activation)
	{
	case RELU:
		actFuncString = "ReLU";
		break;
	case SIGMOID:
		actFuncString = "Sigmoid";
		break;
	case TANH:
		actFuncString = "Tanh";
		break;
	case NONE:
		actFuncString = "None";
		break;

	}
	

	glColor3ub(textColor.r, textColor.g, textColor.b);
	arial.setColorRGB(255., 255., 255.);
	// layer type
	arial.drawText(layerTypeString, centerX, centerY - (ySize/2)*0.75, 0.15, 0.0, true);// Adjust position and size as needed
	//glRasterPos2f(centerX - (xSize / 2) * 0.45, centerY + (ySize / 2) * 0.5); // sets position
	//YsGlDrawFontBitmap8x12(layerType.c_str());
	// input size
	arial.drawText(inSizeString, centerX, centerY - (ySize / 2) * 0.25, 0.15, 0.0, true);// Adjust position and size as needed
	// output size
	arial.drawText(outSizeString, centerX, centerY + (ySize / 2) * 0.25, 0.15, 0.0, true);// Adjust position and size as needed

	//glRasterPos2f(centerX - (xSize / 2) * 0.65, centerY - (ySize / 2) * 0.35); // sets position
	//YsGlDrawFontBitmap8x12(outSizeString.c_str());
	// activation function
	arial.drawText(actFuncString, centerX, centerY + (ySize / 2) * 0.75, 0.13, 0.0, true);// Adjust position and size as needed

//glRasterPos2f(centerX - (xSize / 2) * 0.45, centerY - (ySize / 2) * 0.8); // sets position
//(actFuncString.c_str());


// if it has a rightConnect, draw the connecting line
if (rightConnect != nullptr)
{
	// define vertex points
	float startX = centerX + xSize / 2;
	float startY = centerY;
	float endX = rightConnect->getCenterX() - (rightConnect->getXSize() / 2);
	float endY = rightConnect->getCenterY();

	// draw line
	glColor3ub(0, 0, 255);
	glBegin(GL_LINES);
	glVertex2f(startX, startY);
	glVertex2f(endX, endY);
	glEnd();
	glColor3ub(0, 0, 255);

	std::string outSizeOnLineString;
	// draw the output size
	if (modelLayer.type == LINEAR)
	{
		outSizeOnLineString = std::to_string(modelLayer.outputSize) + "x" + std::to_string(1);
	}
	else if (modelLayer.type == CONV2D)
	{
		outSizeOnLineString = std::to_string(modelLayer.outputSize) + "x" + std::to_string(modelLayer.inputSize);
	}

	arial.setColorRGB(0., 0., 255.);
	arial.drawText(outSizeOnLineString, startX + (endX - startX) * 0.5, endY - (ySize / 2) * 0.2, 0.13, 0.0, true);// Adjust position and size as need
}
}

bool VisualLayer::isClicked(int cursorX, int cursorY)
{
	// calculate edges of the button
	float xMin = centerX - (xSize / 2);
	float xMax = centerX + (xSize / 2);
	float yMin = centerY - (ySize / 2);
	float yMax = centerY + (ySize / 2);

	// if cursorX and cursorY are respectively within the x and y bounds, return true
	if ((cursorX >= xMin && cursorX <= xMax) && (cursorY >= yMin && cursorY <= yMax))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void VisualLayer::move(int newCenterX, int newCenterY)
{
	centerX = newCenterX;
	centerY = newCenterY;
}

void VisualLayer::moveLeftConnect(int newCenterX, int newCenterY)
{
	if (leftConnect == nullptr)
	{
		return;
	}
	else if (leftConnect != nullptr)
	{
		leftConnect->setCenterX(newCenterX);
		leftConnect->setCenterY(newCenterY);

		moveLeftConnect(newCenterX - xDistance, newCenterY);
	}
}


void VisualLayer::moveRightConnect(int newCenterX, int newCenterY)
{

	if (rightConnect == nullptr)
	{
		return;
	}
	else
	{
		rightConnect->setCenterX(newCenterX);
		rightConnect->setCenterY(newCenterY);


		moveRightConnect(newCenterX + xDistance, newCenterY);
	}

}

void VisualLayer::checkPool()
{
	if ((modelLayer.inputSize * modelLayer.inputSize) % 2 == 1) // odd
	{
		modelLayer.pool = false;
		fixedPool = true;
	}
	else // even
	{
		fixedPool = false;
	}
}

void VisualLayer::setActivation(int actVal)
{


switch (actVal)
{
case 0:
	modelLayer.activation = RELU;
	break;
case 1:
	modelLayer.activation = SIGMOID;
	break;
case 2:
	modelLayer.activation = TANH;
	break;
case 3:
	modelLayer.activation = NONE;
	break;
}
}



//void VisualLayer::move(int newCenterX, int newCenterY, std::string moveDirection, VisualLayer* originalVisLayer)
//{	
//	centerX = newCenterX;
//	centerY = newCenterY;
//
//	moveLeftConnect(centerX - xDistance, centerY);
//	moveRightConnect(centerX + xDistance, centerY);
//
//
//	/*
//	// both cases
//	if (leftConnect != nullptr && rightConnect != nullptr)
//	{
//		leftConnect->move(centerX - xDistance, centerY, "left", this);
//		this->getRightConnect()->move(centerX + xDistance, centerY, "right");
//	}
//	else if (leftConnect != nullptr)
//	{
//		leftConnect->move(centerX - xDistance, centerY, "left", this);
//	}
//	else if (rightConnect != nullptr)
//	{
//		this->getRightConnect()->move(centerX + xDistance, centerY, "right");
//	}
//	*/
//
//
//
//
//
//
//	/*
//	if (moveDirection == "left")
//	{
//		std::cout << "L" << std::endl;
//		if (leftConnect == nullptr)
//		{
//			std::cout << "LEFT" << std::endl;
//			return;
//		}
//		else
//		{
//			leftConnect->move(centerX - xDistance, centerY, "left", this);
//		}
//
//	}
//	else if (moveDirection == "right")
//	{
//		std::cout << "R" << std::endl;
//
//		if (this->getRightConnect() == nullptr)
//		{
//			std::cout << "RIGHT" << std::endl;
//			return;
//		}
//		else
//		{
//			this->getRightConnect()->move(centerX + xDistance, centerY, "right");
//		}
//	}
//
//
//	*/
//
//
//
//
//
//	/*
//	centerX = newCenterX;
//	centerY = newCenterY;
//
//
//	if (rightConnect != nullptr)
//	{
//		rightConnect->move(centerX + xDistance, centerY, "right", this);
//	}
//
//	if (this->getLeftConnect() != nullptr)
//	{
//		this->getLeftConnect()->move(centerX - xDistance, centerY, "left");
//	}
//
//	//if (rightConnect != nullptr)
//	//{
//	//	rightConnect->move(centerX + xDistance, centerY); // recurisve? centerX gets keeps getting updated?
//	//}
//	*/
//}


// ======================================================================
// VisualLayerManager functions

void VisualLayerManager::add(int locX, int locY, std::string layerType)
{
	// check if there is a vis layer there
	bool keepChecking = true;
	Point2I resultLocs = checkVisLayerLocs(locX, locY);
	//std::cout << "R" << resultLocs.x << "and" << resultLocs.y << std::endl;

	while (keepChecking)
	{
		if (resultLocs.x == -1 && resultLocs.y == -1)
		{
			locX += 50;
			locY += 50;
			resultLocs = checkVisLayerLocs(locX, locY);
			continue;
		}
		else
		{		
		
			keepChecking = false;
			break;
		}
	}

	// update locX and locY
	locX = resultLocs.x;
	locY = resultLocs.y;
	VisualLayer* newVisLayer = new VisualLayer(locX, locY, layerType);
	allVisualLayers.push_back(newVisLayer);

}

VisualLayer* VisualLayerManager::operator[](int desiredIdx)
{
	if (desiredIdx < allVisualLayers.size())
	{
		return allVisualLayers[desiredIdx];
	}
	return nullptr;
}

void VisualLayerManager::drawVisualLayers()
{
	for (auto& currVisLayer : allVisualLayers)
	{
		currVisLayer->draw();
	}
}

void VisualLayerManager::moveVisualLayers(VisualLayer* currVisLayer, int newCenterX, int newCenterY)
{
	currVisLayer->setCenterX(newCenterX);
	currVisLayer->setCenterY(newCenterY);

	bool keepMovingLefts = true;
	bool keepMovingRights = true;

	VisualLayer* neighborLayer = currVisLayer->getLeftConnect();

	while (keepMovingLefts)
	{
		if (neighborLayer == nullptr)
		{
			keepMovingLefts = false;
			break;
		}
		else
		{
			newCenterX -= xDistance;
			neighborLayer->move(newCenterX, newCenterY);
			neighborLayer = neighborLayer->getLeftConnect();
		}
	}

	newCenterX = currVisLayer->getCenterX();
	neighborLayer = currVisLayer->getRightConnect();
	while (keepMovingRights)
	{
		if (neighborLayer == nullptr)
		{
			keepMovingRights = false;
			break;
		}
		else
		{
			newCenterX += xDistance;
			neighborLayer->move(newCenterX, newCenterY);
			neighborLayer = neighborLayer->getRightConnect();
		}
	}


}

VisualLayer* VisualLayerManager::checkClick(int cursorX, int cursorY)
{

	// if cursor is not in white space (layer area), don't consider it
	if (cursorX < layerAreaMinX || cursorX > layerAreaMaxX ||
		cursorY < layerAreaMinY || cursorY > layerAreaMaxY)
	{
		return nullptr;
	}

	for (auto& currVisLayer : allVisualLayers)
	{
		if (currVisLayer->isClicked(cursorX, cursorY))
		{
			return currVisLayer;
		}
		else
		{
			changeToUnselectMode(currVisLayer);
		}
	}
	return nullptr;
}

void VisualLayerManager::connectVisualLayers(VisualLayer* leftVisLayer, VisualLayer* rightVisLayer)
{
	// connect visual layers
	leftVisLayer->setRightConnect(rightVisLayer);
	rightVisLayer->setLeftConnect(leftVisLayer);

	// "snap" rightVisLayer to be middle-aligned with leftVisLayer (set centerY to be the same)
	// define certain constant X distance between connected visual layers
	//int xDistance = 12; // assume xSize and ySize are 10
	rightVisLayer->setCenterY(leftVisLayer->getCenterY());
	rightVisLayer->setCenterX(leftVisLayer->getCenterX() + xDistance);

	// rightVisLayer's input must equal leftVisLayer's output
	// set rightVisLayer to be in fixedInput mode
	rightVisLayer->setInputSize(leftVisLayer->getLayer().inputSize);
	rightVisLayer->setFixedInput(true);
}

void VisualLayerManager::isNearVicinity(VisualLayer* currVisLayer, std::string connectOrColor)
{
	if (currVisLayer == nullptr) { return; };

	float otherXMin, otherXMax, otherYMin, otherYMax;
	int currX = currVisLayer->getCenterX();
	int currY = currVisLayer->getCenterY();

	int sideRange = 12*10;
	bool madeChange = false;

	for (auto& otherVisLayer : allVisualLayers)
	{
		// move on if self
		if (currVisLayer == otherVisLayer)
		{
			continue;
		}

		// define x,y min and max vicinity bounds with side range
		otherXMin = otherVisLayer->getCenterX() - (otherVisLayer->getXSize() / 2 + sideRange);
		otherXMax = otherVisLayer->getCenterX() + (otherVisLayer->getXSize() / 2 + sideRange);
		otherYMin = otherVisLayer->getCenterY() - (otherVisLayer->getYSize() / 2 + sideRange);
		otherYMax = otherVisLayer->getCenterY() + (otherVisLayer->getYSize() / 2 + sideRange);

		// check if x and y center are within bounds
		if ((currX > otherVisLayer->getCenterX() && currX <= otherXMax && 
			currY > otherYMin && currY <= otherYMax &&
			otherVisLayer->getRightConnect() == nullptr))
		{

			if (connectOrColor == "connect")
			{
				// otherVisLayer is left of currVisLayer
				connectVisualLayers(otherVisLayer, currVisLayer);
				madeChange = true;
			}
			else if (connectOrColor == "color") // "color" is used when currVisLayer is near 
				// vicinity of otherVisLayer, so otherVisLayer will change color to signifiy this
			{
				otherVisLayer->setColor({ 255., 192., 0. });
				otherVisLayer->setTextColor({ 0., 0., 0. });
				madeChange = true;
			}
		}
		// if not near vicinity, change otherVisLayer's color back to default
		else 
		{
			otherVisLayer->setToDefaultColor(); // change both background and text color
		}
	}

	return;
}

void VisualLayerManager::nearVicinityColorChange()
{
}

void VisualLayerManager::setAlltoDefaultColor()
{
	for (auto& currVisLayer : allVisualLayers)
	{
		if (!currVisLayer->getSelected())
		{
			currVisLayer->setColor({ 0., 0., 255. });
		}
	}
}

void VisualLayerManager::snapLayer(VisualLayer* currVisLayer)
{
	if (currVisLayer->getLeftConnect() != nullptr)
	{
		VisualLayer* currLeftConnect = currVisLayer->getLeftConnect();
		currVisLayer->setCenterX(currLeftConnect->getCenterX() + xDistance);
		currVisLayer->setCenterY(currLeftConnect->getCenterY());
	}

}

std::vector<Layer> VisualLayerManager::getFinalModel()
{
	std::vector<Layer> finalModel;
	VisualLayer* visLayerToAdd = nullptr;

	if (allVisualLayers.size() == 0)
	{
		return finalModel;
	}


	// find left most vis layer
	for (auto& currVisLayer : allVisualLayers)
	{
		if (currVisLayer->getLeftConnect() == nullptr)
		{
			visLayerToAdd = currVisLayer;
			break;
		}
	}

	// add the first layer
	finalModel.push_back(visLayerToAdd->getLayer());

	// add all the layers in order
	bool keepAdding = true;
	while (keepAdding)
	{
		if (visLayerToAdd->getRightConnect() == nullptr)
		{
			keepAdding = false;
			break;
		}
		else
		{
			visLayerToAdd = visLayerToAdd->getRightConnect();
			finalModel.push_back(visLayerToAdd->getLayer());
		}

	}


	return finalModel;
}

Point2I VisualLayerManager::checkVisLayerLocs(int locX, int locY)
{
	for (auto& currVisLayer : allVisualLayers)
	{
		if (locX == currVisLayer->getCenterX() && locY == currVisLayer->getCenterY())
		{
			return { -1, -1 };
		}

	}
	return { locX, locY };

}

void VisualLayerManager::changetoSelectMode(VisualLayer* clickedVisLayer)
{
	clickedVisLayer->setSelected(true);
	clickedVisLayer->setColor({ 0., 255., 0. });
}

void VisualLayerManager::changeToUnselectMode(VisualLayer* clickedVisLayer)
{
	clickedVisLayer->setSelected(false);
	clickedVisLayer->setToDefaultColor();
}




//
//Point2D VisualLayerManager::getModelCoords(int screenX, int screenY, float originX, float originY, float scale) {
//	int wei, hei;
//	FsGetWindowSize(wei, hei); // puts window parameters into the two given variables
//	return { screenX / scale + originX, (hei - screenY) / scale + originY };
//}
//
//
//void VisualLayerManager::setOriginAndScale(float& originX, float& originY, float& scale)
//{
//
//	if (theVisualLayers.size() > 0) {
//		Point2D minBnd, maxBnd, currMin, currMax;
//
//		minBnd = theVisualLayers[0]->minBound();
//		maxBnd = theVisualLayers[0]->maxBound();
//		for (VisualLayer* currGear : theVisualLayers) {
//			currMin = currGear->minBound();
//			currMax = currGear->maxBound();
//			if (minBnd.x > currMin.x)
//				minBnd.x = currMin.x;
//			if (minBnd.y > currMin.y)
//				minBnd.y = currMin.y;
//			if (maxBnd.x < currMax.x)
//				maxBnd.x = currMax.x;
//			if (maxBnd.y < currMax.y)
//				maxBnd.y = currMax.y;
//		}
//		originX = minBnd.x; originY = minBnd.y;
//
//		int hei, wid;
//		FsGetWindowSize(wid, hei);
//
//		float scaleX, scaleY;
//		int extraBorder = 10; // allows for some space around all the gears
//		scaleX = (wid - extraBorder) / (maxBnd.x - minBnd.x);
//		scaleY = (hei - extraBorder) / (maxBnd.y - minBnd.y);
//		scale = std::min(scaleX, scaleY);
//		if (scale == scaleX) // adjust originY so as to center vertically
//			originY -= 0.5 * (hei / scale - (maxBnd.y - minBnd.y));
//		else // adjust originX so as to center horizontally
//			originX -= 0.5 * (wid / scale - (maxBnd.x - minBnd.x));
//
//		// adjust for half extraborder
//		originX -= extraBorder / 2. / scale;
//		originY -= extraBorder / 2. / scale;
//	}
//	else {
//		originX = originY = 0.;
//		scale = 1.0;
//	}
//}