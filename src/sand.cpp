/*
 * Implementations for the SandGrain class
 */

#include <SDL3/SDL_log.h>

#include "headers/sand.h"
#include "headers/common.h"
#include "headers/math.h"
#include "headers/interactions.h"

#define VELOCITY_STOP_THRESHOLD 100

SandGrain::SandGrain(const Vec2& startingPosition) {
	position = startingPosition;
	acceleration.set(0, gravityAccel);
}

const Vec2& SandGrain::getPosition() const {
	return position;
}

void SandGrain::updateVelocity(SDL_Time dt) {
	// Update the position if the particle is not stationary
	if (!stationary) {
		velocity.addAssign(acceleration.scale(dtToSec(dt)));
		velocity.addAssign(velocity.scale(AIR_RESISTANCE_SUB_MOD).scale(dtToSec(dt)).negate());
	}
	// REMEMBER: Once particles are put on top of other particles, make a way to turn stationary off with other particles start moving
}

void SandGrain::updatePosition(SDL_Time dt) {
	// MAYBE: Particle collision detection here, both collision detectors turn on a "collided" flag that stops the default position updater,
	// 		particle collision updater returns a new trajectory to then be calculated by a screen bounds collision detector.
	// 		If things break, do this many times until a particle is freed

	// Detects of there is a collision between the particle and a screen bound
	if (detectScreenBoundsCollision(position, velocity, RADIUS, dt))
		// If the particle is effectively immobile, structurallly stop it
		if (velocity.magnitude() < VELOCITY_STOP_THRESHOLD) {
			screenBoundStop(&position, &velocity, RADIUS);
			stationary = true;
		} else
			// Reflect the particle if it is going fast enough
			screenBoundReflect(&position, &velocity, RADIUS, dt);
	else
		// If no collisions, update the position like a good velocity you are
		position.addAssign(velocity.scale(dtToSec(dt)));
}

void SandGrain::instantAccelerate(const Vec2& newVelocity) {
	velocity = newVelocity;
}

// add other stuff
