/*
 * ==========================================================================
 * ENEMY SYSTEM IMPLEMENTATION
 * ==========================================================================
 * 
 * File: enemy.cpp
 * Author: Project Team
 * Date: May 2, 2025
 * 
 * DESCRIPTION:
 * -----------
 * Implementasi sistem musuh (enemy) untuk game, termasuk:
 * - Manajemen musuh (enemies)
 * - Sistem spawning musuh baru
 * - AI pergerakan musuh di area permainan
 * - Animasi musuh berdasarkan status gerakan
 * - Rendering musuh dan health bar
 * - UI untuk informasi musuh
 * 
 * STRUCTURES:
 * ----------
 * Enemy - Didefinisikan di enemy.h
 *   - Struktur musuh dengan properti posisi, rotasi, status kesehatan, dan parameter AI
 * 
 * CONSTANTS: 
 * ---------
 * MAX_ENEMIES         - Jumlah maksimum musuh dalam permainan (5)
 * ENEMY_SPAWN_AREA    - Area spawn untuk musuh (100.0 units)
 * ENEMY_MIN_DISTANCE  - Jarak minimum antar musuh (20.0 units)
 * ENEMY_ATTACK_RANGE  - Jarak serangan untuk deteksi serangan player (1.5 units)
 * ENEMY_DAMAGE        - Damage yang diterima musuh dari serangan player (25.0 hit points)
 * 
 * GLOBAL VARIABLES:
 * ---------------
 * enemies - Vector yang menyimpan semua musuh aktif di scene
 * 
 * FUNCTIONS:
 * ---------
 * initEnemySystem()           - Menginisialisasi sistem musuh dan membuat musuh awal
 * createEnemy()               - Membuat musuh baru dengan parameter yang diberikan
 * updateEnemyAI()             - Memperbarui pergerakan dan animasi musuh dengan AI
 * getRandomEnemyPosition()    - Menghasilkan posisi acak untuk spawn musuh
 * spawnEnemy()                - Membuat dan menambahkan musuh baru ke scene
 * drawEnemy()                 - Menggambar model 3D musuh
 * drawEnemyHealthBar()        - Menggambar health bar di atas musuh
 * displayEnemyInfo()          - Menampilkan info musuh di layar
 * drawCrosshair()             - Menggambar crosshair dengan tampilan berbeda untuk mode scope
 * updateEnemies()             - Memperbarui semua musuh dan mengelola spawning
 * 
 * DEPENDENCIES:
 * -----------
 * - enemy.h     - Header file dengan deklarasi fungsi dan struct Enemy
 * - object.h    - Header untuk akses ke struct Position dan Player
 * - render.h    - Header untuk fungsi rendering dan drawText
 * - collision.h - Header untuk deteksi tabrakan
 * - map.h       - Header untuk informasi peta
 * - combat.h    - Header untuk akses ke sistem combat dan bullets
 * 
 * AI MOVEMENT BEHAVIOR:
 * -------------------
 * 1. Musuh bergerak dengan arah acak untuk durasi tertentu
 * 2. Setelah bergerak untuk waktu tertentu, musuh akan berhenti dan idle
 * 3. Setelah durasi idle selesai, musuh akan memilih arah baru dan bergerak kembali
 * 4. Musuh akan mengubah arah jika bertabrakan dengan tembok
 * 5. Musuh tetap berada di dalam area permainan yang dibatasi
 * 
 * HOW TO USE:
 * ----------
 * 1. Panggil initEnemySystem() saat game dimulai untuk inisialisasi
 * 2. Panggil updateEnemies(deltaTime) di game loop untuk memperbarui musuh
 * 3. Panggil drawEnemy(enemy) dan drawEnemyHealthBar(enemy) saat rendering
 * 4. Panggil displayEnemyInfo() untuk menampilkan info musuh di layar
 * 5. Gunakan drawCrosshair(width, height) untuk menampilkan crosshair
 * 
 * ==========================================================================
 */

#include "enemy.h"
#include "object.h"
#include "render.h"
#include "collision.h"
#include "map.h"
#include "combat.h"
#include <cstdlib>
#include <ctime>
#include <cstring> 
#include <cmath>
#include <bits/stdc++.h>
using namespace std; 
 
std::vector<Enemy> enemies;

const int MAX_ENEMIES = 5;
const float ENEMY_SPAWN_AREA = 100.0f;
const float ENEMY_MIN_DISTANCE = 20.0f;
const float ENEMY_ATTACK_RANGE = 1.5f;
const float ENEMY_DAMAGE = 25.0f;

