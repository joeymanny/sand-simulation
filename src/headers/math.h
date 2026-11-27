#pragma once
/*
 * Defines special-purpose math functions and classes
 */

// Convert nanoseconds dt to seconds dt
#define dtToSec(dt) ((double)dt / 1000000000)

// pi
#define PI 3.141592653589793
inline constexpr double HALF_PI = PI/2;

// Class that defines a two-dimensional vector with x- and y-components, like a position on a 2-dimensional plane
class Vec2 {
	private:
		// TODO: Test if automatically claculating direction, magnitude, and unit is better than only calculating if needed
		double xPos, yPos, dir, mag, unitX, unitY;
	public:
		// Constructs a Vec2 from a given magnitude and direction
		// QUESTION: Is this needed?
		static Vec2 fromMagnitudeDirection(const double& magnitude, const double& direciton);

		// Regular constructor
		Vec2(const double& x, const double& y);
		// Default constructor
		Vec2();

		// Set a new x and y
		void set(const double& x, const double& y);
		// Get x value
		const double& x() const;
		// Get y value
		const double& y() const;

		// Returns the length of the ray formed by the vector components
		const double& magnitude() const;
		// Returns the angle of the vector as if it were on the unit circle
		const double& direction() const;
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
		// -vec
		Vec2 negate() const;
		// -vec but modifies
		void negateAssign();
};

// Provides a modifier (float 0-1) based on the angle of something.
// If `side` is false, 1 will be achieved for completely vertical angles
// If `side` is true, 1 will be achieved for completely horizontal angles
double angleMod(double radians, bool side, double steepness);
