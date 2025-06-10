/*
 * ==========================================================================
 * INPUT SYSTEM IMPLEMENTATION
 * ==========================================================================
 * 
 * File: input.cpp
 * Author: Project Team
 * Date: May 2, 2025
 * 
 * DESCRIPTION:
 * -----------
 * Implementasi sistem input untuk game, termasuk:
 * - Manajemen input keyboard untuk pergerakan karakter
 * - Manajemen input mouse untuk kamera dan interaksi
 * - Pemrosesan gerakan player berbasis input
 * - Mekanika lompatan dan gerakan fisik dasar
 * - Interaksi dengan menu dan UI 
 * - Implementasi mode combat (serangan pukulan dan tembakan)
 * 
 * GLOBAL VARIABLES: 
 * ---------------
 * keys[]                  - Array untuk menyimpan status tombol keyboard
 * movementDirectionX/Z    - Vektor arah pergerakan player
 * isJumping              - Status lompatan player
 * jumpHeight             - Tinggi lompatan saat ini
 * jumpVelocity           - Kecepatan awal lompatan
 * gravity                - Faktor gravitasi untuk lompatan
 * speed                  - Kecepatan pergerakan saat ini
 * isMoving               - Status pergerakan player
 * isShiftPressed         - Status tombol Shift (untuk sprint)
 * 
 * FUNCTIONS:
 * ---------
 * keyboard()              - Menangani event penekanan tombol
 * keyboardUp()            - Menangani event pelepasan tombol
 * restartScene()          - Mereset scene ke keadaan awal
 * mouseButton()           - Menangani event klik mouse
 * validateKeyStates()     - Memvalidasi status tombol keyboard
 * processKeys()           - Memproses input keyboard untuk gerakan player
 * 
 * MOVEMENT SYSTEM:
 * --------------
 * - Sistem gerakan WASD dengan rotasi kamera
 * - Gerakan diagonal dengan kombinasi tombol
 * - Sprint dengan tombol Shift
 * - Lompatan dengan Space
 * - Animasi berjalan otomatis saat bergerak
 * 
 * DEPENDENCIES:
 * -----------
 * - input.h     - Header file dengan deklarasi fungsi input
 * - camera.h    - Header untuk akses ke fungsi kamera
 * - collision.h - Header untuk deteksi tabrakan
 * - render.h    - Header untuk fungsi rendering
 * - menu.h      - Header untuk interaksi menu
 * - enemy.h     - Header untuk interaksi dengan musuh
 * - combat.h    - Header untuk sistem pertarungan
 * - windows.h   - Library untuk fungsi Windows API (GetAsyncKeyState)
 * 
 * HOW TO USE:
 * ----------
 * 1. Panggil processKeys(player) di game loop untuk memproses input keyboard
 * 2. Daftarkan keyboard(), keyboardUp(), dan mouseButton() ke callback GLUT:
 *    glutKeyboardFunc(keyboard);
 *    glutKeyboardUpFunc(keyboardUp);
 *    glutMouseFunc(mouseButton);
 * 3. Gunakan validateKeyStates() untuk memastikan status tombol keyboard konsisten
 * 
 * KEY CONTROLS:
 * -----------
 * W/A/S/D     - Gerakan karakter
 * Space       - Lompat
 * Shift       - Sprint (gerakan cepat)
 * 1           - Beralih ke mode pukulan
 * 2           - Beralih ke mode tembak
 * Y           - Toggle antara kamera first-person dan third-person
 * Klik Kanan  - Aktifkan/nonaktifkan scope mode 
 * Klik Kiri   - Serang (pukul/tembak berdasarkan mode combat)
 * Backspace   - Reset scene
 * Esc         - Keluar aplikasi
 * 
 * ==========================================================================
 */

#include <GL/glut.h>
#include "input.h"
#include "camera.h" 
#include "collision.h" 
#include "render.h"
#include "menu.h"
#include "enemy.h"
#include "combat.h"
#include <cmath>
#include <windows.h>
#include "gamemanager.h"
 
#include <iostream> 
   
bool keys[256]; 
bool isDragging = false;
bool isShiftPressed = false; 
const float moveSpeed = 4.0f; 
bool isJumping = false;
float jumpHeight = 0.0f;
float jumpVelocity = 4.0f;   
float gravity = 5.0f;     
float speed;
bool isMoving = false;
float currentMoveSpeed;


