#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "ysglfontdata.h"
#include "DrawingUtilNG.h"
#include "GraphicFont.h"

// for when two visual layers are connected
// this is the x-distance b/w centers
const double xDistance = 10*10;

// useful for managing coordinates
struct Point2D {
	float x, y;
};

struct Point2I {
	int x, y;
};

struct RGB {
	float r, g, b;
};


// actual layers
enum layerType {
	LINEAR,
	CONV2D
};

enum actType {
	RELU,
	SIGMOID,
	TANH,
	NONE
};

struct Layer {
	layerType type;
	int inputSize;
	int outputSize;
	int kernelSize; //Doesn't matter for linear
	actType activation;
	bool pool; //Doesn't matter for linear
	float dropOut;
};

class VisualLayer
{
protected:
	int centerX = -1, centerY = -1;
	float xSize = 60, ySize = 60;
	static int nextId;
	int id;
	bool selected = false;
	RGB color = { 0, 0, 255 }; // default is blue
	RGB textColor = { 255, 255, 255 }; // default is black, only used for text inside visual layer
	VisualLayer* leftConnect = nullptr;
	VisualLayer* rightConnect = nullptr;
	ArialFont arial;

	Layer modelLayer = {};
	bool fixedInput = false;
	bool fixedDropout = false;
	bool fixedPool = false;

	int heightFM = -1;

public:
	// create a visual layer
	VisualLayer() { id = nextId++; };
	VisualLayer(int locX, int locY, std::string layerType);


	// get attributes
	int getID() { return id; };
	int getCenterX() { return centerX; };
	int getCenterY() { return centerY; };
	float getXSize() { return xSize; };
	float getYSize() { return ySize; };
	VisualLayer* getLeftConnect() { return leftConnect; };
	VisualLayer* getRightConnect() { return rightConnect; };
	bool getSelected() { return selected; };
	Layer getLayer() { return modelLayer; };
	int getActivationVal(actType currActType);
	void setActivation(int actVal);
	void setKernelSizeFromComboBox(int currSelect);
	bool getFixedInput() { return fixedInput; };
	bool getFixedDropout() { return fixedDropout; };
	bool getFixedPool() { return fixedPool; };
	int getHeightFM() { return heightFM; }

	// see if two visual layers are equal
	bool operator==(VisualLayer* otherVisLayer);
	
	// draw the visual layer
	void draw();

	// check if visual layer has been clicked
	bool isClicked(int cursorX, int cursorY);

	// move a visual layer
	void move(int newCenterX, int newCenterY);
	//void move(int newCenterX, int newCenterY, std::string moveDirection = "none", VisualLayer* originalVisLayer = nullptr);
	void moveLeftConnect(int newCenterX, int newCenterY);
	void moveRightConnect(int newCenterX, int newCenterY);

	// set attributes
	void setCenterX(int newCenterX) { centerX = newCenterX; };
	void setCenterY(int newCenterY) { centerY = newCenterY; };
	void setLeftConnect(VisualLayer* otherVisLayer) { leftConnect = otherVisLayer; };
	void setRightConnect(VisualLayer* otherVisLayer) { rightConnect = otherVisLayer; };
	void setColor(RGB newColor) { color = newColor; };
	void setTextColor(RGB newColor) { textColor = newColor; };
	void setSelected(bool newSelectedMode) { selected = newSelectedMode; };
	void setToDefaultColor() { color = { 0., 0., 255. }; textColor = { 255., 255. , 255. }; };
	void setInputSize(int newInputSize) { modelLayer.inputSize = newInputSize; checkPool(); };
	void setOutputSize(int newOutputSize) { modelLayer.outputSize = newOutputSize; };
	void setDropout(double newDropout) { modelLayer.dropOut = newDropout; };
	void setKernelSize(int newKernelSize) { modelLayer.kernelSize = newKernelSize; };
	void setPool(int newPool) 
	{ 
		if (leftConnect != nullptr)
		{
			if (fmod(leftConnect->getHeightFM() - modelLayer.kernelSize + 1, 2) == 1)
			{
				return;
			}

		}
		else
		{
			modelLayer.pool = newPool;
		}
	};
	void setFixedInput(bool newFixedInputSetting) { fixedInput = newFixedInputSetting; };
	void setFixedDropout(bool newDropoutSetting) { fixedDropout = newDropoutSetting; };
	void checkPool();

	bool checkPoolBool();
};


class VisualLayerManager
{
protected:
	std::vector<VisualLayer*> allVisualLayers;
	//std::vector<VisualLayer*> modelLayers;

public:

	// add a visual layer
	void add(int locX, int locY, std::string layerType);

	// access functions
	VisualLayer* operator[](int desiredIdx);

	// move layers
	void moveVisualLayers(VisualLayer* currVisLayer, int newCenterX, int newCenterY);

	// draw all visual layers
	void drawVisualLayers();

	// check which layer has been clicked on
	VisualLayer* checkClick(int cursorX, int cursorY);

	// connect two visual layers
	void connectVisualLayers(VisualLayer* leftVisLayer, VisualLayer* rightVisLayer);

	// check if a visual layer is near the vicinity of the rest
	void isNearVicinity(VisualLayer* currVisLayer, std::string connectOrColor);

	void nearVicinityColorChange();

	// set all layers to default colors
	void setAlltoDefaultColor();

	// snap a vis layer to its leftConnect if it has one
	void snapLayer(VisualLayer* currVisLayer);

	// get all final layers for run
	std::vector<Layer> getFinalModel();

	// check if vis layer can be created at current spot
	Point2I checkVisLayerLocs(int locX, int locY);

	// change color when selected
	void changetoSelectMode(VisualLayer* clickedVisLayer);
	void changeToUnselectMode(VisualLayer* clickedVisLayer);
	// run isNearVicinity() for all visual layers

	// check selected
	VisualLayer* checkSelected()
	{
		for (auto& currVisLayer : allVisualLayers)
		{
			if (currVisLayer->getSelected())
			{
				return currVisLayer;
			}
		}

		return nullptr;
	}

	// delete vis layer
	void deleteVisLayer(VisualLayer* toDeleteVisLayer);

	// clear model (i.e., delete all layers)
	void clearLayers();

};