#ifndef ENEMY_H
#define ENEMY_H

#include <GL/glut.h>
#include <vector>
#include "object.h"
 
// Enemy structure 
struct Enemy {
    char name[50];           // Enemy name
    float health;            // Current health
    float maxHealth;         // Maximum health
    Position position;       // Position in 3D space
    float rotationY;         // Rotation around Y axis
    float scale;             // Size scale
    bool isAlive;            // Is the enemy alive
    
    // AI parameters
    float moveSpeed;            // Kecepatan gerakan
    float moveDirection;        // Arah gerakan dalam derajat (0-359)
    float moveDuration;         // Durasi bergerak ke arah tertentu
    float moveTimer;            // Timer untuk pergantian arah
    bool isMoving;              // Apakah sedang bergerak
    float idleDuration;         // Durasi diam
    float idleTimer;            // Timer untuk waktu diam

    
    // Animation parameters (similar to player)
    float leftArmRotation;
    float rightArmRotation;
    float leftLegRotation;
    float rightLegRotation;
};


// Initialize enemy system
void initEnemySystem();

// Create a new enemy
Enemy createEnemy(const char* name, float x, float y, float z, float maxHealth);

// Draw an enemy
void drawEnemy(const Enemy& enemy);

// Draw enemy health bar
void drawEnemyHealthBar(const Enemy& enemy);

// Display enemy information on screen
void displayEnemyInfo();

// Spawn a new enemy at random position
void spawnEnemy();

// Update all enemies
void updateEnemies(float deltaTime);
// New functions for wave-based enemy spawning
extern void spawnWaveEnemies(int count);
extern int getAliveEnemyCount();
extern void clearAllEnemies();


// Draw crosshair
void drawCrosshair(int width, int height);


// Deklarasi fungsi baru
void updateEnemyAI(Enemy& enemy, float deltaTime);

// Global variables
extern std::vector<Enemy> enemies;

extern const int MAX_ENEMIES;
extern const float ENEMY_SPAWN_AREA;
extern const float ENEMY_MIN_DISTANCE;
extern const float ENEMY_ATTACK_RANGE;
extern const float ENEMY_DAMAGE;

#endif // ENEMY_H