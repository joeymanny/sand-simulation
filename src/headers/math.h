#pragma once
/*
 * Defines special-purpose math functions and classes
 */

#define dtToSec(dt) ((double)dt / 1000000000)

// Class that defines a two-dimensional vector with x- and y-components, like a position on a 2-dimensional plane
class Vec2 {
	private:
		double xPos, yPos, dir, mag, unitX, unitY;
	public:
		// Constructs a Vec2 from a given magnitude and direction
		static Vec2 fromMagnitudeDirection(const double& magnitude, const double& direciton);

		// Regular constructor
		Vec2(const double& x, const double& y);
		// Default constructor
		Vec2();

		// Set a new x and y
		void set(const double& x, const double& y);
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
		double dotProduct(const Vec2& rhs) const;
		// Returns the vector multiplied by some scalar `n`
		Vec2 scale(const double& n) const;
		// scale() but assigns the output to the current vector
		void scaleAssign(const double& n);
		// vec1 + vec2
		Vec2 add(const Vec2 rhs) const;
		// vec1 += vec2;
		void addAssign(const Vec2& rhs);
};
