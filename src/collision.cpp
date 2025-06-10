/*
 * ==========================================================================
 * COLLISION DETECTION SYSTEM IMPLEMENTATION
 * ==========================================================================
 * 
 * File: collision.cpp
 * Author: Project Team
 * Date: May 2, 2025
 * 
 * DESCRIPTION:
 * ----------- 
 * Implementasi sistem deteksi tabrakan (collision) untuk game, termasuk:
 * - Deteksi tabrakan antara player dengan dinding (walls)
 * - Deteksi tabrakan antara player dengan pagar (fences)
 * - Penentuan level permukaan di setiap posisi (ground level)
 * - Deteksi tabrakan menggunakan metode AABB (Axis-Aligned Bounding Box)
 * 
 * STRUCTURES:
 * ----------
 * BoundingBox - Didefinisikan di collision.h
 *   - Kotak pembatas dengan titik minimum dan maksimum (minX, minY, minZ, maxX, maxY, maxZ)
 *  
 * GLOBAL VARIABLES:
 * ---------------
 * walls  - Vector yang menyimpan semua definisi dinding (dari map.h)
 * fences - Vector yang menyimpan semua definisi pagar (dari map.h)
 * keys   - Array yang menyimpan status tombol keyboard (dari input.h)
 * 
 * FUNCTIONS:
 * ---------
 * checkAABBCollision() - Memeriksa apakah dua AABB saling bertabrakan
 *                        Mengembalikan true jika terjadi tabrakan
 * 
 * checkCollision()     - Memeriksa apakah player pada posisi baru akan bertabrakan dengan objek
 *                        Parameter: newX, newZ (koordinat posisi baru)
 *                        Mengembalikan true jika terjadi tabrakan
 * 
 * getGroundLevel()     - Menentukan ketinggian tanah pada posisi tertentu
 *                        Parameter: x, z (koordinat posisi)
 *                        Mengembalikan nilai float ketinggian tanah
 * 
 * DEPENDENCIES:
 * -----------
 * - collision.h - Header file dengan deklarasi fungsi dan struct BoundingBox
 * - map.h       - Header untuk akses ke struktur walls dan fences
 * - input.h     - Header untuk akses ke array keys (untuk debugging)
 * 
 * HOW TO USE:
 * ----------
 * 1. Panggil checkCollision(x, z) sebelum memindahkan entitas ke posisi baru
 * 2. Jika checkCollision() mengembalikan true, hindari pemindahan karena akan terjadi tabrakan
 * 3. Gunakan getGroundLevel(x, z) untuk mendapatkan ketinggian tanah pada posisi tertentu
 * 4. Aktifkan debug mode dengan menekan tombol 'L' untuk menampilkan informasi collision
 * 
 * EXAMPLE:
 * -------
 * // Cek tabrakan sebelum memindahkan player
 * float newX = player.position.x + moveX;
 * float newZ = player.position.z + moveZ;
 * 
 * if (!checkCollision(newX, newZ)) {
 *     // Pindahkan player ke posisi baru jika tidak ada tabrakan
 *     player.position.x = newX;
 *     player.position.z = newZ;
 *     
 *     // Update posisi Y berdasarkan ground level
 *     player.position.y = getGroundLevel(player.position.x, player.position.z);
 * }
 * 
 * ==========================================================================
 */

#include "collision.h"
#include "map.h"
#include "input.h"
#include <cmath>
#include <algorithm>
#include <bits/stdc++.h>

struct Doorway {
    float x1, z1, x2, z2;
    float baseY;
};

// Daftar pintu dan area yang bisa dilewati
std::vector<Doorway> doorways = {
    // Pintu depan lantai 1
    {-2.0f, 9.5f, 2.0f, 10.5f, 0.0f},
    
    // Pintu dalam lantai 1
    {-5.0f, 19.5f, -2.0f, 20.5f, 0.0f},
    {2.0f, 19.5f, 5.0f, 20.5f, 0.0f},
    
    // Pintu rooftop lantai 2
    {-2.0f, 19.5f, 2.0f, 20.5f, 4.0f}
};

