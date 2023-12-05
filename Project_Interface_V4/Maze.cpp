#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include "fssimplewindow.h"
#include "DrawingUtilNG.h"
#include "Maze.h"

using namespace std;

void Maze::createRandom(int wid, int hei, Cell beginCell, Cell endCell)
{
	// clear connMatrix
	clear();
	srand(time(NULL));
	if (1 < wid && wid <= 100 && 1 < hei && hei <= 100) {    // just in case of bad input

		// setup width, height and create connMatrix
		width = wid;
		height = hei;
		overallSize = (width + 1) * (height + 1);
		connMatrix = new bool[overallSize * overallSize](); // the () makes them all zero

		// carefully set start and end
		if (beginCell.row > 0)
			start = beginCell;
		if (endCell.row > 0)
			end = endCell;

		// set up visited array
		// note: some versions of C++ do not allow variables when declaring static arrays
		//       which is why use I use constants (101x101 of bool = 10,201 bits = 1.2 MB of memory)
		bool visited[MAX_MAZE_SIZE][MAX_MAZE_SIZE] = { false };  // the {false} sets all values to false

		int unvisitedCount = width * height;  // will count down to zero

		// now we can start

		// select any cell and make it the current cell
		Cell currCell = { height / 2, width / 2 };  // start in the middle row, middle column
		Cell neighborCell;

		// mark it as visited
		visited[currCell.row][currCell.col] = true;
		unvisitedCount--;  // will do this every time a cell is marked as visited

		// loop until all cells have been visited
		while (unvisitedCount > 0) {
			// get random neighbor
			neighborCell = getRandomNeighbor(currCell);

			// if neighbor is NOT visited, mark it visited and connect current and neighbor
			if (!visited[neighborCell.row][neighborCell.col]) {
				visited[neighborCell.row][neighborCell.col] = true;
				unvisitedCount--;
				connect(currCell, neighborCell);
				connect(neighborCell, currCell);
			}

			// make neighbor the current cell, no matter what
			currCell = neighborCell;
		}
	}
}



// Maze.cpp: Implement the function
void Maze::toggleWall(const Cell& cell1, const Cell& cell2) {
	if (areConnected(cell1, cell2)) {
		// If there is a wall, remove it
		disconnect(cell1, cell2);
		disconnect(cell2, cell1); // Assuming bidirectional connectivity
	}
	else {
		// If there is no wall, add it
		connect(cell1, cell2);
		connect(cell2, cell1); // Assuming bidirectional connectivity
	}
}



Cell Maze::getAdjacentCell(const Cell& currCell, int modelX, int modelY, int centerX, int centerY) {
	Cell adjacentCell = currCell; // Default to the current cell

	bool nearLeft = modelX < centerX - MAZE_BLOCK_SIZE * 0.4;
	bool nearRight = modelX > centerX + MAZE_BLOCK_SIZE * 0.4;
	bool nearTop = modelY < centerY - MAZE_BLOCK_SIZE * 0.4;
	bool nearBottom = modelY > centerY + MAZE_BLOCK_SIZE * 0.4;

	// Check for corner cases and do nothing if a corner is clicked
	if ((nearLeft || nearRight) && (nearTop || nearBottom)) {
		// Corner case, do nothing
		return adjacentCell;
	}

	// Not a corner, proceed to find the adjacent cell
	if (nearLeft) {
		adjacentCell = { currCell.row, currCell.col - 1 }; // Left
	}
	else if (nearRight) {
		adjacentCell = { currCell.row, currCell.col + 1 }; // Right
	}
	else if (nearTop) {
		adjacentCell = { currCell.row - 1, currCell.col }; // Top
	}
	else if (nearBottom) {
		adjacentCell = { currCell.row + 1, currCell.col }; // Bottom
	}

	return adjacentCell;
}



Maze::Maze(int wid, int hei)
{
	width = wid;
	height = hei;
	start = { 1,1 };
	end = { height, width };
	overallSize = (width + 1) * (height + 1);
	connMatrix = new bool[overallSize * overallSize](); // the () makes them all zero
}

