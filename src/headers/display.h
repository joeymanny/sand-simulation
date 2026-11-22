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

// this const must be updated by hand
// it must EXCLUDE the NONE variant
constexpr int NUM_SLIDERS = 2; // READ COMMENT PLS
enum ProgramSlider{
	FRICTION,
	PLATFORM,
	// add new variants above NONE
	// and do not include the NONE variant
	// in the NUM_SLIDERS count
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
		// get slider x position at top left
		int x();
		// get slider y postiion at top left
		int y();
		// set slider position at top left
		void setX(int);
		// set slider position at top left
		void setY(int);
		// get slider's minimum value
		float getMin() const;
		// set slider's minimum value
		void setMin(float);
		// get slider's maximum value
		float getMax() const;
		// set slider's maximum value 
		void setMax(float);
		// get slider's current value, which get updated as user
		// slides the slider
		float getValue() const;
		// set the value of the slider
		void setValue(float);
		// draw the slider onto the given pixel buffer
		void drawSlider(std::vector<unsigned int>* pixels);
		// check if a click at a given position is within the
		// circle of the slider
		bool isClickWithin(int, int);
		// updates the value inside a slider that is held down
		// from the change in mouse motion. this function automatically
		// scales the input to match the min and max of the slider
		void updateValFromMouseMotion(float absX, float deltaX);
};

// draw a circle onto the given pixel buffer at the given position with the given radius and given color
void circle(std::vector<unsigned int>* pixels, const Vec2& center, int radius, unsigned int color);

void setPixel(std::vector<unsigned int>* pixels, int pixelX, int pixelY, unsigned int color);
