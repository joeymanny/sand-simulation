#pragma once
/*
 * This header file defines a grain of sand and how to modify its properties
 * All functions in here operate upon a single grain of sand
 */

#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_time.h>

#include "math.h"

// Sweet spot velocity stop threshold: no sudden cuts, no unnecessary velocity updates. DO NOT CHANGE
#define VELOCITY_STOP_THRESHOLD 38
// Default gravity that makes things look realistic
#define DEFAULT_GRAVITY 4096
// Default friction arbitrarily chosen
#define DEFAULT_FRICTION .5

// Defines a single grain of sand
class SandGrain {
	protected:
		// Particle properties
		// Partial velocity is the remaining velocity to traverse throughout the current frame, for collision purposes
		Vec2 position, velocity, partialVelocity, acceleration;
		// If the current particle is immobile. When setting to true, set velocity and partialVelocity to (0, 0)
		bool stationary = false;
		// Unique particle identifier for equality purposes
		unsigned long identifier;

	public:
		static constexpr double RADIUS = 20; // In pixels

		static double frictionMult;
		static double gravityAccel;

		// Constructor (do `SandGrain grain({x, y})` to initialize)
		SandGrain(const Vec2& initialPosition);
		// Intended to be ran every frame to update the velocity of the particle from the acceleration
		// Acceleration is per-second-per second, but our timeframe is the time between frames "dt" instead of 1 second
		// Refreshes partial velocity to be equal to velocity for collision-related purposes
		void updateVelocity(SDL_Time dt);
		// Intended to be ran every frame to update the position of the particle from the velocity
		// Velocity is per-second, but our timeframe is the time between frames "dt" instead of 1 second
		// This will update the position based on the partial velocity, as the final step in moving the particle,
		void updatePosition(SDL_Time dt);
		// Insantly accelerate the particle by adding `velocity` to the particle's velocity
		void instantAccelerate(const Vec2& velocity);

		// Get position
		const Vec2& getPosition() const;
		// Get velocity
		const Vec2& getVelocity() const;
		// Get partial velocity
		const Vec2& getPartialVelocity() const;

		// If the particle is considered "stationary"
		bool isStationary() const;

		// Return the particle's unique identifier
		unsigned long id() const;

		// If current velocity is slow enough
		bool isSlowEnough() const;

		// Single-particle collision detections

		// Detect screen collisions
		bool detectScreenBoundsCollision(const double& dt) const;
		// Reflect the particle after detecting a collision with the edges of the window
		void screenBoundReflect(const double& dt);
		// Stop the particle when the particle is slow enough to be considered "stopping", after a screen edge collision
		void screenBoundStop();

		friend bool collideParticles(SandGrain* a, SandGrain* b, SDL_Time dt);
};
