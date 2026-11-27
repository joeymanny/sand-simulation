/*
 * Implements the definitions in interactions.h
 */

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <cmath>

#include "headers/interactions.h"
#include "headers/math.h"
#include "headers/common.h"

template <class SandType> SandGrid<SandType>::SandGrid(int screenWidth, int screenHeight) {
	// Divide up the screen into cells
	cellsX = screenWidth / GRID_CELL_WIDTH;
	cellsY = screenHeight / GRID_CELL_WIDTH;

	// Allocate memory for the cells
	the.resize(cellsY);
	for (vector<vector<SandType>>& row : the)
		row.resize(cellsX);
}

int screenClamp(double positionComponent, bool isY) {
	int limit = isY ? SCREEN_HEIGHT : SCREEN_WIDTH;
	return max(0, min((int)positionComponent, limit - 1));
}

template <> void SandGrid<SandGrain>::addParticle(const Vec2& pos) {
	// Create a new particle in the required cell
	the.at(screenClamp(pos.y(), true) / GRID_CELL_WIDTH).at(screenClamp(pos.x(), false) / GRID_CELL_WIDTH).push_back(SandGrain(pos));
	sandCount++;
}
template <> void SandGrid<SandGrain>::addParticle(const SandGrain* grain) { // Whole form
	// Add an existing particle in the required cell
	Vec2 pos = grain->getPosition();
	the.at(screenClamp(pos.y(), true) / GRID_CELL_WIDTH).at(screenClamp(pos.x(), false) / GRID_CELL_WIDTH).push_back(*grain);
	sandCount++;
}
template <> void SandGrid<SandGrain*>::addParticle(SandGrain* const* grain) { // Pointer form (VERY weird definition)
	// Add an existing particle in the required cell
	Vec2 pos = (*grain)->getPosition();
	the.at(screenClamp(pos.y(), true) / GRID_CELL_WIDTH).at(screenClamp(pos.x(), false) / GRID_CELL_WIDTH).push_back(*grain);
	sandCount++;
}

template <> void SandGrid<SandGrain>::updateGridPlacements() {
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

template <class SandType> int SandGrid<SandType>::getSandCount() const {
	return sandCount;
}

// stuff needed

bool collideParticles(SandGrain* a, SandGrain* b, SDL_Time dt) {
    if (a == nullptr || b == nullptr || a == b)
        return false;

    double deltaTime = dtToSec(dt);

    Vec2 dPos = a->getPosition().add(b->getPosition().negate());
    Vec2 dVel = a->getVelocity().add(b->getVelocity().negate());

    double radiusSum = 2 * SandGrain::RADIUS;

    double A = dVel.dotProduct(dVel);
    double B = 2 * dPos.dotProduct(dVel);
    double C = dPos.dotProduct(dPos) - radiusSum * radiusSum;

    double t = -1;

    if (C <= 0) {
        t = 0;
        return 0; }
    else {
        if (A < 1e-10 || B >= 0)
            return false;

        double discriminant = B * B - 4 * A * C;
        if (discriminant < 0)
            return false;

        double sqrtD = sqrt(discriminant);
        double t1 = (-B - sqrtD) / (2 * A);

        if (t1 < 0 || t1 > deltaTime)
            return false;

        t = t1; }

    Vec2 collisionA = a->getPosition().add(a->getVelocity().scale(t));
    Vec2 collisionB = b->getPosition().add(b->getVelocity().scale(t));
    Vec2 difference = collisionA.add(collisionB.negate());

    double distance = difference.magnitude();

    // Make sure that it updated correctly and is not NaN
    if (distance < 1e-10 || !isfinite(distance))
        return false;

    Vec2 normal = difference.scale(1 / distance);

    double relativeNormalVelocity = dVel.dotProduct(normal);

    Vec2 newVelocityA = a->getVelocity();
    Vec2 newVelocityB = b->getVelocity();

    if (relativeNormalVelocity < -1e-9) {
        Vec2 impulse = normal.scale(-(1 + 2 * BOUNCE_VELOCITY_LOSS_MOD) * relativeNormalVelocity * 0.5);

        newVelocityA = newVelocityA.add(impulse);
        newVelocityB = newVelocityB.add(impulse.scale(-1)); }

    double overlap = radiusSum - distance;

    if (overlap > 0) {
        double slop = 0.01;
        double correctionAmount = (overlap + slop) * 0.5;
        Vec2 correction = normal.scale(correctionAmount);

        collisionA = collisionA.add(correction);
        collisionB = collisionB.add(correction.scale(-1)); }

    a->position = collisionA;
    b->position = collisionB;

    a->velocity = newVelocityA;
    b->velocity = newVelocityB;

    double remainingTime = deltaTime - t;

    a->partialVelocity = newVelocityA.scale(remainingTime);
    b->partialVelocity = newVelocityB.scale(remainingTime);

    return true;
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

// Explicit instantiation
template class SandGrid<SandGrain>;
template class SandGrid<SandGrain*>;
