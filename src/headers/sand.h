#pragma once
/*
 * This header file defines a grain of sand and how to modify its properties
 * All functions in here operate upon a single grain of sand
 */

#include <SDL3/SDL_time.h>

#include "math.h"

// Defines a single grain of sand
class SandGrain {
	private:
		// Constants
		const double RADIUS = 1;
		const double FRICTION_MULT = 0.6;
		const double GRAVITY = -9.81;
		const double GRAVITY_ACCEL = GRAVITY;

		// For sliders: Bounds of...
		// FRICTION_MULT -> [.2,.8]					  	   NOT FINAL VALUES
		// GRAVITY_ACCEL -> [1 * GRAVITY, 2 * GRAVITY]     NOT FINAL VALUES

		// Particle properties
		Vec2 position, velocity, acceleration;

	public:
		// Constructor (do `SandGrain grain({x, y})` to initialize)
		SandGrain(Vec2 initialPosition);
		// Intended to be ran every frame to update the velocity of the particle from the acceleration
		// Acceleration is per-second-per second, but our timeframe is the time between frames "dt" instead of 1 second
		void updateVelocity(SDL_Time dt);
		// Intended to be ran every frame to update the position of the particle from the velocity
		// Velocity is per-second, but our timeframe is the time between frames "dt" instead of 1 second
		void updatePosition(SDL_Time dt);
		// Insantly accelerate the particle by adding `velocity` to the particle's velocity
		void instantAccelerate(Vec2 velocity);

};
