//#include <torch/torch.h>
//#include <iostream>
//
//int main() {
//  torch::Tensor tensor = torch::rand({2, 3});
//  if (torch::cuda::is_available()) {
//	  std::cout << "CUDA is available! Training on GPU" << std::endl;
//	  auto tensor_cuda = tensor.cuda();
//	  std::cout << tensor_cuda << std::endl;
//  }
//  else
//  {
//	  std::cout << "CUDA is not available! Training on CPU" << std::endl;
//	  std::cout << tensor << std::endl;	  
//  }
//
//  std::cin.get();
//}

#include <torch/torch.h>
//
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

// Where to find the MNIST dataset.
const char* kDataRoot = "./data";

// The batch size for training.
const int64_t kTrainBatchSize = 64;

// The batch size for testing.
const int64_t kTestBatchSize = 1000;

// The number of epochs to train.
const int64_t kNumberOfEpochs = 10;

// After how many batches to log a new update with the loss value.
const int64_t kLogInterval = 10;
//
//struct Net : torch::nn::Module {
//    Net()
//        : conv1(torch::nn::Conv2dOptions(1, 10, /*kernel_size=*/5)),
//        conv2(torch::nn::Conv2dOptions(10, 20, /*kernel_size=*/5)),
//        fc1(320, 50),
//        fc2(50, 10) {
//        register_module("conv1", conv1);
//        register_module("conv2", conv2);
//        register_module("conv2_drop", conv2_drop);
//        register_module("fc1", fc1);
//        register_module("fc2", fc2);
//    }
//
//    torch::Tensor forward(torch::Tensor x) {
//        x = torch::relu(torch::max_pool2d(conv1->forward(x), 2));
//        x = torch::relu(
//            torch::max_pool2d(conv2_drop->forward(conv2->forward(x)), 2));
//        x = x.view({ -1, 320 });
//        x = torch::relu(fc1->forward(x));
//        x = torch::dropout(x, /*p=*/0.5, /*training=*/is_training());
//        x = fc2->forward(x);
//        return torch::log_softmax(x, /*dim=*/1);
//    }
//
//    torch::nn::Conv2d conv1;
//    torch::nn::Conv2d conv2;
//    torch::nn::Dropout2d conv2_drop;
//    torch::nn::Linear fc1;
//    torch::nn::Linear fc2;
//};


class DynamicNet : public torch::nn::Module {
public:
    DynamicNet(const std::vector<std::string>& layer_types, const std::vector<int>& layer_inputs,
        const std::vector<int>& layer_outputs, const std::vector<std::string>& activation_functions) {
        if (layer_types.size() != layer_inputs.size() || layer_inputs.size() != activation_functions.size()) {
            throw std::invalid_argument("Input sizes of layer_types, layer_sizes, and activation_functions must be equal.");
        }

        for (size_t i = 0; i < layer_inputs.size(); ++i) {
            if (layer_types[i] == "linear") {
                auto fc = register_module("fc" + std::to_string(i + 1),
                    torch::nn::Linear(layer_inputs[i], layer_outputs[i]));
                fc_layers.push_back(fc);
            }
            else if (layer_types[i] == "conv2d") {
                auto conv2d = register_module("conv2d" + std::to_string(i + 1),
                    torch::nn::Conv2d(torch::nn::Conv2dOptions(layer_inputs[i], layer_outputs[i], 5)));
                conv_layers.push_back(conv2d);
            }
            else {
                throw std::invalid_argument("Unsupported layer type: " + layer_types[i]);
            }

            if (activation_functions[i] == "relu") {
                activation_functions_.push_back(torch::relu);
            }
            else if (activation_functions[i] == "sigmoid") {
                activation_functions_.push_back(torch::sigmoid);
            }
            else if (activation_functions[i] == "tanh") {
                activation_functions_.push_back(torch::tanh);
            }
            else {
                throw std::invalid_argument("Unsupported activation function: " + activation_functions[i]);
            }
        }
    }

