#pragma once
/*
 * This header file provides easy renderers for various things like representing density in a single pixel
 */
#include <SDL3/SDL_render.h>
#include <vector>
#include "math.h"

typedef std::vector<unsigned int> PixelList;

struct ScreenPixels {
	PixelList* pixelData;
	int screenWidth;
	int screenHeight;
};

constexpr int NUM_SLIDERS = 2;

enum ProgramSlider{
	FRICTION,
	PLATFORM,
	NONE,
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
		float getMin() const;
		void setMin(float);
		float getMax() const;
		void setMax(float);
		float getValue() const;
		void setValue(float);
		void drawSlider(ScreenPixels);
};

void circle(const ScreenPixels& pixels, const Vec2& center, int radius, unsigned int color);

void setPixel(const ScreenPixels& pixels, int pixelX, int pixelY, unsigned int color);

