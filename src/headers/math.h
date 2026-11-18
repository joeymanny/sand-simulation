#pragma once
/*
 * Defines special-purpose math functions and classes
 */

// Class that defines a two-dimensional vector with x- and y-components, like a position on a 2-dimensional plane
class Vec2 {
	private:
		double xPos, yPos, dir, mag, unitX, unitY;
	public:
		// Constructs a Vec2 from a given magnitude and direction
		static Vec2 fromMagnitudeDirection(double magnitude, double direciton);

		// Regular constructor
		Vec2(double x, double y);
		// Default constructor
		Vec2();

		// Set a new x and y
		void set(double x, double y);
		// Get x value
		double x() const;
		// Get y value
		double y() const;

		// Returns the length of the ray formed by the vector components
		double magnitude() const;
		// Returns the angle of the vector as if it were on the unit circle
		double direction() const;
		// Returns the unit vector of this vector's direction
		Vec2 unit() const;

		// Returns the dot product of this vector and the `rhs` vector
		double dotProduct(Vec2 rhs) const;
		// Returns the vector multiplied by some scalar `n`
		Vec2 scale(double n) const;
		// scale() but assigns the output to the current vector
		void scaleAssign(double n);
};
