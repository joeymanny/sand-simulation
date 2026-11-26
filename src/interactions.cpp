/*
 * Implements the definitions in interactions.h
 */

#include <SDL3/SDL_log.h>
#include <cmath>
#include <algorithm>

#include "headers/interactions.h"
#include "headers/math.h"
#include "headers/common.h"

SandGrid::SandGrid(int screenWidth, int screenHeight) {
	// Divide up the screen into cells
	cellsX = screenWidth / GRID_CELL_WIDTH;
	cellsY = screenHeight / GRID_CELL_WIDTH;

	// Allocate memory for the cells
	the.resize(cellsY);
	for (vector<vector<SandGrain>>& row : the)
		row.resize(cellsX);
}

void SandGrid::addParticle(const Vec2& pos) {
	// Create a new particle in the required cell
	the.at(pos.y() / GRID_CELL_WIDTH).at(pos.x() / GRID_CELL_WIDTH).push_back(SandGrain(pos));
	sandCount++;
}
void SandGrid::addParticle(SandGrain* grain) {
	// Add an existing particle in the required cell
	Vec2 pos = grain->getPosition();
	the.at(pos.y() / GRID_CELL_WIDTH).at(pos.x() / GRID_CELL_WIDTH).push_back(*grain);
	sandCount++;
}

void SandGrid::updateGridPlacements() {
	// For each grid row,
	for (int row = 0; row < the.size(); row++)
		// For each grid col,
		for (int col = 0; col < the.at(row).size(); col++)
			// For each grain in the row+col cell,
			for (int grain = 0; grain < the.at(row).at(col).size();) {
				// Get the grid position the particle is supposed to be in
				int gridY = the[row][col][grain].getPosition().y() / GRID_CELL_WIDTH;
				int gridX = the[row][col][grain].getPosition().x() / GRID_CELL_WIDTH;
				// If the intended grid position mismatches the current grid position, and the intended grid position is valid,
				if ((gridY != row && gridY < cellsY && gridY >= 0) || (gridX != col && gridX < cellsX && gridY >= 0)) {
					// Move the grain to the intended grid position and erase the particle in the current grid
					SandGrain currentGrain = the[row][col][grain];
					auto it = the[row][col].cbegin() + grain;
					the[row][col].erase(it);
					the[gridY][gridX].push_back(currentGrain);
					// TODO: This step can be optimized by taking less memory
				} else
					// If no moves happen, increment the loop counter
					grain++;
			}
}

int SandGrid::getSandCount() const {
	return sandCount;
}

// stuff needed
void checkParticleCollision(SandGrain* a, SandGrain* b){
    Vec2 dPos = a->getPosition().add(b->getPosition().negate());
    Vec2 dVel = a->getVelocity().add(b->getVelocity().negate());

    double a1 = dVel.dotProduct(dVel);
    double b1 = 2* dPos.dotProduct(dVel);
    double c1 = dPos.dotProduct(dPos) - 4* SandGrain::RADIUS;

    if(pow(b1,2) - 4*a1*c1 >= 0 ||c1 <= 0)
        return;

    double time[2];
    time[0] = -b1 + sqrt(pow(b1,2) - 4*a1*c1);
    time[1] = -b1 - sqrt(pow(b1,2) - 4*a1*c1);
    int n = sizeof(time) / sizeof(time[0]);
    double t = *min_element(time, time+n);

    Vec2 o1 = a->getPosition().add(a->getVelocity().scale(t));
    Vec2 o2 = b->getPosition().add(b->getVelocity().scale(t));

    Vec2 nVec = o1.add(o2.negate()).scale(1/(2 * SandGrain::RADIUS));
    double speed1 = a->getVelocity().dotProduct(nVec);
    double speed2 = b->getVelocity().dotProduct(nVec);

    Vec2 NewGeneralVel1 = a->getVelocity().add(nVec.scale(speed2-speed1)).scale(1-BOUNCE_VELOCITY_LOSS_MOD);
    Vec2 NewGeneralVel2 = b->getVelocity().add(nVec.scale(speed1-speed2)).scale(1-BOUNCE_VELOCITY_LOSS_MOD);

    Vec2 NewPartialVel1 = NewGeneralVel1.scale(1-t);
    Vec2 NewPartialVel2 = NewGeneralVel2.scale(1-t);
}


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
