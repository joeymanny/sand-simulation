/*
 * Implementations for the SandGrain class
 */

#include <SDL3/SDL_log.h>
#include <ctime>

#include "headers/sand.h"
#include "headers/common.h"
#include "headers/math.h"

SandGrain::SandGrain(const Vec2& startingPosition) {
	// Assign a unique identifier
	static unsigned long id = 0;
	identifier = id++;

	// Put starting values
	position = startingPosition;
	acceleration.set(0, gravityAccel);
}

unsigned long SandGrain::id() const {
	return identifier;
}

const Vec2& SandGrain::getPosition() const {
	return position;
}

void SandGrain::updateVelocity(SDL_Time dt) {
	// Update the position if the particle is not stationary
	if (!stationary) {
		velocity.addAssign(acceleration.scale(dtToSec(dt)));
		velocity.addAssign(velocity.scale(AIR_RESISTANCE_SUB_MOD).scale(dtToSec(dt)).negate());

		// Refresh velocity
		partialVelocity = velocity;
	}
	// REMEMBER: Once particles are put on top of other particles, make a way to turn stationary off with other particles start moving
}

void SandGrain::updatePosition(SDL_Time dt) {
	// Final updating of particle velocity
	position.addAssign(partialVelocity.scale(dtToSec(dt)));
}

void SandGrain::instantAccelerate(const Vec2& newVelocity) {
	velocity = newVelocity;
}

const Vec2& SandGrain::getVelocity() const {
	return velocity;
}
const Vec2& SandGrain::getPartialVelocity() const {
	return partialVelocity;
}

bool SandGrain::detectScreenBoundsCollision(const double& dt) const {
	// Assume the particle will update its position to p.
	Vec2 newPosition = position.add(partialVelocity.scale(dtToSec(dt)));
	// If p causes any pixel of the particle to be outside of the screen bounds,
	return newPosition.x() + RADIUS >= SCREEN_WIDTH || newPosition.x() - RADIUS < 0 || newPosition.y() + RADIUS > SCREEN_HEIGHT || newPosition.y() - RADIUS < 0;
}

void SandGrain::screenBoundReflect(const double& dt) {
	// Assume the particle will update its position to p.
	Vec2 newPosition = position.add(partialVelocity.scale(dtToSec(dt)));

	// X-axis reflection
	bool xReflect = false;
	double rightExtraX = newPosition.x() + RADIUS - SCREEN_WIDTH;
	double leftExtraX = newPosition.x() - RADIUS;
	double cutoffX = newPosition.x();
	double extraX = 0;
	if (leftExtraX < 0) {
		// If p clips on the left side,
		cutoffX = RADIUS;
		extraX = leftExtraX;
		xReflect = true;
	} else if (rightExtraX > 0) {
		// If p clips on the right side,
		cutoffX = SCREEN_WIDTH - RADIUS;
		extraX = rightExtraX;
		xReflect = true;
	}
	if (xReflect) {
		// Reflect the velocity and attenuate it
		velocity.set(-velocity.x(), velocity.y());
		velocity.addAssign(velocity.scale(BOUNCE_VELOCITY_LOSS_MOD).negate());
	}

	// Y-axis reflection
	bool yReflect = false;
	double bottomExtraY = newPosition.y() + RADIUS - SCREEN_HEIGHT;
	double topExtraY = newPosition.y() - RADIUS;
	double cutoffY = newPosition.y();
	double extraY = 0;
	if (topExtraY < 0) {
		// If p clips on the top,
		cutoffY = RADIUS;
		extraY = topExtraY;
		yReflect = true;
	} else if (bottomExtraY > 0) {
		// If p clips on the bottom,
		cutoffY = SCREEN_HEIGHT - RADIUS;
		extraY = bottomExtraY;
		yReflect = true;
	}
	if (yReflect) {
		// Reflect the velocity and attenuate it
		velocity.set(velocity.x(), -velocity.y());
		velocity.addAssign(velocity.scale(BOUNCE_VELOCITY_LOSS_MOD).negate());
	}

	// Combine reflections to update position and set new cut trajectory
	position.set(cutoffX, cutoffY);
	partialVelocity = Vec2(cutoffX - extraX, cutoffY - extraY).add(position.negate());
}

void SandGrain::screenBoundStop() {
	// Get the vector of the new position relative to the old position (Pn = (Pn-1 + V) - Pn-1) // This is just velocity
	Vec2 relativeVector = (position.add(partialVelocity).add(position.negate()));
	// Calculate the final x distance and y distance
	double yDistance = SCREEN_HEIGHT - RADIUS - position.y();
	double xDistance = 8 * (relativeVector.x() / relativeVector.y());

	// Zero the velocity and set the final positin
	velocity.set(0, 0);
	partialVelocity.set(0, 0);
	position.addAssign({xDistance, yDistance});
	stationary = true;
}

bool SandGrain::isSlowEnough() const {
	// TODO: incorporate angle into this, so that we don't get sudden stops (more angled downward, higher threshold)
	return velocity.magnitude() < VELOCITY_STOP_THRESHOLD;
}
