#include <bits/stdc++.h>
#include <GL/glut.h>
#include <stdio.h>
#include <chrono>
#include "input.h"
#include "camera.h"
#include "render.h"
#include "lighting.h"
#include "menu.h"
#include "collision.h"
#include "object.h"
#include "map.h"  
#include "enemy.h"
#include "scenery.h"
#include "gamemanager.h"
#include "combat.h"
   
Player* pl = &player; 

// Global Variables 
int windowWidth = 640; 
int windowHeight = 480;
bool inMenu = true;   

void display() { 
    static auto lastFrameTime = std::chrono::steady_clock::now(); // Waktu frame sebelumnya
    auto currentFrameTime = std::chrono::steady_clock::now();     // Waktu frame saat ini
    float deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count(); // Hitung deltaTime
    lastFrameTime = currentFrameTime; // Perbarui waktu frame sebelumnya

    if (inMenu) {
        drawMenu(windowWidth, windowHeight); // Pass width and height
    } else if (gameState == GAME_WIN) {
        drawWinScreen(windowWidth, windowHeight);
    } else if (gameState == GAME_WAVE_TRANSITION) {
        // Draw the game scene in the background
        glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        renderScene();
        
        // Draw wave transition overlay
        drawWaveTransitionScreen(windowWidth, windowHeight);
        
        // Update game state
        updateGameState(deltaTime);
        
        glutSwapBuffers();
    }
    else {
        // Atur warna latar belakang (langit) menjadi biru muda
        glClearColor(0.529f, 0.808f, 0.922f, 1.0f); // Contoh: Warna biru langit
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // Only process input and update game state if not paused
        if (!isPaused) {
            processKeys(*pl);
            updateCamera(*pl);
            updateEnemies(deltaTime);
            updateGameState(deltaTime);
        }
        
        renderScene();     // Gambar scene

        // Tampilkan informasi di layar
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, windowWidth, 0, windowHeight);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        if (!isPaused) {
            glColor3f(0.0f, 0.0f, 0.0f);

            if (isFirstPersonView && player.combatMode == SHOOTING_MODE) {
                // Draw hands and weapon in first-person perspective
                glMatrixMode(GL_PROJECTION);
                glPushMatrix();
                glLoadIdentity();
                gluPerspective(60.0, cameraRatio, 0.1, 10.0);
                
                glMatrixMode(GL_MODELVIEW);
                glPushMatrix();
                glLoadIdentity();
                
                // Position the gun in view
                if (isScopeMode) {
                    glTranslatef(0.0f, -0.3f, -1.0f); // Adjust for scope mode
                } else {
                    glTranslatef(0.3f, -0.3f, -0.9f); // Normal view
                }
                glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                drawPistol(true);
                
                glMatrixMode(GL_PROJECTION);
                glPopMatrix();
                glMatrixMode(GL_MODELVIEW);
                glPopMatrix();
            }

            char buffer[128];
            snprintf(buffer, sizeof(buffer), "Rotation Y: %.2f", pl->rotationY);
            drawText(buffer, 10, windowHeight - 20);

            snprintf(buffer, sizeof(buffer), "Position X: %.2f", pl->position.x);
            drawText(buffer, 10, windowHeight - 40); 
            snprintf(buffer, sizeof(buffer), "Position Y: %.2f", pl->position.y);
            drawText(buffer, 10, windowHeight - 60);
    
            snprintf(buffer, sizeof(buffer), "Position Z: %.2f", pl->position.z);
            drawText(buffer, 10, windowHeight - 80);

            snprintf(buffer, sizeof(buffer), "playerRotationY: %.2f", player.rotationY);
            drawText(buffer, 10, windowHeight - 100);

            snprintf(buffer, sizeof(buffer), "cameraRotationY: %.2f", cameraRotationY);
            drawText(buffer, 10, windowHeight - 120);

            snprintf(buffer, sizeof(buffer), "isMoving: %s", isMoving ? "true" : "false");
            drawText(buffer, 10, windowHeight - 180);

            // Tambahkan di fungsi display()
            // Tambahkan sebelum glutSwapBuffers()

            // Debug collision detection
            if (keys['l'] || keys['L']) {
                snprintf(buffer, sizeof(buffer), "Debug Mode: ON");
                drawText(buffer, 10, windowHeight - 220);
                
                snprintf(buffer, sizeof(buffer), "Player Box: %.1f,%.1f to %.1f,%.1f", 
                        pl->position.x - 0.3f, pl->position.z - 0.3f,
                        pl->position.x + 0.3f, pl->position.z + 0.3f);
                drawText(buffer, 10, windowHeight - 240);
            }

            // Tampilkan FPS
            float fps = calculateFPS();
            snprintf(buffer, sizeof(buffer), "FPS: %.2f", fps);
            drawText(buffer, 10, windowHeight - 140);

            snprintf(buffer, sizeof(buffer), "Speed: %.2f", speed);
            drawText(buffer, 10, windowHeight - 160);

            bool isColliding = checkCollision(player.position.x, player.position.z);
            snprintf(buffer, sizeof(buffer), "Collision: %s", isColliding ? "true" : "false");
            drawText(buffer, 10, windowHeight - 200);

            // In the UI drawing section, add:
            snprintf(buffer, sizeof(buffer), "Combat Mode: %s", 
                    player.combatMode == PUNCH_MODE ? "Punch" : "Shooting");
            drawText(buffer, 10, windowHeight - 240);

            if (player.combatMode == SHOOTING_MODE) {
                snprintf(buffer, sizeof(buffer), "Cooldown: %.1f", 
                        player.shootCooldown > 0 ? player.shootCooldown : 0.0f);
                drawText(buffer, 10, windowHeight - 260);
            }
            char waveBuffer[128];

            snprintf(waveBuffer, sizeof(waveBuffer), "Wave: %d/%d", 
                    waveSystem.currentWave, waveSystem.totalWaves);
            drawText(waveBuffer, 10, windowHeight - 280);

            snprintf(waveBuffer, sizeof(waveBuffer), "Enemies: %d/%d", 
                    waveSystem.enemiesKilled, waveSystem.enemiesPerWave);
            drawText(waveBuffer, 10, windowHeight - 300);

            // Display game state
            const char* stateText = "Unknown";
            switch(gameState) {
                case GAME_MENU: stateText = "Menu"; break;
                case GAME_PLAYING: stateText = "Playing"; break;
                case GAME_PAUSED: stateText = "Paused"; break;
                case GAME_WAVE_TRANSITION: stateText = "Wave Transition"; break;
                case GAME_WIN: stateText = "Win"; break;
                case GAME_LOSE: stateText = "Lose"; break;
            }
            snprintf(waveBuffer, sizeof(waveBuffer), "Game State: %s", stateText);
            drawText(waveBuffer, 10, windowHeight - 320);

            // Restore matrices
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();

            // Draw crosshair if not in menu or pause
            drawCrosshair(windowWidth, windowHeight);
        } else {
            // Restore matrices before drawing pause menu
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
            
            // Draw pause menu on top of frozen game scene
            drawPauseMenu(windowWidth, windowHeight);
        }
        glutSwapBuffers();
    }
}

void Reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    cameraReshape(w, h);  
}

void init()
{
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    loadTexture(); 
    initMap();
    drawBorderMap();
    initScenery(); 

    player = createPlayer("Player", 0.0f, 0.0f, 0.0f);
    initEnemySystem();

    initGameSystem(); 
    
    gameState = GAME_MENU;
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Exploring Game");
    glutSetCursor(GLUT_CURSOR_NONE);

    glutFullScreen(); 

    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(Reshape);

    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMove);
    glutPassiveMotionFunc(mousePassiveMove);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp); 
    initLighting();
    init();

    cameraCenterMouse(windowWidth, windowHeight);

    glutMainLoop();
    return 0;
}
 
