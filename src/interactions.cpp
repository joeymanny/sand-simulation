/*
 * Implements the definitions in interactions.h
 */

#include "headers/interactions.h"
#include <SDL3/SDL_log.h>

SandGrid::SandGrid(int screenWidth, int screenHeight) {
	the.resize(screenHeight / GRID_CELL_WIDTH);
	for (vector<vector<SandGrain>>& row : the)
		row.resize(screenWidth / GRID_CELL_WIDTH);
}

void SandGrid::addParticle(Vec2 pos) {
	the.at(pos.y() / GRID_CELL_WIDTH).at(pos.x() / GRID_CELL_WIDTH).push_back(SandGrain(pos));
}

void SandGrid::updateGridPlacements() {
	for (int row = 0; row < the.size(); row++)
		for (int col = 0; col < the.at(row).size(); col++)
			for (int grain = 0; grain < the.at(row).at(col).size();) {
				int gridY = the[row][col][grain].getPosition().y() / GRID_CELL_WIDTH;
				int gridX = the[row][col][grain].getPosition().x() / GRID_CELL_WIDTH;
				if (gridY == row && gridX == col) {
					SandGrain currentGrain = the[row][col][grain];
					auto it = the[row][col].cbegin() + grain;
					the[row][col].erase(it);
					the[gridY][gridX].push_back(currentGrain);
				} else
					grain++;
			}
}

// stuff needed
