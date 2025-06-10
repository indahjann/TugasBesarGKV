#ifndef COMBAT_H
#define COMBAT_H

#include <GL/glut.h> 
#include <vector>
#include "object.h" 
#include "enemy.h"

// Bullet structure
struct Bullet {
    Position position;       // Current position
    Position direction;      // Direction vector
    float speed;             // Movement speed
    float distanceTraveled;  // Distance traveled so far
    float maxDistance;       // Maximum distance before disappearing
    bool active;             // Is the bullet active
};
 
extern std::vector<Bullet> bullets;

// Create a new bullet
void createBullet(const Position& start, const Position& direction);

// Update all bullets
void updateBullets(float deltaTime);

// Draw all bullets
void drawBullets();

// Check if bullet hits an enemy
void checkBulletHits();

// Draw pistol
void drawPistol(bool isRightHand);

// Check if player is attacking an enemy
bool checkPlayerAttack(const Player& player, Enemy& enemy);

extern const float BULLET_SPEED;
extern const float BULLET_MAX_DISTANCE;
extern const float BULLET_DAMAGE;

#endif // COMBAT_H