// Initialize the enemy system
void initEnemySystem() {
    // Seed the random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Clear any existing enemies
    enemies.clear();
    bullets.clear();
    
    // Spawn initial enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        spawnEnemy();
    }
}

// Create a new enemy with given parameters
Enemy createEnemy(const char* name, float x, float y, float z, float maxHealth) {
    Enemy enemy;
    strncpy(enemy.name, name, sizeof(enemy.name) - 1);
    enemy.name[sizeof(enemy.name) - 1] = '\0'; // Ensure null termination
    enemy.health = maxHealth;
    enemy.maxHealth = maxHealth;
    enemy.position.x = x;
    enemy.position.y = y;
    enemy.position.z = z;
    
    // Random rotation between 0 and 360 degrees
    enemy.rotationY = (float)(rand() % 360);
    
    enemy.scale = 1.0f;
    enemy.isAlive = true;
    
    // Initialize animation parameters
    enemy.leftArmRotation = 0.0f;
    enemy.rightArmRotation = 0.0f;
    enemy.leftLegRotation = 0.0f;
    enemy.rightLegRotation = 0.0f;
    
    // Initialize AI parameters
    enemy.moveSpeed = 1.0f + ((float)rand() / RAND_MAX) * 2.0f; // Random speed between 1 and 3
    enemy.moveDirection = (float)(rand() % 360);
    enemy.moveDuration = 2.0f + ((float)rand() / RAND_MAX) * 4.0f; // Move for 2-6 seconds
    enemy.moveTimer = enemy.moveDuration;
    enemy.isMoving = true;
    enemy.idleDuration = 1.0f + ((float)rand() / RAND_MAX) * 2.0f; // Idle for 1-3 seconds
    enemy.idleTimer = 0.0f;
    
    return enemy;
}

void updateEnemyAI(Enemy& enemy, float deltaTime) {
    if (!enemy.isAlive) return;
    
    // Animasikan berdasarkan status gerakan
    if (enemy.isMoving) {
        // Update animasi lengan dan kaki
        const float animSpeed = 2.0f;
        static float animTime = 0.0f;
        animTime += deltaTime * enemy.moveSpeed; 
        
        enemy.leftArmRotation = sin(animTime * animSpeed) * 30.0f;
        enemy.rightArmRotation = -sin(animTime * animSpeed) * 30.0f;
        enemy.leftLegRotation = -sin(animTime * animSpeed) * 30.0f;
        enemy.rightLegRotation = sin(animTime * animSpeed) * 30.0f;
        
        // Update moveTimer
        enemy.moveTimer -= deltaTime;
        
        if (enemy.moveTimer <= 0.0f) {
            // Berhenti sejenak
            enemy.isMoving = false;
            enemy.idleTimer = enemy.idleDuration;
            
            // Reset animasi saat berhenti
            enemy.leftArmRotation = 0.0f;
            enemy.rightArmRotation = 0.0f;
            enemy.leftLegRotation = 0.0f;
            enemy.rightLegRotation = 0.0f;
            return;
        }
        
        // Gerakkan enemy berdasarkan arah
        float rad = enemy.moveDirection * M_PI / 180.0f;
        float moveX = sin(rad) * enemy.moveSpeed * deltaTime;
        float moveZ = cos(rad) * enemy.moveSpeed * deltaTime;
        
        // Cek collision sebelum bergerak
        float newX = enemy.position.x + moveX;
        float newZ = enemy.position.z + moveZ;
        
        // Memastikan tetap di dalam area permainan
        if (newX < -95.0f) newX = -95.0f;
        if (newX > 95.0f) newX = 95.0f;
        if (newZ < -95.0f) newZ = -95.0f;
        if (newZ > 95.0f) newZ = 95.0f;
        
        // Cek collision dengan tembok
        if (!checkCollision(newX, newZ)) {
            enemy.position.x = newX;
            enemy.position.z = newZ;
            
            // Update rotasi untuk menghadap arah gerakan
            enemy.rotationY = enemy.moveDirection;
            
            // Update Y position berdasarkan ground level
            enemy.position.y = getGroundLevel(enemy.position.x, enemy.position.z);
        } else {
            // Jika bertemu tembok, ubah arah
            enemy.moveDirection = (float)(rand() % 360);
        }
        
    } else {
        // Sedang idle (diam)
        enemy.idleTimer -= deltaTime;
        
        if (enemy.idleTimer <= 0.0f) {
            // Kembali bergerak dengan arah acak
            enemy.isMoving = true;
            enemy.moveDirection = (float)(rand() % 360);
            enemy.moveTimer = enemy.moveDuration;
        }
    }
}

