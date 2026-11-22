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
using namespace std;

constexpr int SLIDER_WIDTH = 100;
constexpr int SLIDER_HEIGHT = 4;
constexpr int SLIDER_CIRCLE_RADIUS = 6;

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
	if (absX < xPos + SLIDER_WIDTH && absX > xPos){
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
