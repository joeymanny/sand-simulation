#pragma once
/*
 * Defines interactions between sand particles, like collisions and frictions
 */

#include <vector>
using namespace std;

#include "sand.h"

// Constantinoples

// Sand state
class SandGrid {
	public:
		// Width AND height of cell grid
		static constexpr int GRID_CELL_WIDTH = 10;

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

};

// stuff needed
// One thing to have is a function to convert particle positions to screen coordinates based on window size and zoom
