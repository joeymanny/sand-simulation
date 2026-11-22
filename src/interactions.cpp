/*
 * Implements the definitions in interactions.h
 */

#include <SDL3/SDL_log.h>
#include <cmath>

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
void SandGrid::addParticle(SandGrain *grain) {
	Vec2 pos = grain->getPosition();
	the.at(pos.y() / GRID_CELL_WIDTH).at(pos.x() / GRID_CELL_WIDTH).push_back(*grain);
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

void screenBoundReflect(Vec2* startPos, Vec2* velocity, const double& radius, const double& dt) {
	Vec2 newPosition = startPos->add(velocity->scale(dtToSec(dt)));

	// X-axis reflection
	bool xReflect = false;
	double rightExtraX = newPosition.x() + radius - SCREEN_WIDTH;
	double leftExtraX = newPosition.x() - radius;
	double cutoffX = newPosition.x();
	double extraX = 0;
	if (leftExtraX < 0) {
		cutoffX = radius;
		extraX = leftExtraX;
		xReflect = true;
	} else if (rightExtraX > 0) {
		cutoffX = SCREEN_WIDTH - radius;
		extraX = rightExtraX;
		xReflect = true;
	}
	if (xReflect) {
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
		cutoffY = radius;
		extraY = topExtraY;
		yReflect = true;
	} else if (bottomExtraY > 0) {
		cutoffY = SCREEN_HEIGHT - radius;
		extraY = bottomExtraY;
		yReflect = true;
	}
	if (yReflect) {
		velocity->set(velocity->x(), -velocity->y());
		velocity->addAssign(velocity->scale(BOUNCE_VELOCITY_LOSS_MOD).negate());
	}

	// Combine reflections to update position
	startPos->set(cutoffX - extraX, cutoffY - extraY);
}

void screenBoundStop(Vec2* startPos, Vec2* velocity, const double& radius) {
	Vec2 relativeVector = (startPos->add(*velocity).add(startPos->negate()));
	double yDistance = SCREEN_HEIGHT - radius - startPos->y();
	double xDistance = 8 * (relativeVector.x() / relativeVector.y());

	velocity->set(0, 0);
	startPos->addAssign({xDistance, yDistance});
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
