# DeepLearning-Visualization-Tool
![Visualizer Display Screen](https://github.com/leoichen/DeepLearning-Visualization-Tool/blob/main/images/Screenshot%202023-12-08%20172049.png?raw=true)

## Overview
The project is a Deep Learning tool that provides a graphical approach to creating neural networks. The tool allows you to drag and drop different layers and visualize the state of your model. This provides a way for those who don't have the technical capabilities to code neural networks to be able create them.

A lot files needed to run the Torch API are too large for GitHub. The complete code files can be found: https://drive.google.com/drive/folders/1MLd_l8DBpB-R15clHOXDwG9TWEffvp5e?usp=sharing. However, all the C++ code files are on GitHub.

## Installation
To run this code, you must have Libtorch properly installed. Libtorch provides the C++ Pytorch API which is the backbone to the deep learning models being built in this code.
In addition, the Libtorch Template Extention needs to be downloaded for Visual Studios. The download link can be found at: https://pytorch.org/cppdocs/installing.html#visual-studio-extension
**Libtorch Version**
1.10.0+cu113

## Application Usage

1. **Menu Buttons**: The list of menu buttons on the right side provides the user with different options to select.

2. **Layer Selection**: To begin, select `LAYER -> Linear/Conv` and then fill out all associated properties shown in the Selection box at the bottom of the screen.

3. **Model Architecture Block**: The white screen displays the Model Architecture Block where the user can Drag and Drop Conv or Linear layer blocks on the screen to connect with the previous layer.

4. **Running Training**: After adding the required layers, the user has to run training using the `RUN` button and select the appropriate training properties.

5. **Starting Training and Plotting**: After hitting the `START` button to start the training, the user can click on `PLOT` to observe the Accuracy and Loss graphs for the current training.

6. **Switching Between Graphs**: Note that when you enter the PLOT screen, you can use the `Switch Plot` button to switch between the Loss Graph or Accuracy Graph.

7. **Caution**: Remember to hit the `CANCEL/Cancel` button only when you plan on cancelling or stopping the training.
