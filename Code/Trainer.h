#pragma once
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "model.h"
#include <torch/torch.h>
#include <chrono>


enum Optim {
	SGD,
	ADAM
};



class Trainer {
private:
	float lr;
	int batchSize;
	int epochs;
    int currEpoch;
    bool LRsched;
    Optim optim;
    std::string dataRoot = "./data";
    std::vector<float> losses;
    std::vector<float> accuracies;
    std::vector<float> LRvec;
    bool continueRun = true;


public:

    void newTrainer(float learningRate, Optim optimizer, int numEpochs = 10, int batch = 64, bool scheduler=false);
	void runTrain(Model& model);

    static void ThreadEntry1(Trainer* trainer, Model *model);

	template <typename DataLoader>
	float train(Model& model, torch::optim::Optimizer& optimizer, DataLoader& dataLoader, torch::Device device, size_t epoch, size_t dataSize);

    template<typename DataLoader>
    float test(Model& model, DataLoader& dataLoader, torch::Device device, size_t dataset_size);

    int getCurrEpoch() { return currEpoch; };
    float getCurrLoss() { 
        if (losses.empty()) {
            return 0;
        }
        return losses.back(); };
    float getCurrAcc() { 
        if (accuracies.empty()) {
            return 0;
        }
        return accuracies.back(); };
    float getCurrLR() { return lr; };
    float getEpochs() { return epochs; };
    
    std::vector<float> getLosses() { return losses; };
    std::vector<float> getAcc() { return accuracies; };
    void changeContinueRun(bool cont) { continueRun = cont; };
    bool getContinueRun() { return continueRun; };
    void clear();

};

template<typename DataLoader>
inline float Trainer::train(Model& model, 
    torch::optim::Optimizer& optimizer, 
    DataLoader& dataLoader, 
    torch::Device device, 
    size_t epoch,
    size_t dataSize)
{
    size_t batch_idx = 0;
    float lossOut = 0;
    std::cout << "In train" << std::endl;
    for (auto& batch : dataLoader) {

        if (this->continueRun) {
            auto data = batch.data.to(device);
            auto targets = batch.target.to(device);
            optimizer.zero_grad();
            auto output = model.forward(data);
            auto loss = torch::cross_entropy_loss(output, targets);
            loss.backward();
            optimizer.step();

            if (batch_idx++ % 10 == 0) {
                std::printf(
                    "\rTrain Epoch: %ld [%5ld/%5ld] Loss: %.4f",
                    epoch,
                    batch_idx * batch.data.size(0),
                    dataSize,
                    loss.item<float>());
            }
            lossOut += loss.item<float>();

        }
        else {
            return 0;
        }
    }
    return lossOut / (float)batch_idx;
    
}

template<typename DataLoader>
inline float Trainer::test(Model& model, DataLoader& dataLoader, torch::Device device, size_t dataSize)
{
    torch::NoGradGuard no_grad;
    model.eval();
    int correct = 0;
    for (const auto& batch : dataLoader) {
        if (continueRun) {
            torch::Tensor data = batch.data.to(device);
            torch::Tensor targets = batch.target.to(device);
            auto output = model.forward(data);
            auto pred = output.argmax(1);
            correct += pred.eq(targets).sum().template item<int64_t>();
        }
        else {
            return 0;
        }
    }
    std::cout << ", Test Accuracy: " << (float)correct / (float)dataSize << std::endl;
    return (float)correct / (float)dataSize;
    
}
