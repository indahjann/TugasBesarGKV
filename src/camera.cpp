/*
 * ==========================================================================
 * CAMERA SYSTEM IMPLEMENTATION
 * ==========================================================================
 *  
 * File: camera.cpp
 * Author: Project Team
 * Date: May 2, 2025
 * 
 * DESCRIPTION:
 * -----------
 * Implementasi sistem kamera untuk game, termasuk:
 * - Kamera third-person (over-the-shoulder)
 * - Kamera first-person
 * - Mode scope untuk sistem penargetan
 * - Kamera saat mengendarai kendaraan
 * - Pergerakan kamera berbasis input mouse
 * 
 * CAMERA MODES:
 * -----------
 * - Third Person View: Kamera berada di belakang player dengan offset
 * - First Person View: Kamera berada pada posisi mata player
 * - Scope Mode: Mode zoom untuk penargetan yang lebih akurat
 * - Car Camera: Kamera saat player mengendarai kendaraan
 * 
 * GLOBAL VARIABLES: 
 * ---------------
 * cameraX, cameraY, cameraZ           - Posisi kamera di dunia
 * cameraLX, cameraLY, cameraLZ        - Vektor arah kamera (look at)
 * cameraRotationX, cameraRotationY    - Rotasi kamera dalam derajat
 * isFirstPersonView                   - Status mode first-person
 * isScopeMode                         - Status mode scope
 * defaultFOV, scopeFOV                - Field of View untuk tampilan normal dan scope
 * isDriving                           - Status sedang mengemudi kendaraan
 * 
 * FUNCTIONS:
 * ---------
 * cameraReshape()              - Menyesuaikan proyeksi kamera saat ukuran window berubah
 * cameraCenterMouse()          - Mengembalikan pointer mouse ke tengah layar
 * mouseMove()                  - Mengelola pergerakan kamera berdasarkan mouse
 * mousePassiveMove()           - Wrapper untuk mouseMove saat tidak ada tombol yang ditekan
 * orientMe()                   - Mengatur orientasi kamera berdasarkan sudut
 * movePlayerBasedOnCamera()    - Menggerakkan player relatif terhadap arah kamera
 * updateCameraForCar()         - Memperbarui posisi kamera saat mengendarai kendaraan
 * updateCameraForPlayer()      - Memperbarui posisi kamera untuk mode third-person
 * updateCameraFirstPerson()    - Memperbarui posisi kamera untuk mode first-person
 * updateCameraScopeThirdPerson() - Memperbarui posisi kamera untuk mode scope
 * updateCamera()               - Fungsi utama yang memperbarui kamera berdasarkan mode
 * 
 * DEPENDENCIES:
 * -----------
 * - camera.h  - Header file dengan deklarasi fungsi kamera
 * - object.h  - Header untuk akses ke struct Player dan Car
 * - GL/glut.h - Library untuk OpenGL
 * 
 * HOW TO USE:
 * ---------- 
 * 1. Panggil cameraReshape() saat ukuran window berubah
 * 2. Panggil mouseMove() untuk menangani input mouse
 * 3. Panggil updateCamera() di game loop dengan player saat ini
 * 4. Toggle isFirstPersonView untuk berganti mode kamera
 * 5. Toggle isScopeMode untuk mengaktifkan/menonaktifkan mode scope
 * 
 * ==========================================================================
 */

#include "camera.h"
#include <iostream>
#include <cmath>
#include <chrono> 
#include "object.h" 
#include "map.h"

extern std::vector<WallDefinition> walls;

// Camera parameters    
float cameraAngle = 0.0f, cameraRatio;
float cameraX = 0.0f, cameraY = 1.75f, cameraZ = 15.0f;
float cameraLX = 0.0f, cameraLY = 0.0f, cameraLZ = -1.0f;
float cameraTargetAngle = 0.0f, cameraCurrentAngle = 0.0f;
float cameraTargetLy = 0.0f, cameraCurrentLy = 0.0f;
float cameraSensitivity = 0.02f;
int mouseX, mouseY;
float cameraRotationY = 0.0f;
float cameraRotationX = 0.0f;

bool isFirstPersonView = false; 
bool isThirdPersonView = true; // Default to third-person view
bool isScopeMode = false; 
float defaultFOV = 45.0f;  
float scopeFOV = 30.0f; 
 
