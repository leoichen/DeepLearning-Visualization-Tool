#include "model.h"
#include <torch/torch.h>

void Model::initLayers()
{
	for (int i=0; i < Layers.size(); i++) {	
		if (Layers[i].type == LINEAR) {
			auto fc = register_module("fc" + std::to_string(i),
				torch::nn::Linear(Layers[i].inputSize, Layers[i].outputSize));
			if (Layers[i].dropOut < 1) {
				auto dropout = register_module("dropout", torch::nn::Dropout(torch::nn::DropoutOptions(Layers[i].dropOut)));
				dropOut1d.push_back(dropout);
			}
			else {
				dropOut1d.push_back(nullptr);
			}
			fcLayers.push_back(fc);

		}
		else { //Conv
			auto conv = register_module("conv" + std::to_string(i),
				torch::nn::Conv2d(torch::nn::Conv2dOptions(Layers[i].inputSize, Layers[i].outputSize, Layers[i].kernelSize)));
			if (Layers[i].dropOut < 1) {
				torch::nn::Dropout2d dropout = register_module("dropout2d", torch::nn::Dropout2d(torch::nn::Dropout2dOptions(Layers[i].dropOut)));
				dropOut2d.push_back(dropout);
			}
			else {
				dropOut2d.push_back(nullptr);
			}
			convLayers.push_back(conv);
			
		}
		}
}

void Model::deleteLayer(layerType type)
{
	//Can only delete the last layer
	Layers.pop_back();
}

torch::Tensor Model::forward(torch::Tensor x)
{
	
	for (int i = 0; i < convLayers.size(); i++) {
		//std::cout << "in forward" << std::endl;
		if (Layers[i].pool == true) {
			x = torch::max_pool2d(convLayers[i](x), 2);
		}
		else {
			x = convLayers[i](x);
		}
		switch (Layers[i].activation) {
		case RELU:
			x = torch::relu(x);
			break;
		case SIGMOID:
			x = torch::sigmoid(x);
			break;
		case TANH:
			x = torch::tanh(x);
			break;
		default:
			break; //NONE
		}
		if (Layers[i].dropOut < 1) {
			x = dropOut2d[i](x);
		}
	}

	int fcInShape = convOut[0] * convOut[1] * convOut[2];
	x = x.view({-1, fcInShape});

	for (int i = 0; i < fcLayers.size(); i++) {
		int newIndex = i + convLayers.size();
		x = fcLayers[i](x);
		switch (Layers[newIndex].activation) {
		case RELU:
			x = torch::relu(x);
			break;
		case SIGMOID:
			x = torch::sigmoid(x);
			break;
		case TANH:
			x = torch::tanh(x);
			break;
		default:
			break; //NONE
		}

		if (Layers[newIndex].dropOut < 1) {
			x = dropOut1d[i](x);

		}
	}

	return x;
}

void Model::newModel(std::vector<Layer> layers, std::vector<int> convOutSize)
{
	clear(); //Clear all previously initialized arrays and vectors
	Layers = layers;
	convOut = convOutSize;
    initLayers();

}

void Model::clear()
{	
	//std::cout << "1" << std::endl;
	Layers.clear();
	fcLayers.clear();
	convLayers.clear();
	dropOut1d.clear();
	dropOut2d.clear();

}

