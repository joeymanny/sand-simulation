/*
 * Implementations of display.h are defined here
 */

#include "headers/sand.h"
#include "math.h"

#include <vector>
#include <SDL3/SDL_render.h>

using namespace std;
// add stuff
void display(SDL_Renderer* renderer){ //void display(vector<vector<vector<SandGrain>>>){}
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderPoint(renderer, 639.0, 479.0);
    
}


template <typename T> void displaySlider(Vec2 location, T* display, T min, T max){
    
}