void keyboard(unsigned char key, int x2, int y2) {
    keys[key] = true;
    extern bool inMenu;
    extern bool isPaused;
    extern int windowWidth, windowHeight;
    extern Player player;
    extern bool isFirstPersonView;

    if (key == 27) { // Escape key
        // Toggle pause menu instead of exiting
        if (!inMenu) { // Only toggle pause menu if not in main menu
            togglePauseMenu();
        }
    } else if (key == 8) { // Backspace key
        restartScene(); 
    } else if (key == '1') {
        player.combatMode = PUNCH_MODE; 
        std::cout << "Switched to Punch Mode" << std::endl;
    } else if (key == '2') {
        player.combatMode = SHOOTING_MODE;
        std::cout << "Switched to Shooting Mode" << std::endl;
    } else if (key == 'y' || key == 'Y') { 
        // Toggle camera perspective
        isFirstPersonView = !isFirstPersonView;
        isThirdPersonView = !isThirdPersonView; // Ensure only one view is active
        std::cout << "Camera view: " << (isFirstPersonView ? "First Person" : "Third Person") << std::endl;
    }
}

void keyboardUp(unsigned char key, int x2, int y2) {
    keys[key] = false;

    // Pastikan animasi berhenti jika semua tombol gerakan dilepas
    if (!(keys['w'] || keys['W'] || keys['s'] || keys['S'] || keys['a'] || keys['A'] || keys['d'] || keys['D'])) {
        extern Player* pl;
        updatePlayerAnimation(*pl, 0.0f, false);
    }
}

void restartScene() {
    // Reset posisi player
    extern Player* pl;
    pl->position.x = 0.0f;
    pl->position.y = 0.0f;
    pl->position.z = 0.0f;
    pl->rotationY = 0.0f;
    pl->leftArmRotation = 0.0f;
    pl->rightArmRotation = 0.0f;
    pl->leftLegRotation = 0.0f;
    pl->rightLegRotation = 0.0f;
    pl->isPunching = false;
    pl->punchProgress = 0.0f;

    // Reset variabel lompatan
    isJumping = false;
    jumpHeight = 0.0f;

    // Reset variable gerakan
    movementDirectionX = 0.0f;
    movementDirectionZ = 0.0f;
    speed = 0.0f;
    currentMoveSpeed = 0.0f;
    isShiftPressed = false; // Reset isShiftPressed
    isMoving = false; // Reset isMoving

    // Reset kamera
    extern float cameraRotationY, cameraRotationX;
    cameraRotationY = 0.0f;
    cameraRotationX = 0.0f;

    std::cout << "Scene restarted!" << std::endl;
}

void mouseButton(int button, int state, int x, int y) {
    extern bool inMenu;
    extern bool isPaused;
    extern int windowWidth, windowHeight;
    extern Player player;
    extern bool isScopeMode; 
    extern bool isFirstPersonView;

    if (inMenu) {
        handleMenuClick(button, state, x, y, windowWidth, windowHeight);
    } else if (isPaused) {
        handlePauseMenuClick(button, state, x, y, windowWidth, windowHeight);
    } else if (gameState == GAME_WIN) {
        handleWinScreenClick(button, state, x, y, windowWidth, windowHeight);
    } else {
        if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && player.combatMode == SHOOTING_MODE && isThirdPersonView == true) {
            isFirstPersonView = !isFirstPersonView; // Toggle first-person view
            isScopeMode = !isScopeMode; // Disable scope mode when switching to first-person
        }
        else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && player.combatMode == SHOOTING_MODE) {
            isScopeMode = !isScopeMode;
        }
        else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            if (player.combatMode == PUNCH_MODE) {
                // Punch attack
                if (!player.isPunching) {
                    player.isPunching = true;
                    player.punchProgress = 0.0f;
                }
            } else if (player.combatMode == SHOOTING_MODE) {
                // Shoot attack
                if (player.shootCooldown <= 0.0f) {
                    // Get direction from camera view for shooting
                    extern float cameraLX, cameraLY, cameraLZ;
                    
                    // Create bullet from player's position
                    Position start = {
                        player.position.x,
                        player.position.y + 1.4f, // Adjust to be at gun level
                        player.position.z
                    };
                    
                    // Gunakan vektor arah 3D lengkap termasuk komponen Y
                    Position direction = {
                        cameraLX,
                        cameraLY, // Tambahkan komponen vertikal
                        cameraLZ
                    };
                    
                    // Normalize vektor arah 3D
                    float length = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
                    direction.x /= length;
                    direction.y /= length;
                    direction.z /= length;
                    
                    // Rotate player to face shooting direction (rotasi horizontal saja)
                    player.rotationY = atan2(direction.x, direction.z) * 180.0f / M_PI;
                    
                    // Create the bullet
                    createBullet(start, direction);
                    
                    // Set cooldown
                    player.shootCooldown = 0.0f; // 0.5 second cooldown
                    
                    std::cout << "Shot fired in direction: " << direction.x << ", " << direction.y << ", " << direction.z << std::endl;
                }
            }
        }
    }
}

