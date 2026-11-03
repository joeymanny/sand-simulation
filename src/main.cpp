/*
 * Josie Peterson
 * June Schlabaugh
 * Andrew Moore
 *
 * Unnamed Sand Simulation
 * This source file is the entry point for the sand simulation program.
 */

// Simple Directmedia Layer
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_time.h>

// Our header files
#include "headers/sand.h"
#include "headers/interactions.h"
#include "headers/display.h"

// Global constants
constexpr int defaultScreenWidth = 640;
constexpr int defaultScreenHeight = 480;
constexpr int fpsLimit = 60;
constexpr long nsPerFrame = 1000000000 / fpsLimit;

// Constants for the entire window
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

// Initialize and open a window of width `screenWidth` and height `screenHeight`
bool windowInit();
// Close the window
void windowClose();

// This function is ran every frame. Return false to close the program.
// The vase majority of the program functionality is here, so it is the first defined function
// Run the rendering and calculations here, but don't do SDL_RenderPresent inside this loop.
//     It should take up to 1/60th of a second to do all the calculations, once that is finished,
//     the rest of the time will be spent watiing for the proper time to render, then it will render
//     the window. SDL_RenderPresent happens at the END of every frame for consistency, rather than
//     somewhere in the middle
bool loop(SDL_Event* e, SDL_Time dt) {
	// Handle events
	while (SDL_PollEvent(e)) {
		if (e->type == SDL_EVENT_QUIT) {
			return false;
		}
	}

	// Render the window
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);

	// General idea of process:
	// 1. Calculate collisions and accelerate particles as needed
	//        Could be multithreaded
	// 2. Update the positions of particles
	//        Could be multithreaded, also update the cell each particle is in
	// 3. Render particles
	//        Probably using SDL_RenderPoint and calculating the circle to render

	// Continue
	return true;
}

// No need to edit this function unless chaning something fundamental
int main() {
	// Initialize window
	if (!windowInit()) {
		windowClose();
		return 1;
	}

	// Initialize the user input hander and zero it
	SDL_Event e;
	SDL_zero(e);

	// Initialize time
	SDL_Time now;
	SDL_Time dt = 0;
	SDL_GetCurrentTime(&now);

	// Main loop
	while (loop(&e, dt)) {
		// Get the time it took to render
		SDL_Time tick;
		SDL_GetCurrentTime(&tick);

		// Sleep until you are able to render the next frame. This is what allows framerate limiting
		SDL_Time renderTime = tick - now;
		if (renderTime < nsPerFrame)
			SDL_DelayNS(nsPerFrame - renderTime);

		// Update the elapsed time inbetween frames for whatever the loop function needs it for
		SDL_GetCurrentTime(&tick);
		dt = tick - now;

		// Update the "now"
		now = tick;

		// Render the window
		SDL_RenderPresent(renderer);
	}

	windowClose();
	return 0;
}

bool windowInit() {
	// Initialize SDL, fail if error
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Init error: %s", SDL_GetError());
		return false;
	}

	// Create a window, fail if error
	if (!SDL_CreateWindowAndRenderer("Hello World", defaultScreenWidth, defaultScreenHeight, 0, &window, &renderer)) {
		SDL_Log("Window creation error: %s", SDL_GetError());
		return false;
	}
	// Initialize VSync
	if (!SDL_SetRenderVSync(renderer, 1)) {
		SDL_Log("VSync Init error: %s", SDL_GetError());
		return false;
	}

	return true;
}

void windowClose() {
	// Clean up resources and close the window
	SDL_DestroyRenderer(renderer);
	renderer = nullptr;
	SDL_DestroyWindow(window);
	window = nullptr;

	// Quit SDL
	SDL_Quit();
}
