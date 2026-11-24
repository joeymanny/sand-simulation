#pragma once
/*
 * Defines interactions between sand particles, like collisions and frictions
 */

#include <vector>
using namespace std;

#include "math.h"
#include "sand.h"

// Constantinoples

// Sand state
class SandGrid {
	private:
		int sandCount = 0;
	public:
		// Width AND height of cell grid
		static constexpr int GRID_CELL_WIDTH = 10;
		int cellsX, cellsY;

		/*
			Rows {
				Columns {
					Grid {
						SandGrain
					}
				}
			}
		 */
		vector<vector<vector<SandGrain>>> the;

		// Constructor
		SandGrid(int screenWidth, int screenHeight);

		// Add a sand particle to the grid
		void addParticle(const Vec2& pos);
		// Add a sand particle to the grid
		void addParticle(SandGrain* grain);
		// Remove a random sand particle from the grid
		void removeParticle();

		// Get the sand particle count
		int getSandCount() const;

		// Update grid placements for position updates
		void updateGridPlacements();
};

// Detect if a particle would collide with the edges of the window
bool detectScreenBoundsCollision(const Vec2& startPos, const Vec2& velocity, const double& radius, const double& dt);

// Reflect the particle after detecting a collision with the edges of the window
void screenBoundReflect(Vec2* startPos, Vec2* velocity, const double& radius, const double& dt);
// Stop the particle when the particle is slow enough to be considered "stopping", after a screen edge collision
void screenBoundStop(Vec2* startPos, Vec2* velocity, const double& radius);

// Collide two particles, updating the positin to the collision stop position, and the velocity to the new trajectory after collision
void collideParticles(SandGrain* a, SandGrain* b);

// stuff needed
// One thing to have is a function to convert particle positions to screen coordinates based on window size and zoom