void validateKeyStates() {
    // Check if W is released according to Windows API
    if (!(GetAsyncKeyState('W') & 0x8000) && !(GetAsyncKeyState('w') & 0x8000)) {
        keys['W'] = false;
        keys['w'] = false;
    }
    
    // Check other movement keys too
    if (!(GetAsyncKeyState('A') & 0x8000) && !(GetAsyncKeyState('a') & 0x8000)) {
        keys['A'] = false;
        keys['a'] = false;
    }
    
    if (!(GetAsyncKeyState('S') & 0x8000) && !(GetAsyncKeyState('s') & 0x8000)) {
        keys['S'] = false;
        keys['s'] = false;
    }
    
    if (!(GetAsyncKeyState('D') & 0x8000) && !(GetAsyncKeyState('d') & 0x8000)) {
        keys['D'] = false;
        keys['d'] = false;
    }
}

void processKeys(Player& player) {
    static auto lastFrameTime = std::chrono::steady_clock::now();
    auto currentFrameTime = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
    lastFrameTime = currentFrameTime;

    // Update shoot cooldown
    if (player.shootCooldown > 0.0f) {
        player.shootCooldown -= deltaTime;
    }

    validateKeyStates();

    const float rotationOffset = 90.0f; // Rotasi tambahan untuk A dan D

    // Reset arah gerak
    movementDirectionX = 0.0f;
    movementDirectionZ = 0.0f;

    // Periksa apakah tombol gerakan ditekan
    bool wasPreviouslyMoving = isMoving;
    isMoving = keys['w'] || keys['W'] || keys['a'] || keys['A'] || keys['s'] || keys['S'] || keys['d'] || keys['D'];
    
    // Jika baru saja berhenti bergerak (transisi dari bergerak ke tidak bergerak)
    if (wasPreviouslyMoving && !isMoving) {
        // Reset semua variabel gerakan
        isShiftPressed = false;
        speed = 0.0f;
        currentMoveSpeed = 0.0f;
        movementDirectionX = 0.0f;
        movementDirectionZ = 0.0f;
    }
    
    // Cek Shift hanya jika tombol gerakan ditekan dan karakter tidak sedang lompat
    isShiftPressed = (isMoving && !isJumping) ? ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0) : false;

    // Hitung kecepatan berdasarkan status tombol (tidak menggunakan shift jika sedang lompat)
    if (isMoving) {
        if (isShiftPressed && !isJumping) {
            // Kecepatan sprint (hanya jika tidak sedang lompat)
            speed = moveSpeed * 7.0f;
        } else {
            // Kecepatan normal
            speed = moveSpeed;
        }
    } else {
        speed = 0.0f;
    }
    float currentSpeed = speed * deltaTime;
    
    // Hanya proses gerakan jika ada tombol yang ditekan
    if (isMoving) {
        // Periksa kombinasi tombol untuk gerakan diagonal
        bool keyW = keys['w'] || keys['W'];
        bool keyS = keys['s'] || keys['S'];
        bool keyA = keys['a'] || keys['A'];
        bool keyD = keys['d'] || keys['D'];
        
        // Gerakan diagonal W+A (miring ke kiri depan 45 derajat)
        if (keyW && keyA && !keyS && !keyD) {
            float diagonal = cameraRotationY + 45.0f; // 45 derajat ke kiri dari arah depan
            float rad = diagonal * M_PI / 180.0f;
            movementDirectionX = sin(rad);
            movementDirectionZ = cos(rad);
            player.rotationY = diagonal;
        }
        // Gerakan diagonal W+D (miring ke kanan depan 45 derajat)
        else if (keyW && keyD && !keyS && !keyA) {
            float diagonal = cameraRotationY - 45.0f; // 45 derajat ke kanan dari arah depan
            float rad = diagonal * M_PI / 180.0f;
            movementDirectionX = sin(rad);
            movementDirectionZ = cos(rad);
            player.rotationY = diagonal;
        }
        // Gerakan diagonal S+A (miring ke kiri belakang 45 derajat)
        else if (keyS && keyA && !keyW && !keyD) {
            float diagonal = cameraRotationY + 135.0f; // 45 derajat ke kiri dari arah belakang
            float rad = diagonal * M_PI / 180.0f;
            movementDirectionX = sin(rad);
            movementDirectionZ = cos(rad);
            player.rotationY = diagonal;
        }
        // Gerakan diagonal S+D (miring ke kanan belakang 45 derajat)
        else if (keyS && keyD && !keyW && !keyA) {
            float diagonal = cameraRotationY - 135.0f; // 45 derajat ke kanan dari arah belakang
            float rad = diagonal * M_PI / 180.0f;
            movementDirectionX = sin(rad);
            movementDirectionZ = cos(rad);
            player.rotationY = diagonal;
        }
        // Jika tidak ada kombinasi diagonal yang cocok, proses tombol individual seperti biasa
        else {
            if (keyW) {
                movementDirectionX += sin(cameraRotationY * M_PI / 180.0f);
                movementDirectionZ += cos(cameraRotationY * M_PI / 180.0f);
                player.rotationY = cameraRotationY;
            }
            if (keyS) {
                movementDirectionX -= sin(cameraRotationY * M_PI / 180.0f);
                movementDirectionZ -= cos(cameraRotationY * M_PI / 180.0f);
                player.rotationY = cameraRotationY + 180.0f;
            }
            if (keyA) {
                float rad = (cameraRotationY + rotationOffset) * M_PI / 180.0f;
                movementDirectionX += sin(rad);
                movementDirectionZ += cos(rad);
                player.rotationY = cameraRotationY + 90.0f;
            }
            if (keyD) {
                float rad = (cameraRotationY - rotationOffset) * M_PI / 180.0f;
                movementDirectionX += sin(rad);
                movementDirectionZ += cos(rad);
                player.rotationY = cameraRotationY - 90.0f;
            }
        }

        // Normalisasi arah gerak
        float length = sqrt(movementDirectionX * movementDirectionX + 
                          movementDirectionZ * movementDirectionZ);
        if (length > 0.0f) {
            movementDirectionX /= length;
            movementDirectionZ /= length;
            
            // Calculate new position
            float newX = player.position.x + movementDirectionX * currentSpeed;
            float newZ = player.position.z + movementDirectionZ * currentSpeed;
            
            // Check for collision before updating position
            if (!checkCollision(newX, newZ)) {
                // Update position only if no collision
                player.position.x = newX;
                player.position.z = newZ;
                
                // Update Y position based on ground level if not jumping
                if (!isJumping) {
                    float groundLevel = getGroundLevel(player.position.x, player.position.z);
                    player.position.y = groundLevel;
                }
            }
        }
    }
    else {
        // Pastikan kecepatan benar-benar 0 ketika tidak ada tombol WASD ditekan
        speed = 0.0f;
        currentMoveSpeed = 0.0f;
        // Reset arah gerakan
        movementDirectionX = 0.0f;
        movementDirectionZ = 0.0f;
    }

    // Calculate the intended movement vector
    float moveX = movementDirectionX * currentSpeed;
    float moveZ = movementDirectionZ * currentSpeed;
    
    if (isMoving) {
        // Calculate intended new position
        float newX = player.position.x + moveX;
        float newZ = player.position.z + moveZ;
        
        // Check for collision with wall sliding
        CollisionResult collision = checkWallCollision(newX, newZ, player.position.x, player.position.z);
        
        if (collision.collided) {
            // Wall sliding logic - allow movement along non-blocked axes
            if (!collision.blockX) {
                player.position.x = newX;
            }
            
            if (!collision.blockZ) {
                player.position.z = newZ;
            }
            
            if (keys['l'] || keys['L']) {
                std::cout << "Sliding along wall: x=" << (!collision.blockX ? "allowed" : "blocked")
                          << ", z=" << (!collision.blockZ ? "allowed" : "blocked") << std::endl;
            }
        } else {
            // No collision, move freely
            player.position.x = newX;
            player.position.z = newZ;
        }
        
        // Update Y position based on ground level
        if (!isJumping) {
            float groundLevel = getGroundLevel(player.position.x, player.position.z);
            player.position.y = groundLevel;
        }
    }

    // Update animasi berdasarkan status gerakan
    updatePlayerAnimation(player, deltaTime, isMoving);
    // Add this line to update punch animation
    updatePunchAnimation(player, deltaTime);
    
    // Proses lompatan dengan deltaTime
    if (keys[' '] && !isJumping) {
        isJumping = true;
        jumpHeight = jumpVelocity;
    }

    if (isJumping) {
        player.position.y += jumpHeight * deltaTime;
        jumpHeight -= gravity * deltaTime * 1.5f; // Mengurangi tinggi lompatan seiring waktu
        
        // Check if player would land on a block
        float groundLevel = getGroundLevel(player.position.x, player.position.z);
        
        if (player.position.y <= groundLevel) {
            player.position.y = groundLevel; // Land on the block or ground
            isJumping = false;
            jumpHeight = 0.0f;
        }
    } else if (!isJumping) {
        // Always make sure the player is at the correct ground level
        float groundLevel = getGroundLevel(player.position.x, player.position.z);
        player.position.y = groundLevel;
    }
}