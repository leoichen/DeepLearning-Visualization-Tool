//#include <string>
//#include <algorithm>
//#include "DrawingUtilNG.h"
//#include "StringPlus.h"
//#include "GraphicFont.h"
//#include "ButtonCollection.h"
//
//
//using namespace std;
//
//void ControlCollection::add(int locX, int locY, int wid, int hei, int val, std::string text,
//	GraphicFont* aFont, std::string description)
//{
//	theControls.push_back(new Button(locX, locY, wid, hei, val, text, aFont, description));
//}
//
//void ControlCollection::remove(int val)
//{
//	int wantedIndex = getIndex(val);
//	if (wantedIndex >= 0) {
//		delete theControls[wantedIndex];
//		theControls.erase(theControls.begin() + wantedIndex);
//	}
//}
//
//void ControlCollection::disableButton(int val)
//{
//	int wantedIndex = getIndex(val);
//	if (wantedIndex >= 0)
//		theControls.at(wantedIndex)->disable();
//}
//
//void ControlCollection::enableButton(int val)
//{
//	int wantedIndex = getIndex(val);
//	if (wantedIndex >= 0)
//		theControls.at(wantedIndex)->enable();
//}
//
//int ControlCollection::checkClick(int locX, int locY)
//{
//	int buttonValue;
//	for (auto& currButton : theControls) {
//		buttonValue = currButton->getVal(locX, locY);
//		if (buttonValue != FSKEY_NULL)
//			return buttonValue;
//	}
//	return FSKEY_NULL;
//}
//
//void ControlCollection::checkHover(int locX, int locY)
//{
//	for (auto& currButton : theControls) {
//		currButton->paintHover(locX, locY);
//	}
//}
//
//void ControlCollection::paint()
//{
//	if (theControls.size() > 0) {
//		int buttonWidth = theControls[0]->getWidth() + 10;
//		//glLoadIdentity();
//		glColor3ub(255, 255, 255);
//		int wid, hei;
//		FsGetWindowSize(wid, hei);
//		//DrawingUtilNG::drawRectangle(wid - buttonWidth, 0, buttonWidth, 800, true);
//
//		for (auto& currButton : theControls) {
//			currButton->paint();
//		}
//	} 
//}
//
//ControlCollection::~ControlCollection()
//{
//	for (auto& currButton : theControls)
//		delete currButton;
//}
//
//int ControlCollection::getIndex(int val)
//{
//	int i = 0;
//	for (auto& currButton : theControls) {
//		if (currButton->getVal() == val)
//			return i;  // possible exit from function
//		i++;
//	}
//	return -1;
//}
//
//Control::Control(int locX, int locY, int wid, int hei, int val, std::string text,
//	GraphicFont* aFont, std::string description)
//{
//	x = locX; y = locY; xSize = wid; ySize = hei;
//	value = val; label = text;
//	theFont = aFont;
//	hover = description;
//}
//
//int Button::getVal(int locX, int locY)
//{
//	if (enabled && x <= locX && locX <= x + xSize && y <= locY && locY <= y + ySize)
//		return value;
//	else
//		return FSKEY_NULL;
//}
//
//void Button::paintHover(int locX, int locY)
//{
//	if (hover.length() > 0 && x <= locX && locX <= x + xSize && y <= locY && locY <= y + ySize) {
//		string message = "";
//
//		if (!enabled) {
//			glColor3ub(155, 155, 0);
//			message += "DISABLED: ";
//		}
//		else
//			glColor3ub(255, 255, 0);
//
//		message += hover;
//		message = StringPlus::wrapLongLine(message, 20);
//		vector<string> messageLines = StringPlus::split(message, "\n");
//
//		DrawingUtilNG::drawRectangle(x + 5, y + ySize + 5, 100, 10 + messageLines.size() * 10, true);
//		glColor3ub(0, 0, 0);
//		DrawingUtilNG::drawRectangle(x + 5, y + ySize + 5, 100, 10 + messageLines.size() * 10, false);
//
//		int yAdjust = ySize + 20;
//		for (auto aLine : messageLines) {
//			theFont->drawText(aLine, x + 7, y + yAdjust, .1);
//			yAdjust += 10;
//		}
//	}
//
//}
//
//void Button::paint()
//{
//	// top light
//	glColor3ub(230, 230, 230);
//	DrawingUtilNG::drawRectangle(x, y, xSize, ySize, true);
//
//	// bottom shadow
//	glColor3ub(110, 110, 110);
//	glBegin(GL_QUADS);
//	glVertex2i(x, y + ySize);
//	glVertex2i(x + ySize / 2, y + ySize / 2);
//	glVertex2i(x + xSize, y + ySize / 2);
//	glVertex2i(x + xSize, y + ySize);
//	glEnd();
//
//	glBegin(GL_TRIANGLES);
//	glVertex2i(x + xSize, y);
//	glVertex2i(x + xSize, y + ySize);
//	glVertex2i(x + xSize - ySize / 2, y + ySize / 2);
//	glEnd();
//
//	// flat
//	glColor3ub(210, 210, 210);
//	int width = 3;
//	DrawingUtilNG::drawRectangle(x + width, y + width, xSize - width * 2, ySize - width * 2, true);
//
//	if (enabled)
//		theFont->setColorRGB(0, 0, 0);
//	else
//		theFont->setColorRGB(.4, .4, .4);
//
//	int hei = theFont->getLetterHeight();
//	theFont->drawText(label, x + xSize / 2, y + ySize / 2, (float)(ySize - 8) / hei, 0, true);
//
//}
//
//void ComboBox::addItem(std::string newItem, int location)
//{
//	newItem = StringPlus::trim(newItem);
//
//	location = max(location, 0);
//
//	if (location >= items.size())
//		items.push_back(newItem);
//	else
//		items.insert(items.begin() + location, newItem);
//}
//
//int ComboBox::getVal(int locX, int locY)
//{
//	if (!enabled)
//		return FSKEY_NULL;
//
//	else if (x + xSize - ySize <= locX && locX <= x + xSize && y <= locY && locY <= y + ySize) {
//		// clicked on little triangle
//		isExpanded = !isExpanded;
//		return value + currSelection;
//	}
//	else if (isExpanded)
//		if (x + 3 <= locX && locX <= x + xSize - ySize
//			&& y + ySize < locY && locY < y + ySize + itemYsize * items.size()) {
//			// click is just below combo box, figure out where
//			int clickItem = (locY - y - ySize) / itemYsize;
//			if (clickItem < items.size()) {
//				currSelection = clickItem;
//				isExpanded = false;
//				return value + clickItem;
//			}
//
//		}
//		else
//			isExpanded = false;
//
//}
//
//void ComboBox::paintHover(int locX, int locY)
//{
//	if (isExpanded) {
//		if (x + 3 <= locX && locX <= x + xSize - ySize
//			&& y + ySize < locY && locY < y + ySize + itemYsize * items.size()) {
//			// highlight 
//			int clickItem = (locY - y - ySize) / itemYsize;
//			// turn on blending to create purplish transparent highlight
//			glEnable(GL_BLEND);
//			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//			glColor4ub(0, 0, 255, 140);
//			DrawingUtilNG::drawRectangle(x + 3, y + ySize + itemYsize * clickItem,
//				xSize - 3 - ySize, itemYsize, true);
//			// turn off blending 
//			glDisable(GL_BLEND);
//
//		}
//	}
//	else {
//		Control::paintHover(locX, locY);
//	}
//}
//
//void ComboBox::paint()
//{
//	using namespace DrawingUtilNG;
//
//	// box
//	glColor3ub(250, 250, 190);
//	drawRectangle(x, y, xSize - ySize, ySize, true);
//	glColor3ub(0, 0, 0);
//	drawRectangle(x, y, xSize - ySize, ySize, false);
//
//	// little pulldown button
//	int topLeft = x + xSize - ySize;
//	// top light
//	glColor3ub(230, 230, 230);
//	DrawingUtilNG::drawRectangle(topLeft, y, ySize, ySize, true);
//
//	// bottom shadow
//	glColor3ub(110, 110, 110);
//	glBegin(GL_QUADS);
//	glVertex2i(topLeft, y + ySize);
//	glVertex2i(topLeft + ySize / 2, y + ySize / 2);
//	glVertex2i(topLeft + ySize, y + ySize / 2);
//	glVertex2i(topLeft + ySize, y + ySize);
//	glEnd();
//
//	glBegin(GL_TRIANGLES);
//	glVertex2i(topLeft + ySize, y);
//	glVertex2i(topLeft + ySize, y + ySize);
//	glVertex2i(topLeft + ySize - ySize / 2, y + ySize / 2);
//	glEnd();
//
//	// flat
//	glColor3ub(210, 210, 210);
//	int width = 3;
//	DrawingUtilNG::drawRectangle(topLeft + width, y + width, ySize - width * 2, ySize - width * 2, true);
//
//	// little black triangle
//	if (enabled)
//		glColor3ub(0, 0, 0);
//	else
//		glColor3f(0.4, 0.4, 0.4);
//	glBegin(GL_TRIANGLES);
//	glVertex2i(topLeft + 3 * width, y + 3 * width);
//	glVertex2i(topLeft + ySize - 3 * width, y + 3 * width);
//	glVertex2i(topLeft + ySize / 2, y + 6 * width);
//	glEnd();
//
//	// text showing current selection
//
//	if (enabled)
//		theFont->setColorRGB(0, 0, 0);
//	else
//		theFont->setColorRGB(.4, .4, .4);
//
//	string mainText = label;
//	if (0 <= currSelection && currSelection < items.size())
//		mainText = items[currSelection];
//
//	int hei = theFont->getLetterHeight();
//	float fontScale = (float)(ySize - 8) / hei;
//	theFont->drawText(mainText, x + 3, y + ySize / 2 + hei * fontScale / 2.,
//		fontScale, 0, false);
//
//	if (isExpanded) {
//		// draw the box
//		int currY = y + ySize;
//		glColor3ub(250, 250, 190);
//		drawRectangle(x + 3, currY, xSize - ySize, itemYsize * items.size(), true);
//		glColor3ub(0, 0, 0);
//		drawRectangle(x + 3, currY, xSize - ySize, itemYsize * items.size(), false);
//
//		currY += itemYsize / 2 + hei * fontScale / 2.;
//		for (auto currItem : items) {
//			// write up the item text
//			theFont->drawText(currItem, x + 4, currY, fontScale, 0, false);
//			currY += itemYsize;
//		}
//
//	}
//
//}
//
//
//
//

