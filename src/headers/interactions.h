#pragma once
/*
 * Defines interactions between sand particles, like collisions and frictions
 */

#include <vector>
using namespace std;

#include "math.h"
#include "sand.h"

// Constantinoples
#define PARTICLE_COLL_VELOCITY_STOP_THRESHOLD

// Sand state
// This class is only implemented for SandGrain and SandGrid, as seen in interactions.cpp
template <class SandType = SandGrain> class SandGrid {
	private:
		int sandCount = 0;
	public:
		// Width AND height of cell grid
		// This should be greater than twice the radius of a particle
		static constexpr int GRID_CELL_WIDTH = 50;
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
		vector<vector<vector<SandType>>> the;

		// Constructor
		SandGrid(int screenWidth, int screenHeight);

		// Add a sand particle to the grid
		void addParticle(const Vec2& pos);
		// Add a sand particle to the grid
		void addParticle(const SandType* grain);
		// Remove a random sand particle from the grid
		void removeParticle();

		// Get the sand particle count
		int getSandCount() const;

		// Update grid placements for position updates
		void updateGridPlacements();

		// Zero out the entire sand grid
		void zero();

		// Get the grid placement of a position, limited by the grid bounds
		int getGridX(double x) const;
		int getGridY(double y) const;
};

bool collideParticles(SandGrain* a, SandGrain* b, SDL_Time dt);

// stuff needed
// One thing to have is a function to convert particle positions to screen coordinates based on window size and zoom
