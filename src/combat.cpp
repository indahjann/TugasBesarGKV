/*
 * ==========================================================================
 * COMBAT SYSTEM IMPLEMENTATION
 * ==========================================================================
 *  
 * File: combat.cpp
 * Author: Project Team
 * Date: May 2, 2025
 * 
 * DESCRIPTION:
 * ----------- 
 * Implementasi sistem pertarungan (combat) untuk game, termasuk:
 * - Manajemen peluru (bullets)
 * - Sistem penembakan
 * - Deteksi tabrakan peluru dengan musuh (bullet collision)
 * - Rendering senjata (pistol)
 * - Sistem serangan jarak dekat (melee attack)
 * 
 * STRUCTURES:
 * ----------
 * Bullet - Didefinisikan di combat.h
 *   - Peluru dengan properti posisi, arah, kecepatan, dan jarak tempuh
 *  
 * CONSTANTS:
 * ---------
 * BULLET_SPEED       - Kecepatan peluru (40.0 units/second)
 * BULLET_MAX_DISTANCE - Jarak maksimum yang dapat ditempuh peluru (100.0 units)
 * BULLET_DAMAGE      - Damage yang diberikan peluru ke musuh (35.0 hit points)
 * ENEMY_ATTACK_RANGE - Jarak serangan untuk player melee attack (didefinisikan di enemy.h)
 * ENEMY_DAMAGE       - Damage serangan player ke musuh (didefinisikan di enemy.h)
 * 
 * GLOBAL VARIABLES:
 * ---------------
 * bullets - Vector yang menyimpan semua peluru aktif di scene
 * 
 * FUNCTIONS:
 * ---------
 * createBullet()    - Membuat peluru baru dan menambahkannya ke vector bullets
 * updateBullets()   - Memperbarui posisi dan status semua peluru
 * drawBullets()     - Menggambar semua peluru aktif di scene
 * checkBulletHits() - Memeriksa tabrakan peluru dengan musuh
 * drawPistol()      - Menggambar model pistol
 * checkPlayerAttack() - Memeriksa apakah serangan jarak dekat player mengenai musuh
 * 
 * DEPENDENCIES:
 * -----------
 * - combat.h  - Header file dengan deklarasi fungsi dan struct Bullet
 * - enemy.h   - Header untuk akses ke struct Enemy dan vector enemies
 * - object.h  - Header untuk akses ke struct Position dan Player
 * 
 * HOW TO USE:
 * ----------
 * 1. Panggil createBullet() untuk membuat peluru baru
 * 2. Panggil updateBullets() di game loop untuk memperbarui peluru
 * 3. Panggil drawBullets() saat rendering untuk menampilkan peluru
 * 4. Panggil drawPistol() untuk menggambar senjata
 * 5. Panggil checkPlayerAttack() untuk memeriksa serangan jarak dekat
 * 
 * ==========================================================================
 */

#include "combat.h"
#include "enemy.h"
#include "gamemanager.h"

#include <bits/stdc++.h>
using namespace std; 

std::vector<Bullet> bullets;

// Bullet constants
const float BULLET_SPEED = 200.0f;        // Units per second
const float BULLET_MAX_DISTANCE = 100.0f; // Maximum travel distance
const float BULLET_DAMAGE = 35.0f;       // Damage per bullet

const float HEAD_DAMAGE_MULTIPLIER = 2.5f;   // Headshots deal 2.5x damage
const float BODY_DAMAGE_MULTIPLIER = 1.0f;   // Body shots deal normal damage
const float LEG_DAMAGE_MULTIPLIER = 0.6f;

// Create a new bullet
void createBullet(const Position& start, const Position& direction) {
    Bullet bullet;
    bullet.position = start;
    bullet.direction = direction;
    bullet.speed = BULLET_SPEED;
    bullet.distanceTraveled = 0.0f;
    bullet.maxDistance = BULLET_MAX_DISTANCE;
    bullet.active = true;
    
    bullets.push_back(bullet);
}

