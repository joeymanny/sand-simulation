/*
 * Josie Peterson
 * June Schlabaugh
 * Andrew Moore
 *
 * Unnamed Sand Simulation
 * This source file is the entry point for the sand simulation program.
 */

#include "headers/math.h"
constexpr char TITLE[] = "Unnamed Sand Simulation";

// Simple Directmedia Layer
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_time.h>
#include <SDL3/SDL_video.h>
#include <cmath>
#include <vector>

// Our header files
#include "headers/sand.h"
#include "headers/interactions.h"
#include "headers/display.h"
#include "headers/common.h"

// Global constants
constexpr int FPS_LIMIT = 60;
constexpr int NS_PER_SEC = 1000000000;
constexpr long NS_PER_FRAME = NS_PER_SEC / FPS_LIMIT;

// For sliders: Bounds of...
// FRICTION_MULT -> [.2,.8]                        NOT FINAL VALUES
// GRAVITY_ACCEL -> [1 * GRAVITY, 2 * GRAVITY]     NOT FINAL VALUES

// Constants for the entire window
// The window itself
SDL_Window* window = nullptr;
// The renderer
SDL_Renderer* renderer = nullptr;
// The texture object for the pixels we will put on the screen
SDL_Texture* pixelTexture = nullptr;
// The data that pixelTexture will use for rendering
vector<unsigned int> pixelData(SCREEN_WIDTH * SCREEN_HEIGHT, 0x000000FF);
// More data that is not zeroed each loop
vector<unsigned int> permanentTrail(SCREEN_WIDTH * SCREEN_HEIGHT, 0x00000000);

// The grid of particles
SandGrid grid(SCREEN_WIDTH, SCREEN_HEIGHT);
// Keep track of the grains of sand we have already looped through during the collision checker loop
// This should be zeroed at the start of every loop
SandGrid<SandGrain*> ptrGrid(SCREEN_WIDTH, SCREEN_HEIGHT);

// this const must be updated by hand.
// it must EXCLUDE the NONE variant.
// READ COMMENT PLS
constexpr int NUM_SLIDERS = 2;
enum ProgramSlider{
	FRICTION,
	PLATFORM,
	// README before adding variants!
	// add new variants above NONE
	// and do not include the NONE variant
	// in the NUM_SLIDERS count
	NONE,
};