#include <string>
#include <algorithm>
#include "DrawingUtilNG.h"
#include "StringPlus.h"
#include "GraphicFont.h"
#include "ButtonCollection.h"


using namespace std;

void ControlCollection::add(int locX, int locY, int wid, int hei, int val, std::string text,
	GraphicFont* aFont, std::string description)
{
	theControls.push_back(new Button(locX, locY, wid, hei, val, text, aFont, description));
}

void ControlCollection::remove(int val)
{
	int wantedIndex = getIndex(val);
	if (wantedIndex >= 0) {
		delete theControls[wantedIndex];
		theControls.erase(theControls.begin() + wantedIndex);
	}
}

void ControlCollection::disableButton(int val)
{
	int wantedIndex = getIndex(val);
	if (wantedIndex >= 0)
		theControls.at(wantedIndex)->disable();
}

void ControlCollection::enableButton(int val)
{
	int wantedIndex = getIndex(val);
	if (wantedIndex >= 0)
		theControls.at(wantedIndex)->enable();
}

int ControlCollection::checkClick(int locX, int locY)
{
	int buttonValue;
	for (auto& currButton : theControls) {
		buttonValue = currButton->getVal(locX, locY);
		if (buttonValue != FSKEY_NULL)
			return buttonValue;
	}
	return FSKEY_NULL;
}