// Update all bullets
void updateBullets(float deltaTime) {
    // Update bullet positions
    for (auto& bullet : bullets) {
        if (bullet.active) {
            // Calculate distance to move this frame
            float moveDistance = bullet.speed * deltaTime;
            
            // Update position
            bullet.position.x += bullet.direction.x * moveDistance;
            bullet.position.y += bullet.direction.y * moveDistance;
            bullet.position.z += bullet.direction.z * moveDistance;
            
            // Update total distance traveled
            bullet.distanceTraveled += moveDistance;
            
            // Check if bullet should be deactivated
            if (bullet.distanceTraveled >= bullet.maxDistance) {
                bullet.active = false;
            }
        }
    }
    
    // Remove inactive bullets
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.active; }),
        bullets.end()
    );
    
    // Check for bullet hits on enemies
    checkBulletHits();
}

// Draw all bullets
void drawBullets() {
    // Save current attributes
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
    
    // Set bullet material properties
    glColor3f(1.0f, 0.8f, 0.0f); // Yellow bullet
    
    for (const auto& bullet : bullets) {
        if (bullet.active) {
            glPushMatrix();
                glTranslatef(bullet.position.x, bullet.position.y, bullet.position.z);
                
                // Draw bullet as a small sphere
                glutSolidSphere(0.1f, 8, 8);
            glPopMatrix();
        }
    }
    
    // Restore previous attributes
    glPopAttrib();
}

// Replace the existing checkBulletHits function with this improved version
void checkBulletHits() {
    for (auto& bullet : bullets) {
        if (!bullet.active) continue;
        
        for (auto& enemy : enemies) {
            if (!enemy.isAlive) continue;
            
            // Calculate distance from bullet to enemy
            float dx = bullet.position.x - enemy.position.x;
            float dy = bullet.position.y - (enemy.position.y + 1.4f); // Add offset for enemy height
            float dz = bullet.position.z - enemy.position.z;
            
            // Calculate squared distance (more efficient than using sqrt)
            float distSquared = dx*dx + dz*dz;
            
            // Define hit radius for enemy (make it a bit generous for better gameplay)
            float enemyHitRadius = 1.0f;
            float hitRadiusSquared = enemyHitRadius * enemyHitRadius;
            
            // Check if bullet is within enemy hit radius (horizontal check)
            if (distSquared <= hitRadiusSquared) {
                // Check vertical position for hit area (head, body, legs)
                float damageMultiplier = BODY_DAMAGE_MULTIPLIER;
                
                // Vertical position determines hit area
                // Head: 1.9-2.9 units above ground
                // Body: 0.8-1.9 units above ground
                // Legs: 0-0.8 units above ground
                float enemyHeight = enemy.position.y + 2.4f; // Total height with head
                float bulletHeight = bullet.position.y;
                
                // Check bullet height relative to enemy position
                if (bulletHeight >= enemyHeight - 0.5f) {
                    // Headshot
                    damageMultiplier = HEAD_DAMAGE_MULTIPLIER;
                    std::cout << "HEADSHOT!" << std::endl;
                } else if (bulletHeight >= enemy.position.y + 0.8f) {
                    // Body shot
                    damageMultiplier = BODY_DAMAGE_MULTIPLIER;
                    std::cout << "Body hit" << std::endl;
                } else {
                    // Leg shot
                    damageMultiplier = LEG_DAMAGE_MULTIPLIER;
                    std::cout << "Leg hit" << std::endl;
                }
                
                // Apply damage to enemy with appropriate multiplier
                float finalDamage = BULLET_DAMAGE * damageMultiplier;
                enemy.health -= finalDamage;
                
                std::cout << "Enemy " << enemy.name << " hit for " << finalDamage 
                          << " damage! Health: " << enemy.health << "/" << enemy.maxHealth << std::endl;
                
                // Check if enemy is killed
                if (enemy.health <= 0) {
                    enemy.isAlive = false;
                    registerEnemyKill(); // Update wave system
                    std::cout << "Enemy " << enemy.name << " killed!" << std::endl;
                }
                
                // Deactivate bullet
                bullet.active = false;
                break; // Exit the enemy loop - bullet can only hit one enemy
            }
        }
    }
}

