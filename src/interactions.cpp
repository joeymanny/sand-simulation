/*
 * Implements the definitions in interactions.h
 */

#include <SDL3/SDL_log.h>
#include <cmath>

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

bool detectScreenBoundsCollision(const Vec2& startPos, const Vec2& velocity, const double& radius, const double& dt) {
	// Assume the particle will update its position to p.
	Vec2 newPosition = startPos.add(velocity.scale(dtToSec(dt)));
	// If p causes any pixel of the particle to be outside of the screen bounds,
	return newPosition.x() + radius >= SCREEN_WIDTH || newPosition.x() - radius < 0 || newPosition.y() + radius > SCREEN_HEIGHT || newPosition.y() - radius < 0;
}

void screenBoundReflect(Vec2* startPos, Vec2* velocity, const double& radius, const double& dt) {
	// Assume the particle will update its position to p.
	Vec2 newPosition = startPos->add(velocity->scale(dtToSec(dt)));

	// X-axis reflection
	bool xReflect = false;
	double rightExtraX = newPosition.x() + radius - SCREEN_WIDTH;
	double leftExtraX = newPosition.x() - radius;
	double cutoffX = newPosition.x();
	double extraX = 0;
	if (leftExtraX < 0) {
		// If p clips on the left side,
		cutoffX = radius;
		extraX = leftExtraX;
		xReflect = true;
	} else if (rightExtraX > 0) {
		// If p clips on the right side,
		cutoffX = SCREEN_WIDTH - radius;
		extraX = rightExtraX;
		xReflect = true;
	}
	if (xReflect) {
		// Reflect the velocity and attenuate it
		velocity->set(-velocity->x(), velocity->y());
		velocity->addAssign(velocity->scale(BOUNCE_VELOCITY_LOSS_MOD).negate());
	}

	// Y-axis reflection
	bool yReflect = false;
	double bottomExtraY = newPosition.y() + radius - SCREEN_HEIGHT;
	double topExtraY = newPosition.y() - radius;
	double cutoffY = newPosition.y();
	double extraY = 0;
	if (topExtraY < 0) {
		// If p clips on the top,
		cutoffY = radius;
		extraY = topExtraY;
		yReflect = true;
	} else if (bottomExtraY > 0) {
		// If p clips on the bottom,
		cutoffY = SCREEN_HEIGHT - radius;
		extraY = bottomExtraY;
		yReflect = true;
	}
	if (yReflect) {
		// Reflect the velocity and attenuate it
		velocity->set(velocity->x(), -velocity->y());
		velocity->addAssign(velocity->scale(BOUNCE_VELOCITY_LOSS_MOD).negate());
	}

	// Combine reflections to update position
	startPos->set(cutoffX - extraX, cutoffY - extraY);
}

void screenBoundStop(Vec2* startPos, Vec2* velocity, const double& radius) {
	// Get the vector of the new position relative to the old position (Pn = (Pn-1 + V) - Pn-1) // This is just velocity
	Vec2 relativeVector = (startPos->add(*velocity).add(startPos->negate()));
	// Calculate the final x distance and y distance
	double yDistance = SCREEN_HEIGHT - radius - startPos->y();
	double xDistance = 8 * (relativeVector.x() / relativeVector.y());

	// Zero the velocity and set the final positin
	velocity->set(0, 0);
	startPos->addAssign({xDistance, yDistance});
}

// stuff needed

void collideParticles(SandGrain* a, SandGrain* b) {
	// TODO: CODE THIS
	// Andrew: The code can go into this function. Whatever you do, make sure the positions of a and b change to where their
	//     collision are detected, and their velocity is what the new trajectory will be after the collision.
	// I can do the algorithms of how this function is used
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