bool isDoorwayArea(float x, float z) {
    for (const auto& door : doorways) {
        if (x >= door.x1 && x <= door.x2 && 
            z >= door.z1 && z <= door.z2) {
            return true;
        }
    }
    return false;
}

bool checkAABBCollision(const BoundingBox& a, const BoundingBox& b) {
    return (a.minX <= b.maxX && a.maxX >= b.minX &&
            a.minY <= b.maxY && a.maxY >= b.minY &&
            a.minZ <= b.maxZ && a.maxZ >= b.minZ);
}

CollisionResult checkWallCollision(float newX, float newZ, float oldX, float oldZ) {
    CollisionResult result = {false, false, false};

    // Get the player's current Y position
    extern Player player;
    float playerY = player.position.y;
    float playerHeight = 2.0f;
    
    // Create bounding boxes for both potential X-only and Z-only movements
    // This allows us to determine which axis is causing the collision
    // Create bounding boxes that respect player's height
    BoundingBox playerBoxXOnly = {
        newX - 0.3f, playerY, oldZ - 0.3f,
        newX + 0.3f, playerY + playerHeight, oldZ + 0.3f
    };
    
    BoundingBox playerBoxZOnly = {
        oldX - 0.3f, playerY, newZ - 0.3f,
        oldX + 0.3f, playerY + playerHeight, newZ + 0.3f
    };
    
    BoundingBox playerBoxFull = {
        newX - 0.3f, playerY, newZ - 0.3f,
        newX + 0.3f, playerY + playerHeight, newZ + 0.3f
    };

    // Debug info
    if (keys['l'] || keys['L']) {
        std::cout << "Checking sliding collision for position: (" << newX << "," << newZ << ")" << std::endl;
    }
    
    // Check if on stairs - special handling
    for (const auto& stair : staircases) {
        float stairEndZ = stair.startZ + (stair.stepDepth * stair.numSteps);
        
        // Check if player is in the stair's horizontal bounds
        if (newX >= stair.startX && newX <= stair.startX + stair.width) {
            // Prevent mounting stairs from the end (top)
            if (oldZ > stairEndZ && newZ < stairEndZ) {
                // Check if we're coming from a legitimate rooftop position
                float playerY = player.position.y;
                
                // If player is at proper rooftop height, allow them to enter stairs from top
                // This fixes the transition from rooftop to stairs
                if (std::abs(playerY - (stair.baseY + stair.stepHeight * stair.numSteps)) < 0.2f) {
                    // Player is at the right height, allow movement
                    return result;
                }
                
                // Otherwise block it (prevent teleporting from ground)
                result.collided = true;
                result.blockZ = true;
                return result;
            }
            
            // Normal stair mounting from start is still allowed
            if (newZ >= stair.startZ && newZ <= stairEndZ) {
                // Check collision with side barriers only
                if (newX < stair.startX || newX > stair.startX + stair.width) {
                    result.collided = true;
                    result.blockX = true;
                    return result;
                }
                
                // Let the player move freely along the staircase
                return result;
            }
        }
    }

    // Check doorways first - allow full movement through doorways
    if (isDoorwayArea(newX, newZ)) {
        return result; // No collision in doorways
    }
    
    // Special doorway handling
    if ((newX >= -2.0f && newX <= 2.0f && newZ >= 9.5f && newZ <= 10.5f) ||  // Front door
        (newX >= -5.0f && newX <= -2.0f && newZ >= 19.5f && newZ <= 20.5f) || // Left inner door
        (newX >= 2.0f && newX <= 5.0f && newZ >= 19.5f && newZ <= 20.5f) ||   // Right inner door
        (newX >= -2.0f && newX <= 2.0f && newZ >= 19.5f && newZ <= 20.5f && getGroundLevel(newX, newZ) >= 4.0f)) { // Rooftop door
        return result; // No collision in doorways
    }
    
    // Check collision with each wall
    for (const auto& wall : walls) {
        float minX = std::min(wall.x1, wall.x2) - wall.thickness / 2.0f;
        float maxX = std::max(wall.x1, wall.x2) + wall.thickness / 2.0f;
        float minZ = std::min(wall.z1, wall.z2) - wall.thickness / 2.0f;
        float maxZ = std::max(wall.z1, wall.z2) + wall.thickness / 2.0f;
        
        BoundingBox wallBox = {
            minX, wall.y, minZ,
            maxX, wall.y + wall.height, maxZ
        };

        // Only check collision if player is at wall height
        // Skip collision check if player is well above the wall
        if (playerY > wall.y + wall.height) {
            continue; // Player is above this wall, no collision possible
        }
        
        // Check if player collides with this wall using the full movement box
        if (checkAABBCollision(playerBoxFull, wallBox)) {
            result.collided = true;
            
            // Determine which axis is causing the collision
            bool collideX = checkAABBCollision(playerBoxXOnly, wallBox);
            bool collideZ = checkAABBCollision(playerBoxZOnly, wallBox);
            
            // Block the appropriate axis
            if (collideX) result.blockX = true;
            if (collideZ) result.blockZ = true;
            
            if (keys['l'] || keys['L']) {
                std::cout << "Wall collision detected: blockX=" << result.blockX 
                          << ", blockZ=" << result.blockZ << std::endl;
            }
            
            // If we're hitting the wall head-on (both axes blocked), exit early
            if (result.blockX && result.blockZ) {
                return result;
            }
        }
    }

    // Check rooftops for side collisions
    for (const auto& roof : rooftops) {
        // Skip collision check if player is above the rooftop (can walk on it)
        float playerY = 0.0f;
        extern Player player;
        playerY = player.position.y;
        
        // Only check collision with rooftop sides if player is below the rooftop surface
        if (playerY + 1.8f < roof.y) {
            // Create a bounding box for each side of the rooftop
            
            // Bottom edge (z1)
            BoundingBox bottomEdge = {
                roof.x1, roof.y - roof.thickness, roof.z1 - 0.1f,
                roof.x2, roof.y, roof.z1 + 0.1f
            };
            
            // Top edge (z2)
            BoundingBox topEdge = {
                roof.x1, roof.y - roof.thickness, roof.z2 - 0.1f,
                roof.x2, roof.y, roof.z2 + 0.1f
            };
            
            // Left edge (x1)
            BoundingBox leftEdge = {
                roof.x1 - 0.1f, roof.y - roof.thickness, roof.z1,
                roof.x1 + 0.1f, roof.y, roof.z2
            };
            
            // Right edge (x2)
            BoundingBox rightEdge = {
                roof.x2 - 0.1f, roof.y - roof.thickness, roof.z1,
                roof.x2 + 0.1f, roof.y, roof.z2
            };
            
            // Check collision with each edge
            if (checkAABBCollision(playerBoxFull, bottomEdge)) {
                result.collided = true;
                if (checkAABBCollision(playerBoxZOnly, bottomEdge)) result.blockZ = true;
            }
            
            if (checkAABBCollision(playerBoxFull, topEdge)) {
                result.collided = true;
                if (checkAABBCollision(playerBoxZOnly, topEdge)) result.blockZ = true;
            }
            
            if (checkAABBCollision(playerBoxFull, leftEdge)) {
                result.collided = true;
                if (checkAABBCollision(playerBoxXOnly, leftEdge)) result.blockX = true;
            }
            
            if (checkAABBCollision(playerBoxFull, rightEdge)) {
                result.collided = true;
                if (checkAABBCollision(playerBoxXOnly, rightEdge)) result.blockX = true;
            }
            
            if (result.blockX && result.blockZ) {
                return result;
            }
        }
    }
    
    // Check fences similarly
    for (const auto& fence : fences) {
        float minX = std::min(fence.startX, fence.endX) - fence.thickness / 2.0f;
        float maxX = std::max(fence.startX, fence.endX) + fence.thickness / 2.0f;
        float minZ = std::min(fence.startZ, fence.endZ) - fence.thickness / 2.0f;
        float maxZ = std::max(fence.startZ, fence.endZ) + fence.thickness / 2.0f;
        
        BoundingBox fenceBox = {
            minX, 0.0f, minZ,
            maxX, fence.height, maxZ
        };
        
        if (checkAABBCollision(playerBoxFull, fenceBox)) {
            result.collided = true;
            
            bool collideX = checkAABBCollision(playerBoxXOnly, fenceBox);
            bool collideZ = checkAABBCollision(playerBoxZOnly, fenceBox);
            
            if (collideX) result.blockX = true;
            if (collideZ) result.blockZ = true;
            
            if (result.blockX && result.blockZ) {
                return result;
            }
        }
    }
    
    return result;
}