// Jumping parameters
float jumpSpeed = 0.1f;
float initialJumpVelocity = 0.02f;
float currentJumpVelocity = 0.0f;
std::chrono::time_point<std::chrono::steady_clock> jumpStartTime;
const std::chrono::seconds jumpDuration(3); 

// Idle mouse variables
auto lastMoveTime = std::chrono::steady_clock::now();
const auto idleThreshold = std::chrono::milliseconds(1); 

extern bool inMenu;
extern int windowWidth, windowHeight;

float movementDirectionX = 0.0f; // Arah gerak di sumbu X
float movementDirectionZ = 0.0f; // Arah gerak di sumbu Z

const float cameraDistance = 6.0f; 
const float cameraHeight = 1.8f; 

float cameraRotationAroundCar = 0.0f;

void cameraReshape(int w, int h) {
    cameraRatio = 1.0f * w / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45, cameraRatio, 0.1, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ, cameraX + cameraLX, cameraY + cameraLY, cameraZ + cameraLZ, 0.0f, 1.0f, 0.0f);
}

void cameraCenterMouse(int w, int h) {
    glutSetWindow(glutGetWindow()); 
    glutWarpPointer(w / 2, h / 2);
    mouseX = w / 2;
    mouseY = h / 2;
    lastMoveTime = std::chrono::steady_clock::now();
    std::cout << "Mouse centered at: " << mouseX << ", " << mouseY << std::endl;
}

void mouseMove(int x, int y) {
    extern int windowWidth, windowHeight;

    // Hitung pergerakan relatif mouse
    int dx = x - (windowWidth / 2);
    int dy = y - (windowHeight / 2);

    // Perbarui rotasi kamera
    cameraRotationY -= dx * cameraSensitivity;
    cameraRotationX += dy * cameraSensitivity;

    if (isFirstPersonView) {
        if (cameraRotationX > 30.0f) cameraRotationX = 30.0f;
        if (cameraRotationX < -60.0f) cameraRotationX = -60.0f;
    }
    else {
        if (isScopeMode) {
            if (cameraRotationX > 30.0f) cameraRotationX = 30.0f;
            if (cameraRotationX < -20.0f) cameraRotationX = -20.0f;
        } 
        else {
            if (cameraRotationX > 30.0f) cameraRotationX = 30.0f;
            if (cameraRotationX < 0.0f) cameraRotationX = 0.0f;
        }
        
    }
    
    glutWarpPointer(windowWidth / 2, windowHeight / 2);
}

void mousePassiveMove(int x, int y) {
    extern bool isPaused;
    
    // Only process camera movement if the game is not paused
    if (!isPaused) {
        mouseMove(x, y);
    }
}

void orientMe(float ang) {
    cameraLX = sin(ang);
    cameraLZ = -cos(ang);
}

void movePlayerBasedOnCamera(Player& player, float forward, float right) {
    // Hitung arah kamera
    float radY = player.rotationY * M_PI / 180.0f;

    // Hitung vektor gerakan relatif terhadap rotasi player
    float moveX = forward * sin(radY) + right * cos(radY);
    float moveZ = forward * cos(radY) - right * sin(radY);

    // Perbarui posisi player
    player.position.x += moveX;
    player.position.z += moveZ;
}

void updateCameraForPlayer(const Player& player) {
    // Hitung arah kamera berdasarkan rotasi kamera
    float radY = cameraRotationY * M_PI / 180.0f;
    float radX = cameraRotationX * M_PI / 180.0f;

    // PUBG-style camera parameters
    const float rightShoulderOffset = 1.2f;
    
    // Calculate right vector for offsetting camera to the side
    float rightX = cos(radY);
    float rightZ = sin(radY);
    
    // Hitung posisi kamera ideal (tanpa memperhitungkan oklusi)
    float idealCamX = player.position.x - sin(radY) * cameraDistance * cos(radX) + rightX * rightShoulderOffset;
    float idealCamY = player.position.y + cameraHeight + sin(radX) * cameraDistance;
    float idealCamZ = player.position.z - cos(radY) * cameraDistance * cos(radX) + rightZ * rightShoulderOffset;
    
    // Start position for occlusion check (player's eye position)
    float startX = player.position.x;
    float startY = player.position.y + 1.5f; // Eye level
    float startZ = player.position.z;
    
    // End position for occlusion check (desired camera position)
    float endX = idealCamX;
    float endY = idealCamY;
    float endZ = idealCamZ;
    
    // Check for occlusion and adjust camera position if necessary
    bool occluded = checkCameraOcclusion(startX, startY, startZ, endX, endY, endZ);
    
    // Set final camera position
    cameraX = endX;
    cameraY = endY;
    cameraZ = endZ;
    
    // Look at a point slightly ahead of player for better aiming
    float aimPointDistance = 20.0f; // How far ahead to focus the camera
    float aimPointX = player.position.x + sin(radY) * aimPointDistance;
    float aimPointY = player.position.y + 1.5f; // Eye level
    float aimPointZ = player.position.z + cos(radY) * aimPointDistance;
    
    // Calculate camera direction vector
    cameraLX = aimPointX - cameraX;
    cameraLY = aimPointY - cameraY;
    cameraLZ = aimPointZ - cameraZ;

    // Normalisasi vektor arah kamera
    float length = sqrt(cameraLX * cameraLX + cameraLY * cameraLY + cameraLZ * cameraLZ);
    cameraLX /= length;
    cameraLY /= length;
    cameraLZ /= length;

    glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ,
              aimPointX, aimPointY, aimPointZ,
              0.0f, 1.0f, 0.0f);
}