void drawPistol(bool isRightHand) {
    // Apply mirroring if it's for left hand
    if (!isRightHand) {
        glPushMatrix();
        glScalef(-1.0f, 1.0f, 1.0f);
    }
    
    // Higher quality settings for curves
    GLUquadricObj *quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluQuadricNormals(quadric, GLU_SMOOTH);
    glPushMatrix();
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Rotate to face forward
        // Main body (slide) of the pistol - dark metallic gray
        glColor3f(0.25f, 0.25f, 0.27f);
        glPushMatrix();
            glScalef(0.14f, 0.12f, 0.45f);
            glutSolidCube(1.0f);
        glPopMatrix();
        
        // Slide serrations (grip texture on the slide)
        glColor3f(0.2f, 0.2f, 0.22f);
        for (int i = 0; i < 6; i++) {
            glPushMatrix();
                glTranslatef(0.0f, 0.06f, 0.05f - i * 0.04f);
                glScalef(0.141f, 0.01f, 0.02f);
                glutSolidCube(1.0f);
            glPopMatrix();
        }
        
        // Frame of the pistol - slightly lighter than slide
        glColor3f(0.3f, 0.3f, 0.32f);
        glPushMatrix();
            glTranslatef(0.0f, -0.02f, -0.05f);
            glScalef(0.13f, 0.08f, 0.35f);
            glutSolidCube(1.0f);
        glPopMatrix();
        
        // Handle/Grip of the pistol - textured black polymer
        glColor3f(0.12f, 0.12f, 0.12f);
        glPushMatrix();
            glTranslatef(0.0f, -0.18f, -0.07f);
            glRotatef(75.0f, 1.0f, 0.0f, 0.0f);
            glScalef(0.13f, 0.28f, 0.12f);
            glutSolidCube(1.0f);
        glPopMatrix();
        
        // Enhanced grip texture
        glColor3f(0.08f, 0.08f, 0.08f);
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 3; col++) {
                glPushMatrix();
                    float xPos = -0.05f + col * 0.05f;
                    float yPos = -0.3f + row * 0.05f;
                    glTranslatef(xPos, yPos, -0.13f);
                    glScalef(0.02f, 0.02f, 0.01f);
                    glutSolidCube(1.0f);
                glPopMatrix();
            }
        }
        
        // Barrel - dark metal
        glColor3f(0.15f, 0.15f, 0.15f);
        glPushMatrix();
            glTranslatef(0.0f, 0.01f, 0.25f);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            gluCylinder(quadric, 0.035f, 0.035f, 0.2f, 12, 2);
        glPopMatrix();
        
        // Enhanced barrel detail
        glPushMatrix();
            glTranslatef(0.0f, 0.01f, 0.25f);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glColor3f(0.1f, 0.1f, 0.1f);
            gluCylinder(quadric, 0.036f, 0.036f, 0.05f, 12, 1);
        glPopMatrix();
        
        // Muzzle with enhanced detail
        glPushMatrix();
            glTranslatef(0.0f, 0.01f, 0.45f);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glColor3f(0.05f, 0.05f, 0.05f);
            gluDisk(quadric, 0.0f, 0.035f, 12, 1);
            
            // Barrel interior (darker)
            glColor3f(0.02f, 0.02f, 0.02f);
            gluDisk(quadric, 0.0f, 0.02f, 8, 1);
        glPopMatrix();
        
        // Trigger guard - more ergonomic shape
        glColor3f(0.2f, 0.2f, 0.2f);
        glPushMatrix();
            // Front of trigger guard
            glTranslatef(0.0f, -0.08f, 0.05f);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            gluCylinder(quadric, 0.06f, 0.06f, 0.05f, 12, 1);
            
            // Bottom of trigger guard - elongated
            glTranslatef(0.0f, 0.0f, 0.05f);
            glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
            gluCylinder(quadric, 0.05f, 0.05f, 0.12f, 12, 1);
            
            // Back of trigger guard
            glTranslatef(0.12f, 0.0f, 0.0f);
            glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
            gluCylinder(quadric, 0.05f, 0.05f, 0.12f, 12, 1);
        glPopMatrix();
        
        // Enhanced trigger with angled design
        glColor3f(0.1f, 0.1f, 0.1f);
        glPushMatrix();
            glTranslatef(0.0f, -0.12f, -0.02f);
            glRotatef(-15.0f, 1.0f, 0.0f, 0.0f);
            glScalef(0.025f, 0.1f, 0.04f);
            glutSolidCube(1.0f);
        glPopMatrix();
        
        // Hammer
        glColor3f(0.18f, 0.18f, 0.18f);
        glPushMatrix();
            glTranslatef(0.0f, 0.07f, -0.16f);
            glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
            glScalef(0.04f, 0.08f, 0.04f);
            glutSolidCube(1.0f);
        glPopMatrix();
        
        // Front sight (brighter for visibility)
        glColor3f(0.9f, 0.9f, 0.9f);
        glPushMatrix();
            glTranslatef(0.0f, 0.07f, 0.4f);
            glScalef(0.02f, 0.05f, 0.02f);
            glutSolidCube(1.0f);
        glPopMatrix();
        
        // Rear sight with notch
        glColor3f(0.2f, 0.2f, 0.2f);
        glPushMatrix();
            glTranslatef(0.0f, 0.07f, -0.14f);
            glScalef(0.1f, 0.04f, 0.02f);
            glutSolidCube(1.0f);
        glPopMatrix();
        
        // Sight notch (black)
        glColor3f(0.0f, 0.0f, 0.0f);
        glPushMatrix();
            glTranslatef(0.0f, 0.07f, -0.14f);
            glScalef(0.03f, 0.05f, 0.03f);
            glutSolidCube(1.0f);
        glPopMatrix();
        
        // Add ejection port on slide
        glColor3f(0.1f, 0.1f, 0.1f);
        glPushMatrix();
            glTranslatef(0.07f, 0.06f, -0.05f);
            glScalef(0.01f, 0.01f, 0.15f);
            glutSolidCube(1.0f);
        glPopMatrix();
        
        // Close mirroring if it's for left hand
        if (!isRightHand) {
            glPopMatrix();
        }
    glPopMatrix();
    
    // Clean up
    gluDeleteQuadric(quadric);
}