void ControlCollection::checkHover(int locX, int locY)
{
	for (auto& currButton : theControls) {
		currButton->paintHover(locX, locY);
	}
}

void ControlCollection::paint()
{
	if (theControls.size() > 0) {
		int buttonWidth = theControls[0]->getWidth() + 10;
		//glLoadIdentity();
		glColor3ub(255, 255, 255);
		int wid, hei;
		FsGetWindowSize(wid, hei);
		//DrawingUtilNG::drawRectangle(wid - buttonWidth, 0, buttonWidth, 800, true);

		for (auto& currButton : theControls) {
			currButton->paint();
		}
	}
}

ControlCollection::~ControlCollection()
{
	for (auto& currButton : theControls)
		delete currButton;
}

int ControlCollection::getIndex(int val)
{
	int i = 0;
	for (auto& currButton : theControls) {
		if (currButton->getVal() == val)
			return i;  // possible exit from function
		i++;
	}
	return -1;
}

Control::Control(int locX, int locY, int wid, int hei, int val, std::string text,
	GraphicFont* aFont, std::string description)
{
	x = locX; y = locY; xSize = wid; ySize = hei;
	value = val; label = text;
	theFont = aFont;
	hover = description;
}

int Button::getVal(int locX, int locY)
{
	if (enabled && x <= locX && locX <= x + xSize && y <= locY && locY <= y + ySize)
		return value;
	else
		return FSKEY_NULL;
}

