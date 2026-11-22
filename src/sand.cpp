/*
 * Implementations for the SandGrain class
 */

#include "headers/sand.h"
#include "headers/math.h"
#include "headers/interactions.h"
#include <SDL3/SDL_log.h>

SandGrain::SandGrain(const Vec2& startingPosition) {
	position = startingPosition;
	acceleration.set(0, gravityAccel);
}

Vec2 SandGrain::getPosition() const {
	return position;
}

void SandGrain::updateVelocity(SDL_Time dt) {
	velocity.addAssign(acceleration.scale(dtToSec(dt)));
}

void SandGrain::updatePosition(SDL_Time dt) {
	position.addAssign(velocity.scale(dtToSec(dt)));
}

// add other stuff
