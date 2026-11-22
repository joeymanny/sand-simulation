/*
 * Implementations for the SandGrain class
 */

#include <SDL3/SDL_log.h>

#include "headers/sand.h"
#include "headers/math.h"
#include "headers/interactions.h"
#include "headers/common.h"

SandGrain::SandGrain(const Vec2& startingPosition) {
	position = startingPosition;
	acceleration.set(0, gravityAccel);
}

const Vec2& SandGrain::getPosition() const {
	return position;
}

void SandGrain::updateVelocity(SDL_Time dt) {
	velocity.addAssign(acceleration.scale(dtToSec(dt)));
}

void SandGrain::updatePosition(SDL_Time dt) {
	if (detectScreenBoundsCollision(position, velocity, RADIUS, dt)) {
		acceleration.set(0, 0);
		velocity.set(0, 0);
	} else
		position.addAssign(velocity.scale(dtToSec(dt)));
}

// add other stuff