void Control::paintHover(int locX, int locY)
{
	if (hover.length() > 0 && x <= locX && locX <= x + xSize && y <= locY && locY <= y + ySize) {
		string message = "";

		if (!enabled) {
			glColor3ub(155, 155, 0);
			message += "DISABLED: ";
		}
		else
			glColor3ub(255, 255, 0);

		message += hover;
		message = StringPlus::wrapLongLine(message, 20);
		vector<string> messageLines = StringPlus::split(message, "\n");

		DrawingUtilNG::drawRectangle(x + 5, y + ySize + 5, 100, 10 + messageLines.size() * 10, true);
		glColor3ub(0, 0, 0);
		DrawingUtilNG::drawRectangle(x + 5, y + ySize + 5, 100, 10 + messageLines.size() * 10, false);

		int yAdjust = ySize + 20;
		for (auto aLine : messageLines) {
			theFont->drawText(aLine, x + 7, y + yAdjust, .1);
			yAdjust += 10;
		}
	}

}

void Button::paint()
{
	// top light
	glColor3ub(230, 230, 230);
	DrawingUtilNG::drawRectangle(x, y, xSize, ySize, true);

	// bottom shadow
	glColor3ub(110, 110, 110);
	glBegin(GL_QUADS);
	glVertex2i(x, y + ySize);
	glVertex2i(x + ySize / 2, y + ySize / 2);
	glVertex2i(x + xSize, y + ySize / 2);
	glVertex2i(x + xSize, y + ySize);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex2i(x + xSize, y);
	glVertex2i(x + xSize, y + ySize);
	glVertex2i(x + xSize - ySize / 2, y + ySize / 2);
	glEnd();

	// flat
	glColor3ub(210, 210, 210);
	int width = 3;
	DrawingUtilNG::drawRectangle(x + width, y + width, xSize - width * 2, ySize - width * 2, true);

	if (enabled)
		theFont->setColorRGB(0, 0, 0);
	else
		theFont->setColorRGB(.4, .4, .4);

	int hei = theFont->getLetterHeight();
	theFont->drawText(label, x + xSize / 2, y + ySize / 2, (float)(ySize - 8) / hei, 0, true);

}

void ComboBox::addItem(std::string newItem, int location)
{
	newItem = StringPlus::trim(newItem);

	location = max(location, 0);

	if (location >= items.size())
		items.push_back(newItem);
	else
		items.insert(items.begin() + location, newItem);
}

int ComboBox::getVal(int locX, int locY)
{
	if (!enabled)
		return FSKEY_NULL;

	else if (x + xSize - ySize <= locX && locX <= x + xSize && y <= locY && locY <= y + ySize) {
		// clicked on little triangle
		isExpanded = !isExpanded;
		return value + currSelection;
	}
	else if (isExpanded)
		if (x + 3 <= locX && locX <= x + xSize - ySize
			&& y + ySize < locY && locY < y + ySize + itemYsize * items.size()) {
			// click is just below combo box, figure out where
			int clickItem = (locY - y - ySize) / itemYsize;
			if (clickItem < items.size()) {
				currSelection = clickItem;
				isExpanded = false;
				return value + clickItem;
			}

		}
		else
			isExpanded = false;
		
	return FSKEY_NULL;

}

void ComboBox::paintHover(int locX, int locY)
{
	if (isExpanded) {
		if (x + 3 <= locX && locX <= x + xSize - ySize
			&& y + ySize < locY && locY < y + ySize + itemYsize * items.size()) {
			// highlight 
			int clickItem = (locY - y - ySize) / itemYsize;
			// turn on blending to create purplish transparent highlight
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4ub(0, 0, 255, 140);
			DrawingUtilNG::drawRectangle(x + 3, y + ySize + itemYsize * clickItem,
				xSize - 3 - ySize, itemYsize, true);
			// turn off blending 
			glDisable(GL_BLEND);

		}
	}
	else {
		Control::paintHover(locX, locY);
	}
}

