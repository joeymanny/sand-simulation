#pragma once
/*
 * Defines special-purpose math functions and classes
 */

// Class that defines a two-dimensional vector with x- and y-components, like a position on a 2-dimensional plane
class Vec2 {
	public:
		double x, y;

		// Constructs a Vec2 from a given magnitude and direction
		static Vec2 fromMagnitudeDirection(double magnitude, double direciton);

		// Returns the length of the ray formed by the vector components
		double magnitude() const;
		// Returns the angle of the vector as if it were on the unit circle
		double direction() const;
};