void updateCamera(const Player& player) {
    // Reset FOV jika tidak dalam scope mode
    if (!isScopeMode) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(defaultFOV, cameraRatio, 0.1, 1000);
        glMatrixMode(GL_MODELVIEW);
    }

    if (isFirstPersonView) {
        updateCameraFirstPerson(player); // First-person view
    } else {
        // Jika dalam scope mode tapi masih third person, gunakan tampilan scope khusus
        if (isScopeMode && player.combatMode == SHOOTING_MODE) {
            isFirstPersonView = true;
            updateCameraFirstPerson(player);
        } else {
            updateCameraForPlayer(player); // Third-person view normal
        }
    }
}

void updateCameraFirstPerson(const Player& player) {
    // Center camera exactly at player's eye position with no offsets that could show the model
    cameraX = player.position.x;
    cameraY = player.position.y + 1.8f; // Slightly lower for better eye level
    cameraZ = player.position.z;
    
    // Calculate look direction based on rotation
    float radY = cameraRotationY * M_PI / 180.0f;
    float radX = cameraRotationX * M_PI / 180.0f;
    
    // Calculate direction vector
    cameraLX = sin(radY) * cos(radX);
    cameraLY = -sin(radX);
    cameraLZ = cos(radY) * cos(radX);
    
    // Apply scope zoom effect if in scope mode
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    if (isScopeMode && player.combatMode == SHOOTING_MODE) {
        gluPerspective(scopeFOV, cameraRatio, 0.1, 1000);
    } else {
        gluPerspective(defaultFOV, cameraRatio, 0.1, 1000);
    }
    
    glMatrixMode(GL_MODELVIEW);
    
    // Set the view
    glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ,
              cameraX + cameraLX, cameraY + cameraLY, cameraZ + cameraLZ,
              0.0f, 1.0f, 0.0f);
}

void updateCameraScopeThirdPerson(const Player& player) {
    // Calculate camera direction based on rotation
    float radY = cameraRotationY * M_PI / 180.0f;
    float radX = cameraRotationX * M_PI / 180.0f;
    
    // Position camera slighly behind and above player's shoulder (closer than normal third person)
    const float scopeDistance = 1.5f; // Closer than regular third person
    const float rightShoulderOffset = 0.4f; // Offset to the right shoulder
    
    // Calculate right vector for offsetting camera to the side
    float rightX = cos(radY);
    float rightZ = sin(radY);
    
    // Calculate ideal camera position
    float idealCamX = player.position.x - sin(radY) * scopeDistance * cos(radX) + rightX * rightShoulderOffset;
    float idealCamY = player.position.y + 1.6f + sin(radX) * scopeDistance * 0.5f;
    float idealCamZ = player.position.z - cos(radY) * scopeDistance * cos(radX) + rightZ * rightShoulderOffset;
    
    // Start position for occlusion check (player's eye position)
    float startX = player.position.x;
    float startY = player.position.y + 1.6f; // Eye level
    float startZ = player.position.z + 3.0f;
    
    // End position for occlusion check (desired camera position)
    float endX = idealCamX;
    float endY = idealCamY;
    float endZ = idealCamZ;
    
    // Check for occlusion and adjust camera position if necessary
    bool occluded = checkCameraOcclusion(startX, startY, startZ, endX, endY, endZ);
    
    // Set final camera position
    cameraX = endX;
    cameraY = endY;
    cameraZ = endZ;
    
    // Rest of the function remains the same
    // Calculate aim point far ahead
    float aimPointDistance = 100.0f; // Look far ahead for precision
    float aimPointX = player.position.x + sin(radY) * aimPointDistance;
    float aimPointY = player.position.y + 1.5f - sin(radX) * aimPointDistance; // Adjust for vertical aim
    float aimPointZ = player.position.z + cos(radY) * aimPointDistance;
    
    // Set camera direction
    cameraLX = aimPointX - cameraX;
    cameraLY = aimPointY - cameraY;
    cameraLZ = aimPointZ - cameraZ;
    
    // Normalize camera direction vector
    float length = sqrt(cameraLX * cameraLX + cameraLY * cameraLY + cameraLZ * cameraLZ);
    cameraLX /= length;
    cameraLY /= length;
    cameraLZ /= length;
    
    // Apply scope zoom effect
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(scopeFOV, cameraRatio, 0.1, 1000);
    glMatrixMode(GL_MODELVIEW);
    
    // Set the view
    glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ,
              aimPointX, aimPointY, aimPointZ,
              0.0f, 1.0f, 0.0f);
}

