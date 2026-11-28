/*
 * Implements the definitions in interactions.h
 */

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <cmath>

#include "headers/interactions.h"
#include "headers/math.h"
#include "headers/common.h"
#include "headers/sand.h"

template <class SandType> SandGrid<SandType>::SandGrid(int screenWidth, int screenHeight) {
	// Divide up the screen into cells
	cellsX = ceil((long double)screenWidth / GRID_CELL_WIDTH);
	cellsY = ceil((long double)screenHeight / GRID_CELL_WIDTH);

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
				int gridY = screenClamp(the[row][col][grain].getPosition().y(), true) / GRID_CELL_WIDTH;
				int gridX = screenClamp(the[row][col][grain].getPosition().x(), false) / GRID_CELL_WIDTH;
				// If the intended grid position mismatches the current grid position, and the intended grid position is valid,
				if ((gridY != row || gridX != col) && ((gridY < cellsY && gridY >= 0) && (gridX < cellsX && gridX >= 0))) {
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

template <class SandType> void SandGrid<SandType>::zero() {
	for (vector<vector<SandType>>& row : the)
		for (vector<SandType>& cell : row)
			cell = vector<SandType>();
}

template <class SandType> int SandGrid<SandType>::getGridX(double x) const {
	return screenClamp(x, false) / GRID_CELL_WIDTH;
}
template <class SandType> int SandGrid<SandType>::getGridY(double y) const {
	return screenClamp(y, true) / GRID_CELL_WIDTH;
}

// stuff needed

bool collideParticles(SandGrain* a, SandGrain* b, SDL_Time dt) {
	//checks that the particles exist and aren't pointing to the same particle
	if (a == nullptr || b == nullptr || a == b)
		return false;

	//find the change in time, the change in position and velocity
	//and the distance between the two centers if the objects were touching
	double deltaTime = dtToSec(dt);
	Vec2 dPos = a->getPosition().add(b->getPosition().negate());
	Vec2 dVel = a->getVelocity().add(b->getVelocity().negate());
	double radiusSum = 2 * SandGrain::RADIUS;

	//A is how fast the distance between them changes (A = 0 means no collision)
	//B is how fast they're moving towards / away from eachother (- closer, + further, no collision)
	//C is whether they are connected(- overlapping, + not touching, 0 touching the edge of the circles)
	double A = dVel.dotProduct(dVel);
	double B = 2 * dPos.dotProduct(dVel);
	double C = dPos.dotProduct(dPos) - radiusSum * radiusSum;

	double t = -1;

	//check if they start overlapped
	if (C <= 0) {
		//check if youve already started moving them so they don't touch
		if (dPos.dotProduct(dVel) >= 0)
			return false;
		t = 0; }
	else {
		//make sure they're going to hit
		if (A < 1e-10 || B >= 0)
			return false;

		//the smaller of the two numbers given by the disc is when they touch (smaller is when they enter, larger is when they'd exit if they passed through)
		//if negative they never touch
		//if not real they never touch
		//if 0 they touch but its tangential so no velocity
		double discriminant = B * B - 4 * A * C;
		if (discriminant < 0)
			return false;

		//do the quadratic formula to find t
		double sqrtD = sqrt(discriminant);
		double t1 = (-B - sqrtD) / (2 * A);

		if (t1 < 0 || t1 > deltaTime)
			return false;

		t = t1; }

	//find the points at which they collide
	Vec2 collisionA = a->getPosition().add(a->getVelocity().scale(t));
	Vec2 collisionB = b->getPosition().add(b->getVelocity().scale(t));

	//find their new positions and how far they are away from eachother after collision (should be 2r)
	Vec2 difference = collisionA.add(collisionB.negate());
	double distance = difference.magnitude();

	// Make sure that it updated correctly and is not NaN
	if (distance < 1e-10 || !isfinite(distance))
		return false;

	//find where the intersection is pointing and at what speed the particles are going
	Vec2 normal = difference.scale(1 / distance);
	double relativeNormalVelocity = dVel.dotProduct(normal);

	//initialize the variable that is going to be new velocity
	Vec2 newVelocityA = a->getVelocity();
	Vec2 newVelocityB = b->getVelocity();

	//find the ammount to be added along the vector connecting the 2 collisions and add it
	if (relativeNormalVelocity < -1e-9) {
		Vec2 impulse = normal.scale(-(1 + 2 * BOUNCE_VELOCITY_LOSS_MOD) * relativeNormalVelocity * 0.5);
		newVelocityA = newVelocityA.add(impulse);
		newVelocityB = newVelocityB.add(impulse.scale(-1)); }

	//make sure that they're not touching, if they are, move them
	//back far enough on the vector between their centers that they're not
	double overlap = radiusSum - distance;
	if (overlap > 0) {
		double slop = 0.01;
		double correctionAmount = (overlap + slop) * 0.5;
		Vec2 correction = normal.scale(correctionAmount);

		collisionA = collisionA.add(correction);
		collisionB = collisionB.add(correction.scale(-1)); }

	newVelocityA.scaleAssign(1 - SandGrain::frictionMult);
	newVelocityB.scaleAssign(1 - SandGrain::frictionMult);

	//set all the found values
	double remainingTime = deltaTime - t;

	if (!a->comfortablyStationary || newVelocityB.magnitude() > PARTICLE_COLL_VELOCITY_UNSETTLE_THRESHOLD) {
		a->comfortablyStationary = false;
		a->position = collisionA;
		a->velocity = newVelocityA.scale(1 - SandGrain::frictionMult);
		a->partialVelocity = newVelocityA.scale(remainingTime);
	}
	if (!b->comfortablyStationary || newVelocityA.magnitude() > PARTICLE_COLL_VELOCITY_UNSETTLE_THRESHOLD) {
		b->comfortablyStationary = false;
		b->position = collisionB;
		b->velocity = newVelocityB.scale(1 - SandGrain::frictionMult);
		b->partialVelocity = newVelocityB.scale(remainingTime);
	}

	// If a particle collides with an immobile particle and is slow enough, stop it!
	bool lowVelocityA = a->velocity.magnitude() < PARTICLE_COLL_VELOCITY_STOP_THRESHOLD;
	bool lowVelocityB = b->velocity.magnitude() < PARTICLE_COLL_VELOCITY_STOP_THRESHOLD;
	if (lowVelocityA && lowVelocityB && (a->stationary || b->stationary)) {
		a->velocity = {0, 0};
		a->partialVelocity = {0, 0};
		a->stationary = true;
		b->velocity = {0, 0};
		b->partialVelocity = {0, 0};
		b->stationary = true;
	} else {
		if (a->stationary)
			a->stationaryParticleCollided = true;
		if (b->stationary)
			b->stationaryParticleCollided = true;

		if (!a->comfortablyStationary)
			a->stationary = false;
		if (!b->comfortablyStationary)
			b->stationary = false;
	}

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