void Maze::readFile(std::ifstream& input)
{
	string wholeLineString; // used to read file one whole line at a time 
	stringstream lineStream; // used to easily get numbers from part of line
	int colonLocation; // used to store location of colon in line 
	Cell cellA, cellB;

	// delete old maze data
	clear();

	while (!input.eof()) {
		lineStream.clear();// just in case 

		getline(input, wholeLineString); // read the whole line into a string

		// if there is a colon, set up lineStream to start just after colon 
		// also, remember colonLocation, just in case 
		if ((colonLocation = wholeLineString.find(":")) != string::npos)
			lineStream.str(wholeLineString.substr(colonLocation + 1));
		else {
			colonLocation = -1;
			lineStream.str(wholeLineString);
		}

		// check for keywords in the line
		if (wholeLineString.find("Start:") != string::npos) // find() returns npos when not found 
			lineStream >> start.row >> start.col; // put values into class variables 

		else if (wholeLineString.find("End:") != string::npos) // find() returns npos when not found 
			lineStream >> end.row >> end.col; // put values into class variables 

		else if (wholeLineString.find("Conn:") != string::npos && connMatrix != nullptr) {
			lineStream >> cellA.row >> cellA.col >> cellB.row >> cellB.col;
			connect(cellA, cellB);  // note this is NOT bidirectional
		}
		else if (wholeLineString.find("Begin:") != string::npos)
			lineStream >> width >> height;

		else if (wholeLineString.find("Finish:") != string::npos) {
			break;  //possible exit from while loop
		}

		if (connMatrix == nullptr && width > 0 && height > 0) {
			overallSize = (width + 1) * (height + 1);
			connMatrix = new bool[overallSize * overallSize]();
		}
	}
}

void Maze::draw()
{
	if (connMatrix != nullptr) {
		using namespace DrawingUtilNG;

		int wallThick = MAZE_BLOCK_SIZE / 5;  // thickness of walls

		// start and end blocks
		glColor3ub(0, 255, 0);  // green
		drawRectangle(start.col * MAZE_BLOCK_SIZE, start.row * MAZE_BLOCK_SIZE,
			MAZE_BLOCK_SIZE, MAZE_BLOCK_SIZE, true);
		glColor3ub(255, 0, 0);  // red
		drawRectangle(end.col * MAZE_BLOCK_SIZE, end.row * MAZE_BLOCK_SIZE,
			MAZE_BLOCK_SIZE, MAZE_BLOCK_SIZE, true);

		glColor3ub(60, 60, 90);  // bluish gray
		// top wall
		drawRectangle(1 * MAZE_BLOCK_SIZE - wallThick / 2, (0 + 1) * MAZE_BLOCK_SIZE - wallThick / 2,
			MAZE_BLOCK_SIZE * width + wallThick, wallThick, true);

		// left wall
		drawRectangle((0 + 1) * MAZE_BLOCK_SIZE - wallThick / 2,
			1 * MAZE_BLOCK_SIZE - wallThick / 2, wallThick, MAZE_BLOCK_SIZE * height + wallThick, true);

		// interior walls
		for (int i = 1; i <= height; i++)
			for (int j = 1; j <= width; j++) {
				if (!areConnected({ i,j }, { i + 1,j })) { // no connection to cell to the south
					drawRectangle(j * MAZE_BLOCK_SIZE - wallThick / 2, (i + 1) * MAZE_BLOCK_SIZE - wallThick / 2,
						MAZE_BLOCK_SIZE + wallThick, wallThick, true);
				}
				if (!areConnected({ i,j }, { i,j + 1 })) { // no connection to cell to the east
					drawRectangle((j + 1) * MAZE_BLOCK_SIZE - wallThick / 2,
						i * MAZE_BLOCK_SIZE - wallThick / 2, wallThick, MAZE_BLOCK_SIZE + wallThick, true);
				}
			}
	}
}

Cell Maze::getCellFromMouse(int mouseX, int mouseY)
{
	Cell mouseCell = { mouseY / MAZE_BLOCK_SIZE , mouseX / MAZE_BLOCK_SIZE };
	if (0 < mouseCell.row && mouseCell.row <= height
		&& 0 < mouseCell.col && mouseCell.col <= width) {
		return mouseCell;
	}
	else
		return { -1,-1 };

}

void Maze::changeCell(int mouseX, int mouseY, int key)
{
	Cell mouseCell = getCellFromMouse(mouseX, mouseY);

	if (mouseCell.row != -1) {
		switch (key) {
		case FSKEY_S:
			start = mouseCell;
			break;
		case FSKEY_E:
			end = mouseCell;
			break;
		}
	}
}

bool Maze::setConnState(Cell aCell, Cell anotherCell, bool newState)
{
	int anIndex = getIndex(aCell);
	int anotherIndex = getIndex(anotherCell);
	if (anIndex >= 0 && anotherIndex >= 0) {
		connMatrix[anIndex * overallSize + anotherIndex] = newState;
		return true;
	}
	return false;
}

void Maze::clear() {
	if (connMatrix != nullptr)
		delete[] connMatrix;
	overallSize = 0;
	connMatrix = nullptr;
	width = -1;
	height = -1;
	start = { 1, 1 };
	end = { height,width };
}

