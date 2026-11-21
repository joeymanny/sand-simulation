#pragma once
/*
 * This header file provides easy renderers for various things like representing density in a single pixel
 */
#include <SDL3/SDL_render.h>
#include <vector>
#include "math.h"

typedef std::vector<unsigned int> PixelList;

struct a {
	PixelList* pixelData;
	int screenWidth;
	int screenHeight;
};

class Slider {
	private:
		int xPos;
		int yPos;
		float minVal;
		float maxVal;
		float value;
	public:
		Slider(int, int, float, float, float);
		int x();
		int y();
		void setX(int);
		void setY(int);
		float getMin();
		void setMin(float);
		float getMax();
		void setMax(float);
		float getValue();
		void setValue(float);
		void drawSlider(a);
};
void circle(a pixels, Vec2 center, int radius, unsigned int color);

void setPixel(a* pixels, int pixelX, int pixelY,unsigned int color);

template <typename T> void displaySlider(Vec2 location, T* value, T min, T max);