// variables for holding user's desired communicated by sliders
// number of sliders is in NUM_SLIDERS constant
// this is an array of sliders
Slider sliders[NUM_SLIDERS] = {
	// slider construction:
	// {xPos, yPos, minVal, maxVal, value}
	{20,20, 5.0, 10.0, 7.0}, // Friction slider, accessible with sliders[FRICTION].getValue()
	{20, 40, 50.0, SCREEN_WIDTH - 100, 300}, // Platform slider, accessible with slider[PLATFORM].getValue()
};
ProgramSlider selectedSlider = ProgramSlider::NONE; // could also just do NONE
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
	// Keep track of time (modulated) for framerate rendering purposes
	static unsigned int nanoseconds = 0;
	static bool fpsTextUpdated = false;
	static int fps = 0;

	// Handle events
	while (SDL_PollEvent(e)) {
		// Runs when the person closes the window
		if (e->type == SDL_EVENT_QUIT)
			return false;
		// Detect mouse button-down when the cursor is inside of a slider circle
		if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
			for (int i = 0; i < NUM_SLIDERS; i++) {
				if (sliders[i].isClickWithin(e->motion.x, e->motion.y)) {
					selectedSlider = (ProgramSlider)i;
				}
			}
		}
		// Detect mouse button-up to stop sliding
		if (e->type == SDL_EVENT_MOUSE_BUTTON_UP)
			selectedSlider = ProgramSlider::NONE;
		// Detect mouse motion for updating the sliders
		if (e->type == SDL_EVENT_MOUSE_MOTION && selectedSlider != NONE) {
			sliders[selectedSlider].updateValFromMouseMotion(e->motion.x, e->motion.xrel);
			// SDL_Log("slider value now: %f", sliders[selectedSlider].getValue());
		}
		// TODO: reset button to reset simulation while keeping slider values the same
	}

	// Refresh framebuffer to all black
	pixelData = vector<unsigned int>(SCREEN_WIDTH * SCREEN_HEIGHT, 0x000000FF);

	// Step 1: Update velocity from acceleration and refresh partialVelocity for collision purposes
	for (vector<vector<SandGrain>>& row : grid.the)
		for (vector<SandGrain>& col : row)
			for (SandGrain& grain : col)
				grain.updateVelocity(dt);

	// For the new collisions:
	//     Move updating position to after this for loop i will describe:
	//     For each particle in a given grid, check collisions between each other particle by:
	//         collide 2 particles, and they will have a new position (collision stop) and new velocity (new trajectory)
	//         for each iteration, add a 3rd particle to the system, ONLY check the collision between this and the 2 previously finished
	//             collided particles
	//     THEN finally update the positions after we have the new trajectories

	// Step 2: Detect collisions, and update positions, and shrink partial velocities until there are no more collisions to
	//     collide or no more partial velocities to shrink
	// For checking collisions, you only need to check the grids that are reachable by the maximum velocity magnitude of all particles
	bool collided;
	do {
		double maxVelocityMagnitude = 0;
		collided = false;

		// Refresh the grid of pointers to already-collided sandgrains
		ptrGrid.zero();

		// First loop - go over all particle collisions first
		for (vector<vector<SandGrain>>& row : grid.the) {
			for (vector<SandGrain>& col : row) {
				for (SandGrain& grain : col) {
					// Calculate the highest velocity observed for gridding purposes
					maxVelocityMagnitude = max(maxVelocityMagnitude, grain.getPartialVelocity().magnitude());

					// Get how far one should extend the search area by how big the max velocity magnitude is
					Vec2 hypotheticalPosition = grain.getPosition().add(grain.getPartialVelocity().scale(dtToSec(dt)));
					int expandGrid = max(
						abs(ptrGrid.getGridX(hypotheticalPosition.x()) - grid.getGridX(grain.getPosition().x())),
						abs(ptrGrid.getGridY(hypotheticalPosition.y()) - grid.getGridY(grain.getPosition().y()))
					) + 1;

					// Perform and detect particle collisions using an O(n choose 2) selection algorithm,
					//     limited by the grids
					if (ptrGrid.getSandCount() > 0) {
						int gridY = ptrGrid.getGridY(grain.getPosition().y());
						for (int ptrRow = max(0, gridY - expandGrid); ptrRow <= min(gridY + expandGrid, ptrGrid.cellsY - 1); ptrRow++) {
							int gridX = ptrGrid.getGridX(grain.getPosition().x());
							for (int ptrCol = max(0, gridX - expandGrid); ptrCol <= min(gridX + expandGrid, ptrGrid.cellsX - 1); ptrCol++) {
								for (int grainIndex = 0; grainIndex < ptrGrid.the[ptrRow][ptrCol].size(); grainIndex++) {
									SandGrain* otherGrain = ptrGrid.the[ptrRow][ptrCol][grainIndex];

									// If the two grains of sand are not the same grain of sand, check collision
									if (grain.id() != otherGrain->id()) {
										Vec2 beforeVelocityP0 = grain.getVelocity(); // TESTING CODE
										Vec2 beforePVelocityP0 = grain.getPartialVelocity(); // TESTING CODE
										Vec2 beforeVelocityP1 = otherGrain->getVelocity(); // TESTING CODE
										Vec2 beforePVelocityP1 = otherGrain->getPartialVelocity(); // TESTING CODE
										collided = collideParticles(&grain, otherGrain, dt); // This will also update their properties
										if (collided) {
											SDL_Log("Particle Collision!");

											// TESTING CODE: Print velocity before and after, then visualize the collision stop
											Vec2 afterVelocityP0 = grain.getVelocity();
											Vec2 afterPVelocityP0 = grain.getPartialVelocity();
											Vec2 afterVelocityP1 = otherGrain->getVelocity();
											Vec2 afterPVelocityP1 = otherGrain->getPartialVelocity();
											SDL_Log("\x1B[33mP%ld \x1B[31mPARTICLE\x1B[0m :: VELOCITY         | Before: (\x1B[34m%lf\x1B[0m, \x1B[34m%lf\x1B[0m), After: (\x1B[34m%lf\x1B[0m, \x1B[34m%lf\x1B[0m)", grain.id(), beforeVelocityP0.x(), beforeVelocityP0.y(), afterVelocityP0.x(), afterVelocityP0.y());
											SDL_Log("\x1B[33mP%ld \x1B[31mPARTICLE\x1B[0m :: PARTIAL VELOCITY | Before: (\x1B[36m%lf\x1B[0m, \x1B[36m%lf\x1B[0m), After: (\x1B[36m%lf\x1B[0m, \x1B[36m%lf\x1B[0m)", grain.id(), beforePVelocityP0.x(), beforePVelocityP0.y(), afterPVelocityP0.x(), afterPVelocityP0.y());
											SDL_Log("\x1B[33mP%ld \x1B[31mPARTICLE\x1B[0m :: VELOCITY         | Before: (\x1B[34m%lf\x1B[0m, \x1B[34m%lf\x1B[0m), After: (\x1B[34m%lf\x1B[0m, \x1B[34m%lf\x1B[0m)", otherGrain->id(), beforeVelocityP1.x(), beforeVelocityP1.y(), afterVelocityP1.x(), afterVelocityP1.y());
											SDL_Log("\x1B[33mP%ld \x1B[31mPARTICLE\x1B[0m :: PARTIAL VELOCITY | Before: (\x1B[36m%lf\x1B[0m, \x1B[36m%lf\x1B[0m), After: (\x1B[36m%lf\x1B[0m, \x1B[36m%lf\x1B[0m)", otherGrain->id(), beforePVelocityP1.x(), beforePVelocityP1.y(), afterPVelocityP1.x(), afterPVelocityP1.y());
											circle(&permanentTrail, grain.getPosition(), 5, 0xFF0000FF);
											circle(&permanentTrail, otherGrain->getPosition(), 5, 0xFF0000FF);

											break; // Stop the entire damn loop when a single collision happens
										}
									}
								}
								if (collided) break; // Stop the entire damn loop when a single collision happens
							}
							if (collided) break; // Stop the entire damn loop when a single collision happens
						}
					}

					// Add the current particle to the matrix of particles that have already been collision-calculated
					SandGrain* tempPtr = &grain; // This points to the sandgrain
					ptrGrid.addParticle(&tempPtr); // Pointer to the pointer, yet it pushes the SandGrain* to the matrix. It will work
				}
			}
		}

		// Second loop: Detect screen collisions
		for (vector<vector<SandGrain>>& row : grid.the) {
			for (vector<SandGrain>& col : row) {
				for (SandGrain& grain : col) {
					// TESTING/DEMO CODE: The proper way partial velocity is handled
					Vec2 beforeVelocity = grain.getVelocity(); // TESTING CODE
					Vec2 beforePVelocity = grain.getPartialVelocity(); // TESTING CODE

					if (grain.detectScreenBoundsCollision(dt)) {
						// If the particle is effectively immobile, structurallly stop it
						if (grain.isSlowEnough())
							grain.screenBoundStop();
						else
							// Reflect the particle if it is going fast enough
							grain.screenBoundReflect(dt);
						collided = true;

						// TESTING/DEMO CODE: partial velocity
						Vec2 afterVelocity = grain.getVelocity();
						Vec2 afterPVelocity = grain.getPartialVelocity();
						SDL_Log("\x1B[33mP%ld \x1B[32mSCREEN\x1B[0m   :: VELOCITY         | Before: (\x1B[34m%lf\x1b[0m, \x1B[34m%lf\x1b[0m), After: (\x1B[34m%lf\x1b[0m, \x1B[34m%lf\x1b[0m)", grain.id(), beforeVelocity.x(), beforeVelocity.y(), afterVelocity.x(), afterVelocity.y());
						SDL_Log("\x1B[33mP%ld \x1B[32mSCREEN\x1B[0m   :: PARTIAL VELOCITY | Before: (\x1B[36m%lf\x1b[0m, \x1B[36m%lf\x1b[0m), After: (\x1B[36m%lf\x1b[0m, \x1B[36m%lf\x1b[0m)", grain.id(), beforePVelocity.x(), beforePVelocity.y(), afterPVelocity.x(), afterPVelocity.y());
					}
				}
			}
		}
	} while (collided);

	// Step 3: Perform final position updates of particles
	for (vector<vector<SandGrain>>& row : grid.the)
		for (vector<SandGrain>& col : row)
			for (SandGrain& grain : col)
				grain.updatePosition(dt);

	// Step 4: Update grid placements
	grid.updateGridPlacements();

	// Step 5: Render pixels (update to density map later)
	// This just involves updating the grid placements for the particles,
	// then rendering a circle for each particle
	for (vector<vector<SandGrain>>& row : grid.the) {
		for (vector<SandGrain>& col : row) {
			for (SandGrain& grain : col) {
				circle(&pixelData, grain.getPosition(), grain.RADIUS, 0xFFFFFFFF);
				// circle(&permanentTrail, grain.getPosition(), 5, 0xFF0000FF);
				// TESTING CODE: Label each particle with it's ID for easy tracking
				drawText(&pixelData, grain.getPosition().add({-4, -6}), 2.0, 0x000000FF, to_string(grain.id()));
			}
		}
	}

	// enable this to draw as a density map
	// this totally overwrites pixelData
	// drawDensityMap(&pixelData, grid);

	// Draws each slider, its state, and its text value
	for (int i = 0; i < NUM_SLIDERS; i++) {
		sliders[i].drawSlider(&pixelData, i == selectedSlider);
		drawText(&pixelData, {(double)(sliders[i].x() + SLIDER_WIDTH + 15),(double)sliders[i].y() - 2}, 1.0, 0xFFFFFFFF, to_string(sliders[i].getValue()));
	}

	// TEST
	// drawText(&pixelData, {20.0,200.0}, 4.0, 0xFFFFFFFF, "1234567890.2 1234567890");
	// setSquare(&pixelData, {200,200}, 20.0, 0xFFFFFFFF);
	// TEST

	// Update framerate presentation, but only every half-second
	if (!fpsTextUpdated) {
		double fpsFloat = (double)NS_PER_SEC / (dt + 1); // +1 for no floating point errors
		fps = ceil(fpsFloat);
		fpsTextUpdated = true;
	}
	drawText(&pixelData, {600, 20}, 2.0, 0x00FF00FF, to_string(fps));

	// Update the counter keeping track of if the framerate should update every half-second
	unsigned int prevNS = nanoseconds;
	nanoseconds += dt;
	nanoseconds %= NS_PER_SEC / 2;
	if (nanoseconds < prevNS)
		fpsTextUpdated = false;

	// Render the window
	SDL_UpdateTexture(pixelTexture, NULL, pixelData.data(), SCREEN_WIDTH * 4);
	SDL_RenderTexture(renderer, pixelTexture, NULL, NULL);
	// Render the permanent trails
	// SDL_UpdateTexture(pixelTexture, NULL, permanentTrail.data(), SCREEN_WIDTH * 4);
	// SDL_RenderTexture(renderer, pixelTexture, NULL, NULL);

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
	SDL_Time dt = NS_PER_FRAME;
	SDL_GetCurrentTime(&now);

	// Initialize particles
	SandGrain testParticle0({200, 100});
	// testParticle0.instantAccelerate({0, 0});
	testParticle0.instantAccelerate({-9999999, -9999999}); // Uncomment this to test more complex collisions
	grid.addParticle(&testParticle0);
	SandGrain testParticle1({225, 200});
	testParticle1.instantAccelerate({-9999999, 9999999});
	grid.addParticle(&testParticle1);
	SandGrain testParticle2({600, 50});
	testParticle2.instantAccelerate({9999999, -9999999});
	grid.addParticle(&testParticle2);
	SandGrain testParticle3({20, 20});
	testParticle3.instantAccelerate({9999999, 9999999});
	grid.addParticle(&testParticle3);

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

	// Close the window and terminate the program
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
	if (!SDL_CreateWindowAndRenderer(TITLE, SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer)) {
		SDL_Log("Window creation error: %s", SDL_GetError());
		return false;
	}
	// Initialize VSync
	// if (!SDL_SetRenderVSync(renderer, 1)) {
	// 	SDL_Log("VSync Init error: %s", SDL_GetError());
	// 	return false;
	// }

	// Create a pixel texture sized at the screen size for easy pixel updating
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