Cell Maze::getRandomNeighbor(Cell currCell)
{
	int newRow = currCell.row, newCol = currCell.col;
	string possibilities = "";

	// determine what possible directions neighbors can be: North, South, East or West
	// note that odds can be easily manipulated here by adding more of one letter
	// inappropriate letters are avoided (e.g., no North if currCell is at top row)
	if (currCell.row > 1)
		possibilities += "N";
	if (currCell.row < height)
		possibilities += "S";
	if (currCell.col > 1)
		possibilities += "W";
	if (currCell.col < width)
		possibilities += "E";

	// select random
	char selection = possibilities[rand() % possibilities.length()];

	// make adjustments based on selection 
	switch (selection) {
	case 'N': newRow--; break;
	case 'S': newRow++; break;
	case 'W': newCol--; break;
	case 'E': newCol++; break;
	}

	// return neighbor
	return { newRow, newCol };
}

std::ostream& operator<<(std::ostream& out, const Maze& aMaze)
{
	out << "Start: " << aMaze.start.row << " " << aMaze.start.col << endl;
	out << "End: " << aMaze.end.row << " " << aMaze.end.col << endl;
	out << "Map Begin: " << aMaze.width << " " << aMaze.height << endl;

	Cell cell1, cell2;
	int overallSize = aMaze.overallSize;
	for (int i = 0; i < overallSize; i++)
		for (int j = 0; j < overallSize; j++) {
			if (aMaze.connMatrix[i * overallSize + j]) {
				cell1 = aMaze.getCell(i);
				cell2 = aMaze.getCell(j);
				out << "\tConn: " << cell1.row << " " << cell1.col
					<< " " << cell2.row << " " << cell2.col << endl;
			}
		}

	out << "Map Finish:" << endl;
	return out;
}

void Maze::alter(AlterModes wantedMode)
{
	bool* oldConnMatrix = connMatrix; // hold the old matrix (don't delete yet)
	connMatrix = new bool[overallSize * overallSize](); // the () makes them all zero
	int maxRowIndex = height, maxColIndex = width;

	// if alteration requires swap of width and height, be careful.
	if (wantedMode == MIRROR_DIAGONAL || wantedMode == QUARTER_TURN) {
		swap(width, height);  // need to do this now so that it works later when I use connect() function
		maxRowIndex = width;
		maxColIndex = height;
	}

	Cell fromCell, toCell;
	Cell newFromCell = { -1,-1 }, newToCell = { -1,-1 };  // initialized to avoid warnings
	int oldFromIndex, oldToIndex;

	for (int fromRow = 1; fromRow <= maxRowIndex; fromRow++)
		for (int fromCol = 1; fromCol <= maxColIndex; fromCol++) {
			fromCell = { fromRow, fromCol };
			for (int toRow = 1; toRow <= maxRowIndex; toRow++)
				for (int toCol = 1; toCol <= maxColIndex; toCol++) {
					toCell = { toRow, toCol };
					oldFromIndex = fromCell.row * maxColIndex + fromCell.col;
					oldToIndex = toCell.row * maxColIndex + toCell.col;

					// if there was a connection in old matrix, make an equivalent connection in new matrix
					if (oldConnMatrix[oldFromIndex * overallSize + oldToIndex]) {
						switch (wantedMode) {
						case MIRROR_LEFT_RIGHT:
							newFromCell = { fromCell.row, width - fromCell.col + 1 };
							newToCell = { toCell.row, width - toCell.col + 1 };
							break;
						case MIRROR_UP_DOWN:
							newFromCell = { height - fromCell.row + 1, fromCell.col };
							newToCell = { height - toCell.row + 1, toCell.col };
							break;
						case QUARTER_TURN:
							newFromCell = { height - fromCell.col + 1, fromCell.row };
							newToCell = { height - toCell.col + 1, toCell.row };
							break;
						case MIRROR_DIAGONAL:
							newFromCell = { fromCell.col, fromCell.row };
							newToCell = { toCell.col, toCell.row };
							break;
						}
						connect(newFromCell, newToCell);
					}
				}
		}

	delete[] oldConnMatrix; // not sure if this works

}

void Maze::mirrorLeftRight()
{
	alter(MIRROR_LEFT_RIGHT);

	if (start.col != -1 && end.col != -1) {
		start.col = width - start.col + 1;
		end.col = width - end.col + 1;
	}
}


void Maze::mirrorUpDown()
{
	alter(MIRROR_UP_DOWN);

	if (start.row != -1 && end.row != -1) {
		start.row = height - start.row + 1;
		end.row = height - end.row + 1;
	}
}

void Maze::quarterTurn()
{
	alter(QUARTER_TURN);

	if (start.row != -1 && end.row != -1) {
		int newStartRow = height - start.col + 1;
		int newEndRow = height - end.col + 1;

		start = { newStartRow, start.row };
		end = { newEndRow , end.row };
	}
}

void Maze::mirrorOnDiagonal()
{
	alter(MIRROR_DIAGONAL);

	if (start.row != -1 && end.row != -1) {
		start = { start.col, start.row };
		end = { end.col, end.row };
	}
}

