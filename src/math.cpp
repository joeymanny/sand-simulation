/*
 * Defines implementations on things declared in math.h
 */

#include <cmath>
using namespace std;

#include "headers/math.h"

Vec2::Vec2(const double& x, const double& y) {
	set(x, y);
}
Vec2::Vec2() {
	set(0, 0);
}

Vec2 Vec2::fromMagnitudeDirection(const double& magnitude, const double& direction) {
	return Vec2(
		magnitude * cos(direction), // x
		magnitude * sin(direction)  // y
	);
}

void Vec2::set(const double& x, const double& y) {
	// Set values once so that they don't have to be calculated every time
	xPos = x;
	yPos = y;
	dir = atan2(yPos, xPos);
	mag = sqrt(pow(xPos, 2) + pow(yPos, 2));
	unitX = xPos / mag;
	unitY = yPos / mag;
}

const double& Vec2::x() const {
	return xPos;
}
const double& Vec2::y() const {
	return yPos;
}

const double& Vec2::magnitude() const {
	return mag;
}

const double& Vec2::direction() const {
	return dir;
}

Vec2 Vec2::unit() const {
	return Vec2(unitX, unitY);
}

double Vec2::dotProduct(const Vec2& rhs) const {
	return xPos * rhs.x() + yPos * rhs.y();
}

Vec2 Vec2::scale(const double& n) const {
	return Vec2(
		xPos * n,
		yPos * n
	);
}

void Vec2::scaleAssign(const double& n) {
	xPos *= n;
	yPos *= n;
}

Vec2 Vec2::add(Vec2 rhs) const {
	return {xPos + rhs.x(), yPos + rhs.y()};
}

void Vec2::addAssign(const Vec2& rhs) {
	set(xPos + rhs.x(), yPos + rhs.y());
}

Vec2 Vec2::negate() const {
	return {-xPos, -yPos};
}
void Vec2::negateAssign() {
	xPos = -xPos;
	yPos = -yPos;
}
