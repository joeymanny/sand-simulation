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

void calculatePixels(a pixels) {
	circle(pixels, Vec2(100, 100), 20);
}

void circle(a pixels, Vec2 center, int radius) {
	SDL_Log("(%lf, %lf)", center.x(), center.y());
	for (int row = center.x() - radius; row <= center.x() + radius; row++) {
		for (int col = center.y() - radius; col <= center.y() + radius; col++) {
			if (row > 0 && row < pixels.screenHeight && col > 0 && col < pixels.screenWidth) {
				if (sqrt(pow(row - center.x(), 2) + pow(col - center.y(), 2)) <= radius) {
					pixels.pixelData->at(pixels.screenWidth * row + col) = 0xFFFFFFFF;
				} else {
					pixels.pixelData->at(pixels.screenWidth * row + col) = 0xFF0000FF;
				}
			}
		}
	}
}

template <typename T> void displaySlider(Vec2 location, T* value, T min, T max){
}
