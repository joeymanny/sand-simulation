/*
 * Implementations for the SandGrain class
 */

#include "headers/sand.h"
#include "headers/math.h"

SandGrain::SandGrain(Vec2 startingPosition) {
	position = startingPosition;
	acceleration = {0, GRAVITY_ACCEL};
}

// add other stuff
