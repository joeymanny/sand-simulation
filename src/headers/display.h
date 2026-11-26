#pragma once
/*
 * This header file provides easy renderers for various things like representing density in a single pixel
 */
#include <SDL3/SDL_render.h>
#include <vector>
#include <string>

#include "math.h"
#include "interactions.h"

inline constexpr int SLIDER_WIDTH = 100;
inline constexpr int SLIDER_HEIGHT = 4;
inline constexpr int SLIDER_CIRCLE_RADIUS = 8;
inline constexpr int SLIDER_BOUNDS_GRACE = 1;
// color of the sand - red chennel
inline constexpr unsigned int SAND_COLOR_R = 0xFF; // 0xE2;
// color of the sand - green chennel
inline constexpr unsigned int SAND_COLOR_G = 0xDF; // 0xCA;
// color of the sand - blue chennel
inline constexpr unsigned int SAND_COLOR_B = 0x81; // 0x76;
// how sharply the brightness of a pixel increases with the number
// of sand particle. at a low value one grain will appear dim, at a high
// value one grain will appear bright
inline constexpr double COLOR_RAMP_SHARPNESS = 2.0;
class Slider {
	private:
		int xPos;
		int yPos;
		float minVal;
		float maxVal;
		float value;
	public:
		Slider(int inXPos, int inYPos, float inMin, float inMax, float inValue);
		// get slider x position at top left
		int x();
		// get slider y postiion at top left
		int y();
		// set slider position at top left
		void setX(int newX);
		// set slider position at top left
		void setY(int newY);
		// get slider's minimum value
		float getMin() const;
		// set slider's minimum value
		void setMin(float newMin);
		// get slider's maximum value
		float getMax() const;
		// set slider's maximum value
		void setMax(float newMax);
		// get slider's current value, which get updated as user
		// slides the slider
		float getValue() const;
		// set the value of the slider
		void setValue(float newValue);
		// check if a click at a given position is within the
		// circle of the slider
		bool isClickWithin(int x, int y);
		// updates the value inside a slider that is held down
		// from the change in mouse motion. this function automatically
		// scales the input to match the min and max of the slider
		void updateValFromMouseMotion(float absX, float deltaX);
		// draw the slider onto the given pixel buffer
		void drawSlider(std::vector<unsigned int>* pixels, bool drawAsPressed);
};

// draw a circle onto the given pixel buffer at the given position with the given radius and given color
void circle(std::vector<unsigned int>* pixels, const Vec2& center, int radius, unsigned int color);

// Set a pixel to a certain color in the framebuffer
void setPixel(std::vector<unsigned int>* pixels, int pixelX, int pixelY, unsigned int color);

// Draw a rectangle
void setRect(std::vector<unsigned int>* pixels, const Vec2& location, const Vec2& size, unsigned int color);

// Draw a square
void setSquare(std::vector<unsigned int>* pixels, const Vec2& location, float size, unsigned int color);

// draws some blocky text. location is at the very top-left corner of the text. a scale of 1.0 indicates
// each "pixel" of the text is the same size as a screen pixel. monospace: every character is 5 "scale"s wide
// by 7 "scale"s tall, plus one padding "scale" between each character (so 6 "scale"s wide)
// works best or whole-number values of scale, because floating point numbers get floor'd into pixel locations
void drawText(std::vector<unsigned int>* pixels, const Vec2& location, float scale, unsigned int color, const std::string& text);

// draws a character as a given location. location is at the very top-left of the character. returns the width
// of the drawn character. a scale of 1.0 means each "pixel" of the character is one screen pixel in size.
// drawn character will be 5 "scale"s wide by 7 "scale"s tall
void drawChar(std::vector<unsigned int>* pixels, const Vec2& location, float scale, unsigned int color, char c);

// draws a density map of the given sand grid to the given pixels. fully overwrites
// the inputted pixels.
void drawDensityMap(std::vector<unsigned int>* pixels, const SandGrid<SandGrain>& sandGrid);

// returns a brightness factor within [0,1) for a given number of sand
// grains in one pixel (char is acting as an unsigned 8-bit int)
double pixelBrightness(char count);
