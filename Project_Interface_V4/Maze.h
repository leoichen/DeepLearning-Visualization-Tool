#pragma once

#include <iostream>
#include <string>
#include <fstream>


struct Cell {
	int row;
	int col;
};

class Maze {

public:	static const int MAX_MAZE_SIZE = 101;
public:	static const int MAZE_BLOCK_SIZE = 20;

	  // used for whole maze alterations
	  enum AlterModes { MIRROR_LEFT_RIGHT, MIRROR_UP_DOWN, MIRROR_DIAGONAL, QUARTER_TURN };

protected:
	int height = 20;
	int width = 20;
	int overallSize;
	Cell start = { 1, 1 };
	Cell end = { height,width };

	// connMatrix is a 2D square matrix 
	// indices of connMatrix indicates the row & column number of a specific cell in the maze
	//   for example: cell (3,14) is represented by index 3 * MAX_MAZE_SIZE + 14 
	// so, if max size = 100, 
	//	 when connMatrix[314][315] is true, this means cells (3,14) and (3,15) are connected to each other

	bool* connMatrix = nullptr;

public:
	// PS07  PS07  PS07  PS07  PS07  
	// creates a random maze using Aldous-Broder algorithm (random walk)
	void createRandom(int wid, int hei, Cell beginCell = { 1,1 }, Cell endCell = { 2,2 });

	Maze() { clear(); };
	Maze(int wid, int hei);

	// reads a file from permanent storage
	void readFile(std::ifstream& input);

	// some get and sets
	int getHeight() { return height; }
	int getWidth() { return width; }
	Cell getStart() { return start; }
	Cell getEnd() { return end; }

	// returns true if the first cell is connected to the second cell
	bool areConnected(Cell aCell, Cell anotherCell) {
		int anIndex = getIndex(aCell);
		int anotherIndex = getIndex(anotherCell);
		if (anIndex >= 0 && anotherIndex >= 0)
			// equivalent of connMatrix[anIndex][anotherIndex]
			return connMatrix[anIndex * overallSize + anotherIndex];
		else
			return false;
	}

	bool connect(Cell aCell, Cell anotherCell) {
		return setConnState(aCell, anotherCell, true);
	}

	bool disconnect(Cell aCell, Cell anotherCell) {
		return setConnState(aCell, anotherCell, false);
	}

	void draw();

	friend std::ostream& operator<<(std::ostream& out, const Maze& aMaze);

	// mouseX and mouseY are the pixel coordinates of a left click
	// key is either FSKEY_S, or FSKEY_E which sets the start or end cell
	void changeCell(int mouseX, int mouseY, int key);

	Cell getCellFromMouse(int mouseX, int mouseY);

	~Maze() {
		delete[]connMatrix;
	}

	// Maze.h: Add this function declaration
	void toggleWall(const Cell& cell1, const Cell& cell2);

	Cell getAdjacentCell(const Cell& currCell, int modelX, int modelY, int centerX, int centerY);
	void quarterTurn();

	void mirrorLeftRight();

	void mirrorUpDown();

	void mirrorOnDiagonal();

protected:
	bool setConnState(Cell aCell, Cell anotherCell, bool newState);
	void clear();
	int getIndex(Cell aCell) const {
		if (0 < aCell.row && aCell.row <= height && 0 < aCell.col && aCell.col <= width)
			return aCell.row * width + aCell.col;
		else
			return -1;  // error
	}

	// not happy with this function. works fine, but handling the indexing from 1,1 badly
	// if width=10, index of 109 is 10,9   but index of 110 calculates to 11,0 when it should be 10,10
	// only used by operator<< so not such an issue
	Cell getCell(int index) const {
		if (0 <= index && index < overallSize) {
			Cell test = { index / width, index % width };
			if (test.col == 0) {  // correction for last cell
				test.row--;
				test.col = width;
			}
			return test;
		}
		else
			return { -1,-1 }; // error
	}

	Cell getRandomNeighbor(Cell currCell);

	void alter(AlterModes wantedMode);

};