bool checkCollision(float newX, float newZ) {
    CollisionResult result = checkWallCollision(newX, newZ, newX, newZ);
    return result.collided;
}

float getGroundLevel(float x, float z) {
    float groundLevel = 0.0f;
    float firstFloorRoofHeight = 6.5f;
    float secondFloorBaseY = firstFloorRoofHeight;

    // Check staircases
    for (const auto& stair : staircases) {
        float stairEndZ = stair.startZ + (stair.stepDepth * stair.numSteps);
        float finalStepHeight = stair.baseY + (stair.stepHeight * stair.numSteps);
        
        if (x >= stair.startX - 0.5f && x <= -9.8f) {
            // Area tangga kecuali step terakhir
            for (int i = 0; i < stair.numSteps - 1; i++) {  // Kurangi 1 untuk skip step terakhir
                float stepZ = stair.startZ + (stair.stepDepth * i);
                float nextStepZ = stepZ + stair.stepDepth;
                
                if (z >= stepZ - 0.2f && z <= nextStepZ + 0.2f) {
                    // Khusus untuk step terakhir, sambungkan dengan landing platform
                    if (i == stair.numSteps - 1) {
                        // Area transisi antara step terakhir dan landing
                        if (z >= nextStepZ - 0.3f) {
                            return finalStepHeight;
                        }
                    }
                    return stair.baseY + (stair.stepHeight * (i + 1));
                }
            }
            
            // Area landing platform
            if (z >= stairEndZ - 0.3f && z <= stairEndZ + 2.5f) {
                return finalStepHeight;  // Same height as last step
            }
        }
    }


    // Check atap lantai 1
    if (z >= 10.0f && z <= 30.0f && x >= -10.0f && x <= 10.0f) {
        extern Player player;
        if (player.position.y >= firstFloorRoofHeight - 0.5f) {
            return firstFloorRoofHeight;
        }
    }

    // Check doorways (ground level)
    if (x >= -2.0f && x <= 2.0f && z >= 9.5f && z <= 10.5f) {
        return 0.0f;
    }
    
    // First floor internal doors
    if ((x >= -5.0f && x <= -2.0f && z >= 19.5f && z <= 20.5f) ||
        (x >= 2.0f && x <= 5.0f && z >= 19.5f && z <= 20.5f)) {
        return 0.0f;
    }

    // Check lantai 2
    if (z >= 20.0f && z <= 30.0f && x >= -10.0f && x <= 10.0f) {
        extern Player player;
        if (player.position.y >= firstFloorRoofHeight - 0.5f) {
            return secondFloorBaseY; // Return ground level lantai 2
        }
    }

    return groundLevel;
}

bool isAtStairTransition(float x, float z, float y) {
    for (const auto& stair : staircases) {
        float stairEndZ = stair.startZ + (stair.stepDepth * stair.numSteps);
        float topStepHeight = stair.baseY + stair.numSteps * stair.stepHeight;
        
        // Check if at top of stairs (transition to second floor)
        if (x >= stair.startX && x <= stair.startX + stair.width &&
            z >= stairEndZ - 0.2f && z <= stairEndZ + 1.2f &&
            std::abs(y - topStepHeight) < 0.2f) {
            return true;
        }
        
        // Check if at bottom of stairs (transition from ground level)
        if (x >= stair.startX && x <= stair.startX + stair.width &&
            z >= stair.startZ - 0.2f && z <= stair.startZ + 0.2f &&
            y < 0.5f) {
            return true;
        }
    }
    
    return false;
}