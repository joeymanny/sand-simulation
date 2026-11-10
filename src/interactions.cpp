/*
 * Implements the definitions in interactions.h
 */

#include "headers/interactions.h"

SandGrid::SandGrid(int screenWidth, int screenHeight) {
	the.resize(screenHeight / GRID_CELL_WIDTH);
	for (vector<vector<SandGrain>> row : the)
		row.resize(screenWidth / GRID_CELL_WIDTH);
}

// stuff needed
