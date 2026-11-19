#pragma once
/*
 * This header file provides easy renderers for various things like representing density in a single pixel
 */
#include <SDL3/SDL_render.h>
#include <vector>
#include "math.h"

// void display(vector<vector<vector<SandGrain>>>){}

typedef std::vector<unsigned int> PixelList;

struct a {
	PixelList* pixelData;
	int screenWidth;
	int screenHeight;
};

void calculatePixels(a pixels);

void circle(a* pixels, Vec2 center, int radius, unsigned int color);

void setPixel(a* pixels, int pixelX, int pixelY);

template <typename T> void displaySlider(Vec2 location, T* value, T min, T max);