    torch::Tensor forward(torch::Tensor x) {
        //std::cout << "conv layers:" << conv_layers.size() << std::endl;
        //std::cout << "linear layers:" << fc_layers.size() << std::endl;
        for (size_t i = 0; i < conv_layers.size(); i++) {
            x = torch::relu(torch::max_pool2d(conv_layers[i](x), 2));
        }
        x = x.view({-1, 160});
        for (size_t i = 0; i < fc_layers.size() - 1; i++) {
            x = torch::relu(fc_layers[i](x));
        }
        x = torch::log_softmax(fc_layers.back()(x), 1);

        //for (size_t i = 0; i < fc_layers.size(); ++i) {
        //    if (!conv_layers.empty()) {
        //        x = torch::relu(torch::max_pool2d(conv_layers[i](x), 2)); 
        //    }
        //    else {
        //        x = torch::relu(fc_layers[i](x));
        //    }

        //    if (i < activation_functions_.size()) {
        //        x = activation_functions_[i](x);
        //    }
        //}
        return x;
    }

private:
    std::vector<torch::nn::Linear> fc_layers;
    std::vector<torch::nn::Conv2d> conv_layers;
    std::vector<std::function<torch::Tensor(torch::Tensor)>> activation_functions_;
};

template <typename DataLoader>
void train(
    size_t epoch,
    DynamicNet& model,
    torch::Device device,
    DataLoader& data_loader,
    torch::optim::Optimizer& optimizer,
    size_t dataset_size) {
    model.train();
    size_t batch_idx = 0;
    for (auto& batch : data_loader) {
        auto data = batch.data.to(device), targets = batch.target.to(device);
        optimizer.zero_grad();
        auto output = model.forward(data);
        auto loss = torch::nll_loss(output, targets);
        AT_ASSERT(!std::isnan(loss.template item<float>()));
        loss.backward();
        optimizer.step();

        if (batch_idx++ % kLogInterval == 0) {
            std::printf(
                "\rTrain Epoch: %ld [%5ld/%5ld] Loss: %.4f",
                epoch,
                batch_idx * batch.data.size(0),
                dataset_size,
                loss.template item<float>());
        }
    }
}

template <typename DataLoader>
void test(
    DynamicNet& model,
    torch::Device device,
    DataLoader& data_loader,
    size_t dataset_size) {
    torch::NoGradGuard no_grad;
    model.eval();
    double test_loss = 0;
    int32_t correct = 0;
    for (const auto& batch : data_loader) {
        auto data = batch.data.to(device), targets = batch.target.to(device);
        auto output = model.forward(data);
        test_loss += torch::nll_loss(
            output,
            targets,
            /*weight=*/{},
            torch::Reduction::Sum)
            .template item<float>();
        auto pred = output.argmax(1);
        correct += pred.eq(targets).sum().template item<int64_t>();
    }

    test_loss /= dataset_size;
    std::printf(
        "\nTest set: Average loss: %.4f | Accuracy: %.3f\n",
        test_loss,
        static_cast<double>(correct) / dataset_size);
}
//
//auto main() -> int {
//    torch::manual_seed(1);
//
//    torch::DeviceType device_type;
//    if (torch::cuda::is_available()) {
//        std::cout << "CUDA available! Training on GPU." << std::endl;
//        device_type = torch::kCUDA;
//    }
//    else {
//        std::cout << "Training on CPU." << std::endl;
//        device_type = torch::kCPU;
//    }
//    torch::Device device(device_type);
//
//    Net model;
//    model.to(device);
//
//    auto train_dataset = torch::data::datasets::MNIST(kDataRoot)
//        .map(torch::data::transforms::Normalize<>(0.1307, 0.3081))
//        .map(torch::data::transforms::Stack<>());
//    const size_t train_dataset_size = train_dataset.size().value();
//    auto train_loader =
//        torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
//            std::move(train_dataset), kTrainBatchSize);
//
//    auto test_dataset = torch::data::datasets::MNIST(
//        kDataRoot, torch::data::datasets::MNIST::Mode::kTest)
//        .map(torch::data::transforms::Normalize<>(0.1307, 0.3081))
//        .map(torch::data::transforms::Stack<>());
//    const size_t test_dataset_size = test_dataset.size().value();
//    auto test_loader =
//        torch::data::make_data_loader(std::move(test_dataset), kTestBatchSize);
//
//    torch::optim::SGD optimizer(
//        model.parameters(), torch::optim::SGDOptions(0.01).momentum(0.5));
//
//    for (size_t epoch = 1; epoch <= kNumberOfEpochs; ++epoch) {
//        train(epoch, model, device, *train_loader, optimizer, train_dataset_size);
//        test(model, device, *test_loader, test_dataset_size);
//    }
//}