Position getRandomEnemyPosition() {
    Position pos;
    const float MIN_POS = -95.0f; // 5 units from fence at -100
    const float MAX_POS = 95.0f;  // 5 units from fence at 100
    bool validPosition = false;
    
    // Try to find a valid position
    int maxAttempts = 50; // Prevent infinite loop
    int attempts = 0;
    
    while (!validPosition && attempts < maxAttempts) {
        // Generate random position
        pos.x = MIN_POS + (float)rand() / (float)RAND_MAX * (MAX_POS - MIN_POS);
        pos.y = 0.0f; // Ground level
        pos.z = MIN_POS + (float)rand() / (float)RAND_MAX * (MAX_POS - MIN_POS);
        
        // Check if position collides with walls
        if (!checkCollision(pos.x, pos.z)) {
            // Check distance from other enemies
            bool tooClose = false;
            for (const auto& enemy : enemies) {
                float dx = pos.x - enemy.position.x;
                float dz = pos.z - enemy.position.z;
                float distSq = dx * dx + dz * dz;
                
                if (distSq < ENEMY_MIN_DISTANCE * ENEMY_MIN_DISTANCE) {
                    tooClose = true;
                    break;
                }
            }
            
            if (!tooClose) {
                validPosition = true;
            }
        }
        
        attempts++;
    }
    
    // If we couldn't find a good position, just use a default one
    if (!validPosition) {
        pos.x = 0.0f;
        pos.y = 0.0f;
        pos.z = 0.0f;
    }
    
    // Set proper Y position based on ground level
    pos.y = getGroundLevel(pos.x, pos.z);
    
    return pos;
}

// Spawn a new enemy at a random position
void spawnEnemy() {
    static int enemyCount = 0;
    
    // Don't spawn more than max enemies
    if (enemies.size() >= MAX_ENEMIES) {
        return;
    }
    
    // Generate name
    char name[50];
    sprintf(name, "Enemy %d", ++enemyCount);
    
    // Get random position
    Position pos = getRandomEnemyPosition();
    
    // Create enemy with random health between 50-100
    float health = 50.0f + (float)rand() / (float)RAND_MAX * 50.0f;
    
    // Create and add the enemy
    Enemy enemy = createEnemy(name, pos.x, pos.y, pos.z, health);
    enemies.push_back(enemy);
    
    std::cout << "Spawned " << name << " at position (" << pos.x << ", " << pos.y << ", " << pos.z << ") with " << health << " HP" << std::endl;
}

// Draw an enemy
void drawEnemy(const Enemy& enemy) {
    if (!enemy.isAlive) {
        return;
    }
    
    glPushMatrix();
        glTranslatef(enemy.position.x, enemy.position.y, enemy.position.z);
        glRotatef(enemy.rotationY, 0.0f, 1.0f, 0.0f);
        glScalef(enemy.scale, enemy.scale, enemy.scale);
        
        // Enemy body - use a red color to differentiate from player
        glColor3f(0.8f, 0.2f, 0.2f);
        
        // Body
        glPushMatrix();
            glTranslatef(0.0f, 1.4f, 0.0f);
            glScalef(0.85f, 1.1f, 0.5f);
            glutSolidCube(1.0f);
        glPopMatrix();
        
        // Head
        glPushMatrix();
            glTranslatef(0.0f, 2.4f, 0.0f);
            glutSolidSphere(0.5f, 20, 20);
            
            // Eyes (white)
            glColor3f(1.0f, 1.0f, 1.0f);
            
            // Left eye
            glPushMatrix();
                glTranslatef(-0.15f, 0.1f, 0.4f);
                glutSolidSphere(0.12f, 10, 10);
                
                // Pupil (black)
                glColor3f(0.0f, 0.0f, 0.0f);
                glTranslatef(0.0f, 0.0f, 0.05f);
                glutSolidSphere(0.06f, 8, 8);
            glPopMatrix();
            
            // Right eye
            glColor3f(1.0f, 1.0f, 1.0f);
            glPushMatrix();
                glTranslatef(0.15f, 0.1f, 0.4f);
                glutSolidSphere(0.12f, 10, 10);
                
                // Pupil (black)
                glColor3f(0.0f, 0.0f, 0.0f);
                glTranslatef(0.0f, 0.0f, 0.05f);
                glutSolidSphere(0.06f, 8, 8);
            glPopMatrix();
            
            // Mouth (angry expression)
            glColor3f(0.0f, 0.0f, 0.0f);
            glPushMatrix();
                glTranslatef(0.0f, -0.1f, 0.45f);
                glScalef(0.25f, 0.05f, 0.05f);
                glutSolidCube(1.0f);
            glPopMatrix();
        glPopMatrix(); 
        
        // Arms - darker red
        glColor3f(0.6f, 0.1f, 0.1f);
        
        // Update enemy arm positions:

        // Left arm
        glPushMatrix();
        glTranslatef(-0.6f, 1.4f, 0.0f);
        glRotatef(enemy.leftArmRotation, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, 0.0f); // Changed from -0.1f to -0.5f for better arm length
        glScalef(0.3f, 1.0f, 0.3f);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Right arm
        glPushMatrix();
        glTranslatef(0.6f, 1.4f, 0.0f);
        glRotatef(enemy.rightArmRotation, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, 0.0f); // Changed from -0.1f to -0.5f for better arm length
        glScalef(0.3f, 1.0f, 0.3f);
        glutSolidCube(1.0f);
        glPopMatrix();
        
        // Legs - black like player's
        glColor3f(0.0f, 0.0f, 0.0f);
        
        // Left leg
        glPushMatrix();
            glTranslatef(-0.2f, 0.8f, 0.0f);
            glRotatef(enemy.leftLegRotation, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, -0.4f, 0.0f);
            glScalef(0.35f, 1.0f, 0.3f);
            glutSolidCube(1.0f);
        glPopMatrix();
        
        // Right leg
        glPushMatrix();
            glTranslatef(0.2f, 0.8f, 0.0f);
            glRotatef(enemy.rightLegRotation, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, -0.4f, 0.0f);
            glScalef(0.35f, 1.0f, 0.3f);
            glutSolidCube(1.0f);
        glPopMatrix();
        
        // Draw health bar above enemy
        drawEnemyHealthBar(enemy);
    glPopMatrix();
}

