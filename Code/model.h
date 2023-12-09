#pragma once
#include <torch/torch.h>
#include <vector>
#include "VisualLayerManager.h"

class Model : public torch::nn::Module {
private:
    std::vector<Layer> Layers;
    std::vector<int> convOut;
    std::vector<torch::nn::Linear> fcLayers;
    std::vector<torch::nn::Conv2d> convLayers;
    std::vector<torch::nn::Dropout> dropOut1d;
    std::vector<torch::nn::Dropout2d> dropOut2d;
public:
    void initLayers();
    void deleteLayer(layerType type);
    torch::Tensor forward(torch::Tensor x);
    void clear();
    void newModel(std::vector<Layer> layers, std::vector<int> convOutSize); //Clear old model and initialize new model
};