// Check if player's attack hits an enemy
bool checkPlayerAttack(const Player& player, Enemy& enemy) {
    if (!enemy.isAlive) {
        return false;
    }
    
    // Calculate distance between player and enemy
    float dx = player.position.x - enemy.position.x;
    float dz = player.position.z - enemy.position.z;
    float distanceSq = dx * dx + dz * dz;
    
    // Check if enemy is within attack range
    if (distanceSq <= ENEMY_ATTACK_RANGE * ENEMY_ATTACK_RANGE) {
        // Check if player is facing the enemy
        // Calculate the angle between player's direction and enemy's position
        float angle = atan2(dx, dz) * 180.0f / M_PI;
        float playerAngle = fmod(player.rotationY + 360.0f, 360.0f);
        float angleDiff = fmod(fabs(angle - playerAngle) + 360.0f, 360.0f);
        
        // If player is facing the enemy (within a 90-degree arc)
        if (angleDiff < 45.0f || angleDiff > 315.0f) {
            // Apply damage to enemy
            enemy.health -= ENEMY_DAMAGE;
            
            // Check if enemy died
            if (enemy.health <= 0 && enemy.isAlive) {
                enemy.isAlive = false;
                std::cout << "Killed enemy " << enemy.name << "!" << std::endl;
                
                // Register the kill with the wave system
                registerEnemyKill();
            }
            
            return true;
        }
    }
    
    return false;
}
