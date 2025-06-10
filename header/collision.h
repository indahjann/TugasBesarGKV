#ifndef COLLISION_H
#define COLLISION_H
 
#include "object.h"
  
struct CollisionResult {
    bool collided;    
    bool blockX;       
    bool blockZ;      
};

bool checkCollision(float newX, float newZ);
bool checkAABBCollision(const BoundingBox& a, const BoundingBox& b);
float getGroundLevel(float x, float z);
CollisionResult checkWallCollision(float newX, float newZ, float oldX, float oldZ);
bool checkEnemySceneryCollision(float x, float z, float radius = 0.5f);
#endif