//#include <torch/torch.h>
//#include <vector>
//#include <string>



int main() {
    // Get user input for network architecture
    std::vector<std::string> layer_types;
    std::vector<int> layer_inputs;
    std::vector<int> layer_outputs;
    std::vector<std::string> activation_functions;
    int num_layers;
    std::cout << "Enter the number of layers: ";
    std::cin >> num_layers;

    for (int i = 0; i < num_layers; ++i) {
        std::string layer_type, activation_function;
        int layer_size;

        std::cout << "Enter the type of layer " << i + 1 << " (linear/conv2d): ";
        std::cin >> layer_type;
        layer_types.push_back(layer_type);

        std::cout << "Enter the input of layer " << i + 1 << ": ";
        std::cin >> layer_size;
        layer_inputs.push_back(layer_size);

        std::cout << "Enter the output of layer " << i + 1 << ": ";
        std::cin >> layer_size;
        layer_outputs.push_back(layer_size);

        std::cout << "Enter the activation function for layer " << i + 1 << " (relu/sigmoid/tanh): ";
        std::cin >> activation_function;
        activation_functions.push_back(activation_function);
    }

    // Create an instance of the dynamically defined model
    DynamicNet model(layer_types, layer_inputs, layer_outputs, activation_functions);

    // Print the model architecture
    std::cout << model << std::endl;

    //// Dummy input tensor for testing
    //torch::Tensor input;

    //if (!layer_types.empty() && layer_types[0] == "conv2d") {
    //    input = torch::randn({ 1, layer_sizes[0], 28, 28 });  // Adjust input size for convolutional layers
    //}
    //else {
    //    input = torch::randn({ 1, layer_sizes[0] });
    //}

    // Forward pass
    //torch::Tensor output = model.forward(input);

    // Print the output
    //std::cout << "Output: " << output << std::endl;

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
    
    auto train_dataset = torch::data::datasets::MNIST(kDataRoot)
        .map(torch::data::transforms::Normalize<>(0.1307, 0.3081))
        .map(torch::data::transforms::Stack<>());
    const size_t train_dataset_size = train_dataset.size().value();
    auto train_loader =
        torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
            std::move(train_dataset), kTrainBatchSize);
    
    auto test_dataset = torch::data::datasets::MNIST(
        kDataRoot, torch::data::datasets::MNIST::Mode::kTest)
        .map(torch::data::transforms::Normalize<>(0.1307, 0.3081))
        .map(torch::data::transforms::Stack<>());
    const size_t test_dataset_size = test_dataset.size().value();
    auto test_loader =
        torch::data::make_data_loader(std::move(test_dataset), kTestBatchSize);
    
    torch::optim::SGD optimizer(
        model.parameters(), torch::optim::SGDOptions(0.01).momentum(0.5));
    
    for (size_t epoch = 1; epoch <= kNumberOfEpochs; ++epoch) {
        train(epoch, model, device, *train_loader, optimizer, train_dataset_size);
        test(model, device, *test_loader, test_dataset_size);
    }

    return 0;
}