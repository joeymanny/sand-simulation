/*
 * Implementations of display.h are defined here
 */

#include "headers/display.h"
#include "headers/sand.h"
#include "headers/math.h"
#include "headers/common.h"

#include <SDL3/SDL_log.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
using namespace std;

constexpr int SLIDER_WIDTH = 100;
constexpr int SLIDER_HEIGHT = 4;
constexpr int SLIDER_CIRCLE_RADIUS = 8;

Slider::Slider(int inXPos, int inYPos, float inMin, float inMax, float inValue){
	xPos = inXPos;
	yPos = inYPos;
	minVal = inMin;
	maxVal = inMax;
	value = clamp(inValue, minVal, maxVal);
}
int Slider::x() {
	return xPos;
}
int Slider::y() {
	return yPos;
}
void Slider::setX(int newX) {
	xPos = newX;
}
void Slider::setY(int newY) {
	yPos = newY;
}
float Slider::getMin() const {
	return minVal;
}
void Slider::setMin(float newMin) {
	minVal = newMin;
}
float Slider::getMax() const{
	return maxVal;
}
void Slider::setMax(float newMax) {
	maxVal = newMax;
}
float Slider::getValue() const {
	return value;
}
void Slider::setValue(float newValue) {
	value = clamp(newValue, minVal, maxVal);
}
bool Slider::isClickWithin(int x, int y){
	
	int sliderX = ((float)xPos + ((float)SLIDER_WIDTH * ((value - minVal) / (maxVal - minVal)))) - SLIDER_CIRCLE_RADIUS;
	int sliderY = ((float)yPos + (SLIDER_HEIGHT / 2.0)) - SLIDER_CIRCLE_RADIUS;
	return x > sliderX && x < (sliderX + SLIDER_CIRCLE_RADIUS * 2.0) && y > sliderY && y < (sliderY + SLIDER_CIRCLE_RADIUS * 2.0);
}

void Slider::updateValFromMouseMotion(float absX, float deltaX){
	if (absX < xPos + SLIDER_WIDTH + SLIDER_CIRCLE_RADIUS && absX > xPos - SLIDER_CIRCLE_RADIUS){
		this->setValue(this->getValue() +  (deltaX / SLIDER_WIDTH) * (maxVal - minVal));
	}
}
// sliders are drawn starting at their position going SLIDER_WIDTH to the right
// they are a wide rectangle with a red circle drawn to indicate value
// don't place it too close to the right edge, it doesnt do any edge detection
void Slider::drawSlider(vector<unsigned int>* pixels){
	for (int row = yPos; row < yPos + SLIDER_HEIGHT; row++){
		for (int col = xPos; col < xPos + SLIDER_WIDTH; col++){
			setPixel(pixels, col, row, 0xFFFFFFFF);
		}
	}
	circle(
		pixels,
		Vec2((float)xPos + ((float)SLIDER_WIDTH * ((value - minVal) / (maxVal - minVal))), (float)yPos + (SLIDER_HEIGHT / 2.0)),
		SLIDER_CIRCLE_RADIUS,
		0xFF0000FF
	);
}

void setPixel(vector<unsigned int>* pixels, int pixelX, int pixelY, unsigned int color) {
	if (pixelY > 0 && pixelY < SCREEN_HEIGHT && pixelX > 0 && pixelX < SCREEN_WIDTH) {
		pixels->at(SCREEN_WIDTH * pixelY + pixelX) = color;
	}
}

void setRect(vector<unsigned int>* pixels, const Vec2& location, const Vec2& size, unsigned int color){
	// TODO do this is a less naive way. surely vectors have a way of setting a range of values
	// to the same value, right?
	for (int i = floor(location.x()); i < floor(location.x() + size.x()); i++){
		for (int j = floor(location.y()); j < floor(location.y() + size.y()); j++){
			setPixel(pixels, i, j, color);
		}
	}
}

void setSquare(vector<unsigned int>* pixels, const Vec2& location, float size, unsigned int color){
	setRect(pixels, location, {size, size}, color);
}

void circle(vector<unsigned int>* pixels, const Vec2& center, int radius, unsigned int color) {
	for (int row = center.x() - radius; row <= center.x() + radius; row++) {
		for (int col = center.y() - radius; col <= center.y() + radius; col++) {
			float dist = sqrt(pow(row - center.x(), 2) + pow(col - center.y(), 2));
			if (dist <= radius) {
				setPixel(pixels, row, col, color);
				// Border color and size
				// if (fabs(dist - radius) < 5)
				// 	setPixel(pixels, col, row, 0xFF0000FF);
			}
		}
	}
}


void drawText(vector<unsigned int>* pixels, const Vec2& location, float scale, unsigned int color, string text){
	float drawCursorX = location.x();
	float drawCursorY = location.y();
	for (char c : text){
		drawChar(pixels, {drawCursorX, drawCursorY}, scale, color, c);
		// one extra for padding
		drawCursorX += 6.0 * scale;
	}
}

// returns width of drawn character
//  5 factors wide by 7 factor tall
void drawChar(vector<unsigned int>* pixels, const Vec2& location, float scale, unsigned int color, char c){
	// TODO
	switch (c){
		case '1':
			// draw stem of the '1'
			setRect(pixels, {location.x() + 2.0 * scale, location.y()}, {scale, 7.0 * scale}, color);
			// draw the base of the '1'
			setRect(pixels, {location.x(), location.y() + 6.0 * scale}, {5.0 * scale, scale}, color);
			// draw the lil nose thingy
			setSquare(pixels, {location.x() + scale, location.y() + scale}, scale, color);
		break;
		case '.':
			setSquare(pixels, {location.x() + 2.0 * scale, location.y() + 6.0 * scale}, scale, color);
		break;
		default:
		break;
	}
}

