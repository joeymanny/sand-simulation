/*
 * Implementations for the SandGrain class
 */

#include <SDL3/SDL_log.h>

#include "headers/sand.h"
#include "headers/common.h"
#include "headers/math.h"
#include "headers/interactions.h"

#define VELOCITY_STOP_THRESHOLD 200

SandGrain::SandGrain(const Vec2& startingPosition) {
	position = startingPosition;
	acceleration.set(0, gravityAccel);
}

const Vec2& SandGrain::getPosition() const {
	return position;
}

void SandGrain::updateVelocity(SDL_Time dt) {
	if (!stationary) {
		velocity.addAssign(acceleration.scale(dtToSec(dt)));
		velocity.addAssign(velocity.scale(AIR_RESISTANCE_SUB_MOD).scale(dtToSec(dt)).negate());
	}
}

void SandGrain::updatePosition(SDL_Time dt) {
	// SDL_Log("Velocity: (%lf, %lf)", velocity.x(), velocity.y());
	if (detectScreenBoundsCollision(position, velocity, RADIUS, dt))
		if (velocity.magnitude() < VELOCITY_STOP_THRESHOLD) {
			screenBoundStop(&position, &velocity, RADIUS);
			stationary = true;
		} else
			screenBoundReflect(&position, &velocity, RADIUS, dt);
	else
		position.addAssign(velocity.scale(dtToSec(dt)));
}

void SandGrain::instantAccelerate(const Vec2& newVelocity) {
	velocity = newVelocity;
}

// add other stuff
