/*
 * Implements the definitions in interactions.h
 */

#include <SDL3/SDL_log.h>

#include "headers/interactions.h"
#include "headers/math.h"
#include "headers/common.h"

SandGrid::SandGrid(int screenWidth, int screenHeight) {
	cellsX = screenWidth / GRID_CELL_WIDTH;
	cellsY = screenHeight / GRID_CELL_WIDTH;

	the.resize(cellsY);
	for (vector<vector<SandGrain>>& row : the)
		row.resize(cellsX);
}

void SandGrid::addParticle(const Vec2& pos) {
	the.at(pos.y() / GRID_CELL_WIDTH).at(pos.x() / GRID_CELL_WIDTH).push_back(SandGrain(pos));
	sandCount++;
}

void SandGrid::updateGridPlacements() {
	for (int row = 0; row < the.size(); row++)
		for (int col = 0; col < the.at(row).size(); col++)
			for (int grain = 0; grain < the.at(row).at(col).size();) {
				int gridY = the[row][col][grain].getPosition().y() / GRID_CELL_WIDTH;
				int gridX = the[row][col][grain].getPosition().x() / GRID_CELL_WIDTH;
				if ((gridY != row && gridY < cellsY && gridY >= 0) || (gridX != col && gridX < cellsX && gridY >= 0)) {
					SandGrain currentGrain = the[row][col][grain];
					auto it = the[row][col].cbegin() + grain;
					the[row][col].erase(it);
					the[gridY][gridX].push_back(currentGrain);
				} else
					grain++;
			}
}

int SandGrid::getSandCount() const {
	return sandCount;
}

bool detectScreenBoundsCollision(const Vec2& startPos, const Vec2& velocity, const double& radius, const double& dt) {
	Vec2 newPosition = startPos.add(velocity.scale(dtToSec(dt)));
	return newPosition.x() + radius >= SCREEN_WIDTH || newPosition.x() - radius < 0 || newPosition.y() + radius > SCREEN_HEIGHT || newPosition.y() - radius < 0;
}

// stuff needed

// 1. Find time at which the particles interact (between 0 and 1 tick)
//      1a. Relative Velocity = (ending - starting pos 1) - (ending - starting pos 2)
//      1b. Relative Pos = starting pos 1 - starting pos 2
//      1c. var1 = dot (relative velocity, realative velocity)
//      1d. var2 = 2 * dot(relative pos 1, relative velcoity)
//      1e. var3 = dot(relative pos 1, relative pos 1) - 4 * pow(radius,2)
//      1f. Time = min((-var2 - sqrt(pow(var2,2) [+/-] 4 * var1 * var3)) / (2 * var1))
//      Note: if time is between 0 and 1, the particles do not interact
// 2. Find the point at which they interact
//      2a. Point of Collision for particle 1: starting pt 1 + Time * (ending pt 1 - starting pt 1)
//      2b. Point of Collision for particle 2: starting pt 2 + Time * (ending pt 2 - starting pt 2)
// 3. Find Ending Position after Collision
//      3a. Relative Normal Vector = (Relative pos + Relative Velocity * Time) / (2* Radius)
//      3b. Difference in Velocities = Velocity of 1 - Velocity of 2
//      3c. New Velociy of 1 = Velocity of 1 - dot(dot(diff of velo, relative normal), relative normal)
//      3d. New Velociy of 2 = Velocity of 2 + dot(dot(diff of velo, relative normal), relative normal)
//      3e. Ending Pos 1 = Velocity of 1 * t + (1 - friction) * (1 - Time) * New Velocity of 1
//      3e. Ending Pos 2 = Velocity of 2 * t + (1 - friction) * (1 - Time) * New Velocity of 2
