/*
 * Implementations of display.h are defined here
 */

#include "headers/display.h"
#include "headers/sand.h"
#include "math.h"

#include <SDL3/SDL_log.h>
#include <vector>
#include <cmath>
using namespace std;

class Slider {
	private:
		Vec2 position;
		float min;
		float max;
		float value;
	public:
		Vec2 getPosition(){
			return position;
		}
		void setPosition(Vec2  newPosition){
			position = newPosition;
		}
		float getMin(){
			return min;
		}
		void setMin(float newMin){
			min = newMin;
		}
		float getMax(){
			return max;
		}
		void setMax(float newMax){
			max = newMax;
		}
		float getValue(){
			return value;
		}
		void setValue(float newValue){
			value = newValue;
		}
		
};

void setPixel(a* pixels, int pixelX, int pixelY, unsigned int color) {
	if (pixelY > 0 && pixelY < pixels->screenHeight && pixelX > 0 && pixelX < pixels->screenWidth) {
		pixels->pixelData->at(pixels->screenWidth * pixelY + pixelX) = color;
	}
}

void circle(a pixels, Vec2 center, int radius, unsigned int color) {
	for (int row = center.x() - radius; row <= center.x() + radius; row++) {
		for (int col = center.y() - radius; col <= center.y() + radius; col++) {
			float dist = sqrt(pow(row - center.x(), 2) + pow(col - center.y(), 2));
			if (dist <= radius) {
				setPixel(&pixels, col, row, color);
				// Border color and size
				// if (fabs(dist - radius) < 5)
				// 	setPixel(pixels, col, row, 0xFF0000FF);
			}
		}
	}
}

template <typename T> void displaySlider(Vec2 location, T* value, T min, T max){
}