// Draw enemy health bar above the enemy
void drawEnemyHealthBar(const Enemy& enemy) {
    const float healthBarWidth = 1.0f;
    const float healthBarHeight = 0.15f;
    const float healthBarOffset = 3.0f; // Height above enemy
    
    // Health percentage
    float healthPercent = enemy.health / enemy.maxHealth;
    
    // Get player's position to make health bar face the player
    extern Player player;
    float dx = player.position.x - enemy.position.x;
    float dz = player.position.z - enemy.position.z;
    float angleToPlayer = atan2(dx, dz) * 180.0f / M_PI;
    
    // Save current attributes
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
    
    // Disable lighting for health bar
    glDisable(GL_LIGHTING);
    
    // Health bar background (dark gray)
    glColor3f(0.3f, 0.3f, 0.3f);
    glPushMatrix();
        glTranslatef(0.0f, healthBarOffset, 0.0f);
        glRotatef(angleToPlayer, 0.0f, 1.0f, 0.0f); // Rotate to face player
        glScalef(healthBarWidth, healthBarHeight, 0.1f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Current health (green to red based on health percentage)
    float red = 1.0f - healthPercent;
    float green = healthPercent;
    glColor3f(red, green, 0.0f);
    
    glPushMatrix();
        glTranslatef(-healthBarWidth * 0.5f * (1.0f - healthPercent), healthBarOffset, 0.01f);
        glRotatef(angleToPlayer, 0.0f, 1.0f, 0.0f); // Rotate to face player
        glScalef(healthBarWidth * healthPercent, healthBarHeight, 0.1f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Restore previous attributes
    glPopAttrib();
}

// Display enemy information on the right side of the screen
void displayEnemyInfo() {
    extern int windowWidth, windowHeight;
    
    // Save current matrix state
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Disable lighting for text
    glDisable(GL_LIGHTING);
    
    // Display enemy info
    int y = windowHeight - 20;
    int startX = windowWidth - 250;
    
    // Title
    glColor3f(1.0f, 0.5f, 0.5f);
    char title[50];
    sprintf(title, "Enemy List (%d/%d)", static_cast<int>(enemies.size()), MAX_ENEMIES);
    drawText(title, startX, y);
    y -= 20;
    
    // List each enemy
    for (const auto& enemy : enemies) {
        if (enemy.isAlive) {
            // Format: "Name: HP/MaxHP"
            char buffer[128];
            sprintf(buffer, "%s: %.0f/%.0f HP", enemy.name, enemy.health, enemy.maxHealth);
            
            // Color based on health percentage
            float healthPercent = enemy.health / enemy.maxHealth;
            glColor3f(1.0f - healthPercent, healthPercent, 0.0f);
            
            drawText(buffer, startX, y);
            y -= 20;
        }
    }
    
    // Restore matrix state
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    // Re-enable lighting
    glEnable(GL_LIGHTING);
}

void drawCrosshair(int width, int height) {
    extern bool isScopeMode;
    extern Player player;
    
    int size = 12; // Default crosshair size
    int thickness = 2; // Default thickness
    
    // Save current matrix state
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Disable lighting and depth test for UI elements
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    if (isScopeMode && player.combatMode == SHOOTING_MODE) {
        // Draw scope reticle
        int scopeSize = width / 4; // Size of the scope circle
        
        // Draw scope circle - black outline
        glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
        glLineWidth(3.0f);
        
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 360; i += 5) {
            float angle = i * M_PI / 180.0f;
            float x = width / 2 + scopeSize * cos(angle);
            float y = height / 2 + scopeSize * sin(angle);
            glVertex2f(x, y);
        }
        glEnd();
        
        // Draw crosshairs inside scope
        glLineWidth(1.5f);
        
        // Horizontal line
        glBegin(GL_LINES);
        glVertex2i(width/2 - scopeSize/2, height/2);
        glVertex2i(width/2 + scopeSize/2, height/2);
        glEnd();
        
        // Vertical line
        glBegin(GL_LINES);
        glVertex2i(width/2, height/2 - scopeSize/2);
        glVertex2i(width/2, height/2 + scopeSize/2);
        glEnd();
        
        // Small dot in the center
        glPointSize(3.0f);
        glBegin(GL_POINTS);
        glVertex2i(width/2, height/2);
        glEnd();
    } 
    else {
        // Standard crosshair (original code)
        // Draw crosshair (white with black outline)
        // Horizontal line (black outline)
        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(thickness + 2);
        glBegin(GL_LINES);
        glVertex2i(width/2 - size - 1, height/2);
        glVertex2i(width/2 + size + 1, height/2);
        glEnd();
        
        // Vertical line (black outline)
        glBegin(GL_LINES);
        glVertex2i(width/2, height/2 - size - 1);
        glVertex2i(width/2, height/2 + size + 1);
        glEnd();
        
        // Horizontal line (white)
        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(thickness);
        glBegin(GL_LINES);
        glVertex2i(width/2 - size, height/2);
        glVertex2i(width/2 + size, height/2);
        glEnd();
        
        // Vertical line (white)
        glBegin(GL_LINES);
        glVertex2i(width/2, height/2 - size);
        glVertex2i(width/2, height/2 + size);
        glEnd();
        
        // Add center dot (black outline)
        glColor3f(0.0f, 0.0f, 0.0f);
        glPointSize(3 + 2);
        glBegin(GL_POINTS);
        glVertex2i(width/2, height/2);
        glEnd();
        
        // Add center dot (white)
        glColor3f(1.0f, 1.0f, 1.0f);
        glPointSize(3);
        glBegin(GL_POINTS);
        glVertex2i(width/2, height/2);
        glEnd();
    }
    
    // Restore settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    
    // Restore matrix state
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void updateEnemies(float deltaTime) {
    // Check if we need to spawn new enemies
    int aliveCount = 0;
    for (const auto& enemy : enemies) {
        if (enemy.isAlive) {
            aliveCount++;
        }
    }
    
    // Spawn new enemies if needed
    if (aliveCount < MAX_ENEMIES) {
        // Random chance to spawn a new enemy (adjusted to be approximately every 3-5 seconds)
        if ((float)rand() / RAND_MAX < 0.01f) {
            spawnEnemy();
        }
    }
    
    // Update enemy animations and AI
    for (auto& enemy : enemies) {
        if (enemy.isAlive) {
            updateEnemyAI(enemy, deltaTime);
        }
    }
    
    // Update bullets
    updateBullets(deltaTime);
}

void spawnWaveEnemies(int count) {
    // Clear any existing enemies first
    clearAllEnemies();
    
    // Spawn the requested number of enemies
    for (int i = 0; i < count; i++) {
        spawnEnemy();
    }
    
    std::cout << "Spawned " << count << " enemies for the wave." << std::endl;
}

int getAliveEnemyCount() {
    int aliveCount = 0;
    for (const auto& enemy : enemies) {
        if (enemy.isAlive) {
            aliveCount++;
        }
    }
    return aliveCount;
}

void clearAllEnemies() {
    enemies.clear();
    std::cout << "Cleared all enemies from the scene." << std::endl;
}