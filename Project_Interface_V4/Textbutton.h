#pragma once
#ifndef TEXTINPUTBUTTON_H
#define TEXTINPUTBUTTON_H

#include <string>

class TextInputButton {
public:
    TextInputButton(int x, int y, int width, int height, int id, const std::string& label, const std::string& defaultText);

    void draw();
    bool handleInput(int key, int mouseEvent, int mouseX, int mouseY);
    std::string getText() const;

private:
    int x, y, width, height, id;
    std::string label, text;
    bool isFocused;

    void appendChar(char c);
    void backspace();
};

#endif
