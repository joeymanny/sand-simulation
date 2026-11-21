/*
 * Implementations for the SandGrain class
 */

#include "headers/sand.h"
#include "headers/math.h"

SandGrain::SandGrain(Vec2 startingPosition) {
	position = startingPosition;
	acceleration.set(0, gravityAccel);
}

Vec2 SandGrain::getPosition() const {
	return position;
}

// add other stuff
