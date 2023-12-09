#include "Trainer.h"

void Trainer::newTrainer(float learningRate, Optim optimizer, int numEpochs, int batch, bool scheduler)
{	
	clear(); //Clear losses and accuracies
	lr = learningRate;
	optim = optimizer;
	batchSize = batch;
	epochs = numEpochs;
	LRsched = scheduler;
	currEpoch = 0;
	continueRun = true;
}

void Trainer::runTrain(Model& model)
{

	
		torch::optim::SGD optimizer(model.parameters(), torch::optim::SGDOptions(lr).momentum(0.5));

		if (optim == SGD) {
			torch::optim::SGD optimizer(model.parameters(), torch::optim::SGDOptions(lr).momentum(0.5));
		}
		else if (optim == ADAM) {
			torch::optim::Adam optimizer(model.parameters(), torch::optim::AdamOptions(lr));
		}


		std::cout << model << std::endl;

		torch::DeviceType device_type;
		if (torch::cuda::is_available()) {
			std::cout << "CUDA available! Training on GPU." << std::endl;
			device_type = torch::kCUDA;
		}
		else {
			std::cout << "Training on CPU." << std::endl;
			device_type = torch::kCPU;
		}
		torch::Device device(device_type);

		model.to(device);
	

		auto trainDataset = torch::data::datasets::MNIST(dataRoot)
			.map(torch::data::transforms::Normalize<>(0.1307, 0.3081))
			.map(torch::data::transforms::Stack<>());
		const size_t trainDataSize = trainDataset.size().value();
		auto trainLoader =
			torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
				std::move(trainDataset), batchSize);
		
		auto testDataset = torch::data::datasets::MNIST(
			dataRoot, torch::data::datasets::MNIST::Mode::kTest)
			.map(torch::data::transforms::Normalize<>(0.1307, 0.3081))
			.map(torch::data::transforms::Stack<>());
		const size_t testDataSize = testDataset.size().value();
		auto testLoader =
			torch::data::make_data_loader(std::move(testDataset), 1000); //set default test batch size as 1000

		for (size_t epoch = 0; epoch < epochs; epoch++) {
			if (continueRun) {
				LRvec.push_back(lr);
				float loss = train(model, optimizer, *trainLoader, device, epoch, trainDataSize);
				float accuracy = test(model, *testLoader, device, testDataSize);

				if (continueRun) {
					losses.push_back(loss);
					accuracies.push_back(accuracy);
					currEpoch += 1;
				}
			}
			else {
				break;
			}

	}

}

void Trainer::ThreadEntry1(Trainer *trainer, Model *model)
{
	trainer->runTrain(*model);
}

void Trainer::clear()
{
	losses.clear();
	accuracies.clear();
	LRvec.clear();
}


