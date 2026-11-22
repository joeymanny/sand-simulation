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
#include <SDL3/SDL_video.h>
#include <vector>

// Our header files
#include "headers/sand.h"
#include "headers/interactions.h"
#include "headers/display.h"

// Global constants
constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;
constexpr int FPS_LIMIT = 60;
constexpr long NS_PER_FRAME = 1000000000 / FPS_LIMIT;

// For sliders: Bounds of...
// FRICTION_MULT -> [.2,.8]					  	   NOT FINAL VALUES
// GRAVITY_ACCEL -> [1 * GRAVITY, 2 * GRAVITY]     NOT FINAL VALUES

// Constants for the entire window
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* pixelTexture = nullptr;
vector<unsigned int> pixelData(SCREEN_WIDTH * SCREEN_HEIGHT, 0x000000FF);
SandGrid grid(SCREEN_WIDTH, SCREEN_HEIGHT);

// variables for holding user's desired communicated by sliders
Slider sliderAirResistance(10,10, 5.0, 10.0, 7.0);
// constexpr int SLIDER_AIR_RES_INDEX = 0;
// float sliderAirResistance = 1.0; // range from X to Y
float sliderGravity = 9.8; // range form X to Y
float sliderSandFriction = 1.0; // X to Y
int sandParticles = 1000;

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

	// Refresh framebuffer to all black
	pixelData = vector<unsigned int>(SCREEN_WIDTH * SCREEN_HEIGHT, 0x000000FF);

	// Calculate particles
	for (vector<vector<SandGrain>>& row : grid.the)
		for (vector<SandGrain>& col : row)
			for (SandGrain& grain : col) {
				grain.updateVelocity(dt);
				grain.updatePosition(dt);
				grid.updateGridPlacements();
			}

	// Render pixels (update to density map later)
	for (vector<vector<SandGrain>>& row : grid.the)
		for (vector<SandGrain>& col : row)
			for (SandGrain& grain : col)
				circle({&pixelData, SCREEN_WIDTH, SCREEN_HEIGHT}, grain.getPosition(), grain.RADIUS, 0xFFFFFFFF);

	sliderAirResistance.setValue(sliderAirResistance.getValue() + 0.1);
	sliderAirResistance.drawSlider({&pixelData, SCREEN_WIDTH, SCREEN_HEIGHT});

	// Render the window
	SDL_UpdateTexture(pixelTexture, NULL, pixelData.data(), SCREEN_WIDTH * 4);
	SDL_RenderTexture(renderer, pixelTexture, NULL, NULL);

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

	// Initialize particles
	grid.addParticle({320, 100});
	grid.addParticle({400, 50});

	// Main loop
	while (loop(&e, dt)) {
		// Get the time it took to render
		SDL_Time tick;
		SDL_GetCurrentTime(&tick);

		// Sleep until you are able to render the next frame. This is what allows framerate limiting
		SDL_Time renderTime = tick - now;
		if (renderTime < NS_PER_FRAME)
			SDL_DelayNS(NS_PER_FRAME - renderTime);

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
	if (!SDL_CreateWindowAndRenderer("Hello World", SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer)) {
		SDL_Log("Window creation error: %s", SDL_GetError());
		return false;
	}
	// Initialize VSync
	if (!SDL_SetRenderVSync(renderer, 1)) {
		SDL_Log("VSync Init error: %s", SDL_GetError());
		return false;
	}

	pixelTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	return true;
}

void windowClose() {
	// Clean up resources and close the window
	SDL_DestroyTexture(pixelTexture);
	pixelTexture = nullptr;
	SDL_DestroyRenderer(renderer);
	renderer = nullptr;
	SDL_DestroyWindow(window);
	window = nullptr;

	// Quit SDL
	SDL_Quit();
}
