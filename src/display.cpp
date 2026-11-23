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
	if (absX < xPos + SLIDER_WIDTH + SLIDER_CIRCLE_RADIUS + SLIDER_BOUNDS_GRACE && absX > xPos - SLIDER_CIRCLE_RADIUS - SLIDER_BOUNDS_GRACE){
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
		case '2':
			// hookey bit at the left
			setSquare(pixels, {location.x(), location.y() + scale}, scale, color);
			// top flat bit
			setRect(pixels, {location.x() + scale, location.y()}, {scale * 3.0, scale}, color);
			// right part of the curve
			setRect(pixels, {location.x() + scale * 4.0, location.y() + scale}, {scale, scale * 2.0}, color);
			// middle bit
			setRect(pixels, {location.x() + 2.0 * scale, location.y() + scale * 3.0}, {scale * 2.0, scale}, color);
			// connecting middle bit and base
			setSquare(pixels, {location.x() + scale, location.y() + scale * 4.0}, scale, color);
			// connecting previous bit to base
			setSquare(pixels, {location.x(), location.y() + scale * 5.0}, scale, color);
			// base
			setRect(pixels, {location.x(), location.y() + 6.0 * scale}, {scale * 5.0, scale}, color);
			// tail
			setSquare(pixels, {location.x() + 4.0 * scale, location.y() + scale * 5.0}, scale, color);
		break;
		case '3':
			// upper hookey bit at left
			setSquare(pixels, {location.x(), location.y() + scale}, scale, color);
			// top flat bit
			setRect(pixels, {location.x() + scale, location.y()}, {scale * 3.0, scale}, color);
			// right part of upper hook
			setRect(pixels, {location.x() + 4.0 * scale, location.y() + scale}, {scale, 2.0 * scale}, color);
			// middle bit
			setRect(pixels, {location.x() + 2.0 * scale, location.y() + 3.0 * scale}, {2.0 * scale, scale}, color);
			// right part of lower hook
			setRect(pixels, {location.x() + 4.0 * scale, location.y() + scale * 4.0}, {scale, scale * 2.0}, color);
			// bottom flat bit
			setRect(pixels, {location.x() + scale, location.y() + 6.0 * scale}, { scale * 3.0, scale}, color);
			// lower hookey bit at left
			setSquare(pixels, {location.x(), location.y() + 5.0 * scale}, scale, color);
			
		break;
		case '4':
			// the diagonal bit
			setSquare(pixels, {location.x() + 0.0 * scale, location.y() + 3.0 * scale}, scale, color);
			setSquare(pixels, {location.x() + 1.0 * scale, location.y() + 2.0 * scale}, scale, color);
			setSquare(pixels, {location.x() + 2.0 * scale, location.y() + 1.0 * scale}, scale, color);
			setSquare(pixels, {location.x() + 3.0 * scale, location.y() + 0.0 * scale}, scale, color);
			// the lower bar
			setRect(pixels, {location.x(), location.y() + 4.0 * scale}, {scale * 4.0, scale}, color);
			// the stem
			setRect(pixels, {location.x() + 4.0 * scale, location.y()}, {scale, scale * 7}, color);
		break;
		case '5':
			// top flat bit
			setRect(pixels, {location.x(), location.y()}, {scale * 5, scale}, color);
			// left step under flat bit
			setRect(pixels, {location.x(), location.y() + scale,}, {scale, scale * 2.0}, color);
			// top flat part of curve
			setRect(pixels, {location.x() + scale, location.y() + 2.0 * scale}, {3.0 * scale, scale}, color);
			// right part of curve
			setRect(pixels, {location.x() + 4.0 * scale, location.y() + 3.0 * scale}, {scale, 3.0 * scale}, color);
			// lower flat bit
			setRect(pixels, {location.x() + scale, location.y() + 6.0 *scale}, {3.0 * scale, scale}, color);
			// the hookey bit at the back
			setSquare(pixels, {location.x(), location.y() + 5.0 * scale}, scale, color);
			
		break;
		case '6':
			// top small flat bit
			setRect(pixels, {location.x() + scale * 2, location.y()}, {scale * 2.0, scale}, color);
			// connecting bit at upper left
			setSquare(pixels,{location.x() + scale, location.y() + scale}, scale, color);
			// back of the 6
			setRect(pixels, {location.x(), location.y() + 2.0 * scale}, {scale, 4.0 * scale}, color);
			// flat bottom part
			setRect(pixels, {location.x() + scale, location.y() + 6.0 * scale}, {scale * 3.0, scale}, color);
			// right flat bit aka right part of curve
			setRect(pixels, {location.x() + 4.0 * scale, location.y() + 4.0 * scale}, {scale, 2.0 * scale}, color);
			// top part off curve
			setRect(pixels, {location.x() + scale, location.y() + scale * 3.0}, {3.0 * scale, scale}, color);
		break;
		case '7':
			// hookey bit
			setSquare(pixels, {location.x(), location.y() + scale}, scale, color);
			// top flat bit
			setRect(pixels, location, {scale * 5.0, scale}, color);
			// back part of hook aka flat part at far right
			setRect(pixels, {location.x() + scale * 4.0, location.y() + scale}, {scale, scale * 2.0}, color);
			// connecting bit in the middle
			setSquare(pixels, {location.x() + 3.0 * scale, location.y() + 3.0 * scale}, scale, color);
			// stem
			setRect(pixels, {location.x() + scale * 2.0, location.y() + scale * 4.0}, {scale, 3.0 * scale}, color);
		break;
		case '8':
			// using symmetry...
			// three horizontal bars
			setRect(pixels, {location.x() + scale, location.y() + 0.0 * scale}, {3.0 * scale, scale}, color);
			setRect(pixels, {location.x() + scale, location.y() + 3.0 * scale}, {3.0 * scale, scale}, color);
			setRect(pixels, {location.x() + scale, location.y() + 6.0 * scale}, {3.0 * scale, scale}, color);
			// two upper vertical bars
			setRect(pixels, {location.x() + 0.0 * scale, location.y() + scale}, {scale, 2.0 * scale}, color);
			setRect(pixels, {location.x() + 4.0 * scale, location.y() + scale}, {scale, 2.0 * scale}, color);
			// two lower veritcal bars
			setRect(pixels, {location.x() + 0.0 * scale, location.y() + 4.0 * scale}, {scale, 2.0 * scale}, color);
			setRect(pixels, {location.x() + 4.0 * scale, location.y() + 4.0 * scale}, {scale, 2.0 * scale}, color);

		break;
		case '9':
			// upper flat bit
			setRect(pixels, {location.x() + scale, location.y()}, {3.0 * scale, scale}, color);
			// left vertical bit
			setRect(pixels, {location.x(), location.y() + scale}, {scale, scale * 2}, color);
			// lower part of curv aka lower horizontal flat bit
			setRect(pixels, {location.x() + scale, location.y() + 3.0 * scale}, {3.0 * scale, scale}, color);
			// back of the nine aka right vertical flat bit
			setRect(pixels, {location.x() + 4.0 * scale, location.y() + scale}, {scale, 4.0 * scale}, color);
			// connecty bit in the lower middle right
			setSquare(pixels, {location.x() + 3.0 * scale, location.y() + 5.0 * scale}, scale, color);
			// bottom flat horizontal bit
			setRect(pixels, {location.x() + scale, location.y() + 6.0 * scale}, {scale * 2.0, scale}, color);
		break;
		case '0':
			// symmetry again...
			// top of the zero
			setRect(pixels, {location.x() + scale, location.y() + 0.0 * scale}, {3.0 * scale, scale}, color);
			// bottom of the zero
			setRect(pixels, {location.x() + scale, location.y() + 6.0 * scale}, {3.0 * scale, scale}, color);
			// left of the zero
			setRect(pixels, {location.x(), location.y() + scale}, {scale, 5.0 * scale}, color);
			setRect(pixels, {location.x() + 4.0 * scale, location.y() + scale}, {scale, 5.0 * scale}, color);
			// now the cross in the middle
			setSquare(pixels, {location.x() + 1.0 * scale, location.y() + 4.0 * scale}, scale, color);
			setSquare(pixels, {location.x() + 2.0 * scale, location.y() + 3.0 * scale}, scale, color);
			setSquare(pixels, {location.x() + 3.0 * scale, location.y() + 2.0 * scale}, scale, color);
		break;
		break;
		case '.':
			setSquare(pixels, {location.x() + 2.0 * scale, location.y() + 6.0 * scale}, scale, color);
		break;
		default:
		break;
	}
}

