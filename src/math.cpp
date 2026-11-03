/*
 * Defines implementations on things declared in math.h
 */

#include <cmath>
using namespace std;

#include "headers/math.h"

Vec2 Vec2::fromMagnitudeDirection(double magnitude, double direction) {
	return {
		magnitude * cos(direction), // x
		magnitude * sin(direction)  // y
	};
}

double Vec2::magnitude() const {
	// Calculated by pythagorean theorem
	return sqrt(pow(x, 2) + pow(y, 2));
}

double Vec2::direction() const {
	// Calculated by atan2
	return atan2(y, x);
}
