/*
  ===============================
 || XXX   XXXX        XX    XXXX||
 ||   X X XXXX XXXX X    XX XXXX||  Nestor Gomez
 ||XX X XsXXXX XXXX XXXXXXX XXXX||  Carnegie Mellon University
 ||XX   X XXX    XX XXXX   XXXXX||  Eng. Computation, 24-780-B
 ||XX XXX  XX X XXX XXXX XXXXXXX||  Prob Set 7
 ||XX XXXX XXXX XXX XXXX      XX||  Due Tues. Nov. 7, 2023
 ||XX XXXX X    XXX XXXX XXXX XX||
 ||XX XXXX X XXXXXX XXXX X      ||
 ||XX XXXXXX XXXXXX XXXX XXXXXX ||
 || X  XXXXX       XXXXX XXXXXX ||  Maze navigation using keyboard to
 || XX X     XX XX  XXXX X    X ||  create simple interactive game.
 || XX X XXX XX XXX XXX XXeXX X ||  
 || XX X   X XX X X XXX XXXXX XX||	The Entity is the object that
 ||    XXX X XX X X XXX       XX||	moves around the maze AND finds
 ||XXX     X    X   XXXXXXXXXXXX||	shortest path.
  ===============================

*/
#pragma once
//#include "Maze.h"  // not used to avoid circular includes

class Maze; // needed as holding spot for any class that includes Entity.h

class Entity {
protected:
	enum TorchShape { EVERYTHING, SQUARE, CIRCLE, OVAL, CLOUD};
	int locRow, locCol; // current location of entity
	int celebrateCounter; // used to control celebration animation
	bool traveled[Maze::MAX_MAZE_SIZE][Maze::MAX_MAZE_SIZE];  // note name change to more appropriate

	int torchSize = 5;
	TorchShape theTorchShape = EVERYTHING;

	Maze* myMaze = nullptr; 	

	bool visited[Maze::MAX_MAZE_SIZE][Maze::MAX_MAZE_SIZE];
	Cell parent[Maze::MAX_MAZE_SIZE][Maze::MAX_MAZE_SIZE] = {-1,-1}; // but need to really do this in reset also

public:

	// PS07  PS07  PS07  PS07  PS07
	// use BFS to find shortest path using visited and parent arrays
	void findShortestPath();

	bool pathIsAvailable();

	Entity(Maze* aMaze) { myMaze = aMaze; }
	int getRow() { return locRow; }
	int getCol() { return locCol; }

	void reset();

	// returns true if move was successful
	bool move(int direction);

	// for fun, this draws where the entity has been using traveled array
	void paintTraveledTrack();

	// draw the entity at current location
	// also draws torch
	void paint();

	// check if the current location is the end of the maze
	bool reachedGoal();

	// do a little animation while the entity is at the end of the maze
	void celebrateGoal();

	// playing around with hiding the map and only showing what can be
	// seen using a "torch"
	int getTorchSize() { return torchSize; }
	void disableTorch() { theTorchShape = EVERYTHING; }
	void increaseTorch() { torchSize++; }
	void decreaseTorch() { if (torchSize > 1) torchSize--; }
	void toggleTorchShape() {
		theTorchShape = (TorchShape)((theTorchShape + 1) % 3);
	}

	void setMaze(Maze& aMaze) { myMaze = &aMaze; };

	// erases visited and parent arrays
	void clearShortestPath();

	// marks all the cells that were visited during last shortest path search
	void paintSearchedCells();

	void findShortestPathAstar();
	int heuristic(Cell aCell);
};
