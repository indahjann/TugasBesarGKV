#ifndef SCENERY_H
#define SCENERY_H

#include <vector>
#include <GL/glut.h>
#include "object.h"
 
// Scenery object types
enum SceneryType {
    SCENERY_TREE,
    SCENERY_PINE_TREE,
    SCENERY_BUSH,
    SCENERY_ROCK,
    SCENERY_BENCH,
    SCENERY_LAMPPOST,
    SCENERY_BUILDING,
    SCENERY_FOUNTAIN,
    SCENERY_FLOWER_BED
};

// Generic scenery object with position, type and rendering properties
struct SceneryObject {
    Position position;
    float rotationY;
    float scale;
    SceneryType type;
    Color color;
    bool castShadow;  // Whether this object should cast shadow
};

// Building types for variety
struct Building {
    Position position;
    float width;
    float length;
    float height;
    float rotationY;
    int numWindows;
    int numFloors;
    Color baseColor;
    Color roofColor;
    GLuint texture;
};

// Define functions
void initScenery();
void drawScenery();
void drawTree(float x, float y, float z, float scale);
void drawPineTree(float x, float y, float z, float scale);
void drawBush(float x, float y, float z, float scale);
void drawRock(float x, float y, float z, float scale);
void drawBench(float x, float y, float z, float rotationY, float scale);
void drawLampPost(float x, float y, float z, float scale);
void drawDistantBuilding(const Building& building);
void drawFountain(float x, float y, float z, float scale);
void drawFlowerBed(float x, float y, float z, float width, float length, float rotationY);

// Global array of scenery objects
extern std::vector<SceneryObject> sceneryObjects;
extern std::vector<Building> buildings;

#endif