bool checkCameraOcclusion(float startX, float startY, float startZ, 
                         float& endX, float& endY, float& endZ) {
    // Create a ray from player position (start) to desired camera position (end)
    float dirX = endX - startX;
    float dirY = endY - startY;
    float dirZ = endZ - startZ;
    
    // Normalize direction vector
    float length = sqrt(dirX * dirX + dirY * dirY + dirZ * dirZ);
    if (length < 0.0001f) return false;
    
    dirX /= length;
    dirY /= length;
    dirZ /= length;
    
    // Check intersection with walls
    for (const auto& wall : walls) {
        // Convert wall to AABB
        float minX = std::min(wall.x1, wall.x2) - wall.thickness/2;
        float maxX = std::max(wall.x1, wall.x2) + wall.thickness/2;
        float minZ = std::min(wall.z1, wall.z2) - wall.thickness/2;
        float maxZ = std::max(wall.z1, wall.z2) + wall.thickness/2;
        float minY = wall.y;
        float maxY = wall.y + wall.height;
        
        // Ray-AABB intersection test
        float tmin = -INFINITY, tmax = INFINITY;
        
        // Check X slab
        if (std::abs(dirX) < 0.0001f) {
            // Ray is parallel to X slab
            if (startX < minX || startX > maxX)
                continue; // No intersection with this wall
        } else {
            float t1 = (minX - startX) / dirX;
            float t2 = (maxX - startX) / dirX;
            
            tmin = std::max(tmin, std::min(t1, t2));
            tmax = std::min(tmax, std::max(t1, t2));
            
            if (tmin > tmax)
                continue; // No intersection
        }
        
        // Check Y slab
        if (std::abs(dirY) < 0.0001f) {
            if (startY < minY || startY > maxY)
                continue;
        } else {
            float t1 = (minY - startY) / dirY;
            float t2 = (maxY - startY) / dirY;
            
            tmin = std::max(tmin, std::min(t1, t2));
            tmax = std::min(tmax, std::max(t1, t2));
            
            if (tmin > tmax)
                continue;
        }
        
        // Check Z slab
        if (std::abs(dirZ) < 0.0001f) {
            if (startZ < minZ || startZ > maxZ)
                continue;
        } else {
            float t1 = (minZ - startZ) / dirZ;
            float t2 = (maxZ - startZ) / dirZ;
            
            tmin = std::max(tmin, std::min(t1, t2));
            tmax = std::min(tmax, std::max(t1, t2));
            
            if (tmin > tmax)
                continue;
        }
        
        // If we get here, we have an intersection
        if (tmin > 0 && tmin < length) {
            // Update camera position to be just in front of the wall
            // Add a small buffer to prevent z-fighting or clipping
            float buffer = 0.3f;
            float hitDistance = tmin - buffer;
            
            if (hitDistance < 0) hitDistance = 0;
            
            // Calculate new position along the ray
            endX = startX + dirX * hitDistance;
            endY = startY + dirY * hitDistance;
            endZ = startZ + dirZ * hitDistance;
            
            return true; // Occlusion detected and handled
        }
    }
    
    return false; // No occlusion
}
