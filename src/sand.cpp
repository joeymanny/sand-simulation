/*
 * Implementations for the SandGrain class
 */

#include <SDL3/SDL_log.h>
#include <cmath>
#include <ctime>
#include <algorithm>

#include "headers/sand.h"
#include "headers/common.h"
#include "headers/math.h"

double SandGrain::frictionMult = DEFAULT_FRICTION;
double SandGrain::gravityAccel = DEFAULT_GRAVITY;

SandGrain::SandGrain(const Vec2& startingPosition) {
	// Assign a unique identifier
	static unsigned long id = 0;
	identifier = id++;

	// Put starting value
	position = startingPosition;
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
		velocity.addAssign(Vec2(0, gravityAccel).scale(dtToSec(dt)));
		velocity.addAssign(velocity.scale(AIR_RESISTANCE_SUB_MOD).scale(dtToSec(dt)).negate());

		// Refresh velocity
		partialVelocity = velocity;
	}
	// REMEMBER: Once particles are put on top of other particles, make a way to turn stationary off with other particles start moving

	// At this point this function is effectively a state resetter too
	frameStartPosition = position;
	stationaryParticleCollided = false;
}

void SandGrain::updatePosition(SDL_Time dt) {
	// Final updating of particle velocity
	position.addAssign(partialVelocity.scale(dtToSec(dt)));
	// Clamp to be within screen
	position = {std::clamp(position.x(), RADIUS, SCREEN_WIDTH - RADIUS), std::clamp(position.y(), RADIUS, SCREEN_HEIGHT - RADIUS)};

	// At this point this function is effectifely a frame finalizer
	// This determines if a particle should settle based on being collided with despite having been stationary
	if (stationaryParticleCollided && position.add(frameStartPosition.negate()).magnitude() < DISPLACEMENT_SETTLE_THRESHOLD) {
		position = frameStartPosition;
		stationary = true;
		comfortablyStationary = true;
		velocity = {0, 0};
	}
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
bool SandGrain::isStationary() const {
	return stationary;
}
bool SandGrain::isComfortablyStationary() const {
	return comfortablyStationary;
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
		velocity.addAssign(velocity.scale(BOUNCE_VELOCITY_LOSS_MOD * angleMod(velocity.direction(), true, 0.5)).negate());
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
		velocity.addAssign(velocity.scale(BOUNCE_VELOCITY_LOSS_MOD * angleMod(velocity.direction(), false, 2)).negate());
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
	double xDistance = yDistance * (relativeVector.x() / relativeVector.y());

	// Zero the velocity and set the final positin
	velocity.set(0, 0);
	partialVelocity.set(0, 0);
	position.addAssign({xDistance, yDistance});
	stationary = true;
}

bool SandGrain::isSlowEnough() const {
	// Less steep downward angle = lower velocity stop threshold
	return velocity.magnitude() < VELOCITY_STOP_THRESHOLD * angleMod(velocity.direction(), false, 5);
}
