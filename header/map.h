#ifndef MAP_H
#define MAP_H

#include <vector>
 
// Wall definition structure 
struct WallDefinition {
    float x1, z1;     // Starting point (x, z)
    float x2, z2;     // Ending point (x, z)
    float y;          // Base height (ground level)
    float height;     // Wall height
    float thickness;  // Wall thickness
}; 

struct FenceDefinition {  
    float startX, startZ; // Starting point (x, z)
    float endX, endZ;     // Ending point (x, z)
    float height;         // Fence height
    float thickness;      // Fence thickness
    int numPosts;        // Number of posts
};
 
struct RooftopDefinition {
    float x1, z1;       // Starting point (x, z)
    float x2, z2;       // Ending point (x, z)
    float y;            // Base height (ground level)
    float thickness;    // Roof thickness
};

// Add this structure after the other structure definitions

struct StaircaseDefinition {
    float startX, startZ;    // Starting position of staircase
    float width;             // Width of staircase
    float stepDepth;         // Depth of each step
    float stepHeight;        // Height of each step
    int numSteps;            // Number of steps
    float baseY;             // Base height (ground level)
};

// Add this to the extern section
extern std::vector<StaircaseDefinition> staircases;
extern std::vector<Rooftop> rooftops;

// Initialize the map layout
void initMap();

// Draw the entire map
void drawMap();

void drawBorderMap();

// Make walls accessible from other files
extern std::vector<WallDefinition> walls;
extern std::vector<FenceDefinition> fences; // List of fences


#endif