void ComboBox::paint()
{
	using namespace DrawingUtilNG;

	// box
	if (enabled)
		glColor3ub(250, 250, 190);
	else
		glColor3ub(200, 200, 140);

	drawRectangle(x, y, xSize - ySize, ySize, true);
	glColor3ub(0, 0, 0);
	drawRectangle(x, y, xSize - ySize, ySize, false);

	// little pulldown button
	int topLeft = x + xSize - ySize;
	// top light
	glColor3ub(230, 230, 230);
	DrawingUtilNG::drawRectangle(topLeft, y, ySize, ySize, true);

	// bottom shadow
	glColor3ub(110, 110, 110);
	glBegin(GL_QUADS);
	glVertex2i(topLeft, y + ySize);
	glVertex2i(topLeft + ySize / 2, y + ySize / 2);
	glVertex2i(topLeft + ySize, y + ySize / 2);
	glVertex2i(topLeft + ySize, y + ySize);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex2i(topLeft + ySize, y);
	glVertex2i(topLeft + ySize, y + ySize);
	glVertex2i(topLeft + ySize - ySize / 2, y + ySize / 2);
	glEnd();

	// flat
	glColor3ub(210, 210, 210);
	int width = 3;
	DrawingUtilNG::drawRectangle(topLeft + width, y + width, ySize - width * 2, ySize - width * 2, true);

	// little black triangle
	if (enabled)
		glColor3ub(0, 0, 0);
	else
		glColor3f(0.4, 0.4, 0.4);
	glBegin(GL_TRIANGLES);
	glVertex2i(topLeft + 3 * width, y + 3 * width);
	glVertex2i(topLeft + ySize - 3 * width, y + 3 * width);
	glVertex2i(topLeft + ySize / 2, y + 6 * width);
	glEnd();

	// text showing current selection

	if (enabled)
		theFont->setColorRGB(0, 0, 0);
	else
		theFont->setColorRGB(.4, .4, .4);

	string mainText = label;
	if (0 <= currSelection && currSelection < items.size())
		mainText = items[currSelection];

	int hei = theFont->getLetterHeight();
	float fontScale = (float)(ySize - 8) / hei;
	theFont->drawText(mainText, x + 3, y + ySize / 2 + hei * fontScale / 2.,
		fontScale, 0, false);

	if (isExpanded) {
		// draw the box
		int currY = y + ySize;
		glColor3ub(250, 250, 190);
		drawRectangle(x + 3, currY, xSize - ySize, itemYsize * items.size(), true);
		glColor3ub(0, 0, 0);
		drawRectangle(x + 3, currY, xSize - ySize, itemYsize * items.size(), false);

		currY += itemYsize / 2 + hei * fontScale / 2.;
		for (auto currItem : items) {
			// write up the item text
			theFont->drawText(currItem, x + 4, currY, fontScale, 0, false);
			currY += itemYsize;
		}

	}

}

int TextBox::getVal(int locX, int locY)
{
	if (!enabled) {
		beingEdited = false;
		return FSKEY_NULL;
	}
	else if (x <= locX && locX <= x + xSize && y <= locY && locY <= y + ySize) {
		// clicked on text box
		beingEdited = true;
		return value;
	}
	else
		beingEdited = false; // there was a click outside textbox so stop editing

	return FsInkey();   // just something that is not FSKEY_NULL
}

void TextBox::paint()
{
	using namespace DrawingUtilNG;

	// box
	if (enabled)
		glColor3ub(250, 250, 190);
	else
		glColor3ub(200, 200, 140);

	drawRectangle(x, y, xSize, ySize, true);
	glColor3ub(0, 0, 0);
	drawRectangle(x, y, xSize, ySize, false);

	// text showing current text

	string toShow = currText;;
	if (enabled) {
		theFont->setColorRGB(0, 0, 0);
		if (beingEdited)
			toShow = currText + "_";
	}
	else
		theFont->setColorRGB(.4, .4, .4);

	int hei = theFont->getLetterHeight();
	float fontScale = (float)(ySize - 8) / hei;
	theFont->drawText(label, x + 3, y - 25 + ySize / 2 + hei * fontScale / 2.,
		fontScale, 0, false);
	theFont->drawText(toShow, x + 3, y + ySize / 2 + hei * fontScale / 2.,
		fontScale, 0, false);

}

std::string TextBox::getString(int key)
{
	if (enabled && beingEdited) {
		DrawingUtilNG::buildStringFromFsInkey(key, currText);
	}
	return currText;
}

