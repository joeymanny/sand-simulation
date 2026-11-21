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
		void addParticle(Vec2 pos);
		// Remove a random sand particle from the grid
		void removeParticle();

		// Update grid placements for position updates
		void updateGridPlacements();
};

// stuff needed
// One thing to have is a function to convert particle positions to screen coordinates based on window size and zoom
