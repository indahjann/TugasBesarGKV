/*
 * ==========================================================================
 * MENU SYSTEM IMPLEMENTATION
 * ==========================================================================
 * 
 * File: menu.cpp
 * Author: Project Team
 * Date: May 2, 2025
 * 
 * DESCRIPTION:
 * -----------
 * Implementasi sistem menu untuk game, termasuk:
 * - Menu utama dengan tombol Start
 * - Rendering latar belakang menu dengan pemandangan
 * - Penanganan interaksi dengan tombol menggunakan mouse
 * - Transisi dari menu ke gameplay
 * 
 * VISUAL COMPONENTS: 
 * ----------------
 * - Background       - Latar belakang menu dengan langit, gunung, sawah, dan jalan
 * - Start Button     - Tombol untuk memulai permainan
 * - Game Title       - Judul game yang ditampilkan di tengah layar
 *  
 * GLOBAL VARIABLES:
 * ---------------
 * inMenu        - Status apakah game sedang dalam mode menu
 * windowWidth   - Lebar window aplikasi
 * windowHeight  - Tinggi window aplikasi
 * 
 * FUNCTIONS:
 * ---------
 * drawBackground()   - Menggambar latar belakang menu (langit, gunung, sawah, dan jalan)
 * handleMenuClick()  - Menangani event klik mouse pada menu
 * drawMenu()         - Menggambar seluruh elemen menu termasuk tombol dan teks
 * 
 * DEPENDENCIES:
 * -----------
 * - menu.h      - Header file dengan deklarasi fungsi menu
 * - camera.h    - Header untuk akses ke fungsi cameraCenterMouse
 * - GL/glut.h   - Library OpenGL untuk fungsi rendering dan input
 * - iostream    - Untuk output debugging
 * 
 * HOW TO USE:
 * ----------
 * 1. Atur variabel inMenu ke true di awal aplikasi untuk menampilkan menu
 * 2. Panggil drawMenu(windowWidth, windowHeight) di loop rendering ketika inMenu = true
 * 3. Daftarkan handleMenuClick ke callback mouse untuk menangani klik pada menu:
 *    glutMouseFunc(mouseButton); // mouseButton memanggil handleMenuClick ketika inMenu = true
 * 4. Variabel inMenu akan diubah ke false ketika tombol Start diklik
 * 
 * MENU FLOW:
 * ---------
 * 1. User melihat menu utama dengan tombol Start
 * 2. Ketika tombol Start diklik, inMenu diubah ke false
 * 3. Mouse diposisikan ke tengah screen dan cursor disembunyikan
 * 4. Game berpindah dari menu ke gameplay
 * 
 * ==========================================================================
 */

#include <GL/glut.h>
#include <iostream>
#include <cstdio>
#include "menu.h"
#include "camera.h"
#include "input.h"
#include "gamemanager.h"


bool isPaused = false;

void drawBackground(int width, int height) { 
    // Gambar garis jalan
    glColor3f(1.0f, 1.0f, 1.0f); // Putih garis jalan
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (int i = 0; i < height / 2; i += 40) { // Naikkan posisi garis jalan
        glVertex2i(width / 2, i);
        glVertex2i(width / 2, i + 20);
    } 
    glEnd();   
 
    // Gambar jalan raya (dipersempit dan dinaikkan)
    glColor3f(0.2f, 0.2f, 0.2f); // Abu-abu jalan
    glBegin(GL_QUADS);
    glVertex2i(width / 2 - 50, 0);          // Naikkan posisi jalan
    glVertex2i(width / 2 + 50, 0);          // Naikkan posisi jalan
    glVertex2i(width / 2 + 50, height / 2);          // Naikkan posisi jalan
    glVertex2i(width / 2 - 50, height / 2);          // Naikkan posisi jalan
    glEnd();

    // Gambar sawah
    glColor3f(0.6f, 0.8f, 0.2f); // Hijau sawah
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(width, 0);
    glVertex2i(width, height / 2);
    glVertex2i(0, height / 2);
    glEnd(); 

    // Gambar gunung
    glColor3f(0.4f, 0.76f, 0.38f); // Hijau gunung
    glBegin(GL_TRIANGLES);
    glVertex2i(width / 4 - 200, height / 2);
    glVertex2i(width / 2 - 300, height / 2 + 400);
    glVertex2i(3 * width / 4 - 300, height / 2);
    glEnd();

    // Gambar gunung
    glColor3f(0.4f, 0.81f, 0.41f); // Hijau gunung
    glBegin(GL_TRIANGLES);
    glVertex2i(width / 4 + 300, height / 2);
    glVertex2i(width / 2 + 300, height / 2 + 500);
    glVertex2i(3 * width / 4 + 300, height / 2);
    glEnd();

    // Gambar langit
    glColor3f(0.529f, 0.808f, 0.922f); // Biru langit
    glBegin(GL_QUADS);
    glVertex2i(0, height / 2);
    glVertex2i(width, height / 2);
    glVertex2i(width, height);
    glVertex2i(0, height);
    glEnd();
}

void togglePauseMenu() {
    isPaused = !isPaused;
    
    if (isPaused) {
        // Store mouse position when pausing
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
    } else {
        // Hide cursor and recenter when resuming gameplay
        glutSetCursor(GLUT_CURSOR_NONE);
        extern int windowWidth, windowHeight;
        cameraCenterMouse(windowWidth, windowHeight);
    }
}

void drawPauseMenu(int width, int height) {
    // Semi-transparent overlay
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Draw dark semi-transparent background
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
        glVertex2i(0, 0);
        glVertex2i(width, 0);
        glVertex2i(width, height);
        glVertex2i(0, height);
    glEnd();
    
    glDisable(GL_BLEND);
    
    // Set projection for 2D menu
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Draw menu title
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(width / 2 - 80, height - 100);
    const char *title = "GAME PAUSED";
    for (const char *c = title; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }

    // Define button dimensions
    int buttonWidth = 220;
    int buttonHeight = 60;
    int buttonSpacing = 80;
    int startY = height / 2 + buttonHeight;
    
    // Function to draw a button with text
    auto drawButton = [&](const char* text, int posY, bool highlight) {
        int buttonX = width / 2 - buttonWidth / 2;
        int buttonY = posY - buttonHeight / 2;
        
        // Draw button text
        glColor3f(1.0f, 1.0f, 1.0f);
        int textWidth = 0;
        for (const char* c = text; *c != '\0'; c++) {
            textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
        }
        
        glRasterPos2i(buttonX + (buttonWidth - textWidth) / 2, buttonY + buttonHeight / 2 - 5);
        for (const char* c = text; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
        
        // Draw button background
        if (highlight) {
            glColor3f(0.4f, 0.6f, 1.0f); // Highlighted blue
        } else {
            glColor3f(0.3f, 0.3f, 0.3f); // Dark gray
        }
        
        glBegin(GL_QUADS);
            glVertex2i(buttonX, buttonY);
            glVertex2i(buttonX + buttonWidth, buttonY);
            glVertex2i(buttonX + buttonWidth, buttonY + buttonHeight);
            glVertex2i(buttonX, buttonY + buttonHeight);
        glEnd();
        
        // Draw button border
        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2i(buttonX, buttonY);
            glVertex2i(buttonX + buttonWidth, buttonY);
            glVertex2i(buttonX + buttonWidth, buttonY + buttonHeight);
            glVertex2i(buttonX, buttonY + buttonHeight);
        glEnd();
        
        
    };
    
    // Get mouse position to highlight buttons
    int mouseX, mouseY;
    extern int windowWidth, windowHeight;
    mouseX = glutGet(GLUT_WINDOW_X);
    mouseY = windowHeight - glutGet(GLUT_WINDOW_Y); // Invert Y coordinate
    
    // Draw Resume button
    bool highlightResume = (mouseX >= width/2 - buttonWidth/2 && mouseX <= width/2 + buttonWidth/2 &&
                           mouseY >= startY - buttonHeight/2 && mouseY <= startY + buttonHeight/2);
    drawButton("Resume Game", startY, highlightResume);
    
    // Draw Restart button
    bool highlightRestart = (mouseX >= width/2 - buttonWidth/2 && mouseX <= width/2 + buttonWidth/2 &&
                           mouseY >= startY - buttonSpacing - buttonHeight/2 && 
                           mouseY <= startY - buttonSpacing + buttonHeight/2);
    drawButton("Restart Game", startY - buttonSpacing, highlightRestart);
    
    // Draw Exit button
    bool highlightExit = (mouseX >= width/2 - buttonWidth/2 && mouseX <= width/2 + buttonWidth/2 &&
                         mouseY >= startY - buttonSpacing*2 - buttonHeight/2 && 
                         mouseY <= startY - buttonSpacing*2 + buttonHeight/2);
    drawButton("Exit Game", startY - buttonSpacing*2, highlightExit);
    
    // Restore projection and modelview matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void handlePauseMenuClick(int button, int state, int x, int y, int windowWidth, int windowHeight) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int buttonWidth = 220;
        int buttonHeight = 60;
        int buttonSpacing = 80;
        int startY = windowHeight / 2 + buttonHeight;
        
        // Convert mouse coordinates
        int mouseY = windowHeight - y; // Invert Y coordinate
        
        // Resume button
        if (x >= windowWidth/2 - buttonWidth/2 && x <= windowWidth/2 + buttonWidth/2 &&
            mouseY >= startY - buttonHeight/2 && mouseY <= startY + buttonHeight/2) {
            std::cout << "Resume button clicked" << std::endl;
            togglePauseMenu(); // Resume the game
            return;
        }
        
        // Restart button
        if (x >= windowWidth/2 - buttonWidth/2 && x <= windowWidth/2 + buttonWidth/2 &&
            mouseY >= startY - buttonSpacing - buttonHeight/2 && 
            mouseY <= startY - buttonSpacing + buttonHeight/2) {
            std::cout << "Restart button clicked" << std::endl;
            togglePauseMenu(); // Unpause
            restartScene();    // Restart the game
            return;
        }
        
        // Exit button
        if (x >= windowWidth/2 - buttonWidth/2 && x <= windowWidth/2 + buttonWidth/2 &&
            mouseY >= startY - buttonSpacing*2 - buttonHeight/2 && 
            mouseY <= startY - buttonSpacing*2 + buttonHeight/2) {
            std::cout << "Exit button clicked" << std::endl;
            exit(0);
        }
    }
}

void handleMenuClick(int button, int state, int x, int y, int windowWidth, int windowHeight) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Calculate button position
        int buttonWidth = 200;
        int buttonHeight = 80;
        int buttonX = windowWidth / 2 - buttonWidth / 2;
        int buttonY = windowHeight / 2 - buttonHeight / 2;
        
        // Check if click is on the Start button
        if (x >= buttonX && x <= buttonX + buttonWidth && 
            y >= buttonY && y <= buttonY + buttonHeight) {
            
            // Hide cursor and center it for gameplay
            glutSetCursor(GLUT_CURSOR_NONE);
            cameraCenterMouse(windowWidth, windowHeight);
            
            // Exit menu and start the game
            inMenu = false;
            
            // Set the game state to playing and initialize first wave
            gameState = GAME_PLAYING;
            resetGame(); // Reset and start the first wave
            
            std::cout << "Game started!" << std::endl;
        }
    }
}

void drawMenu(int width, int height) {
    glutSetCursor(GLUT_CURSOR_LEFT_ARROW);

    // Set warna latar belakang menu
    glClearColor(0.529f, 0.808f, 0.922f, 1.0f); // Blue sky background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set proyeksi orthographic
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Function to draw text box with white background and border
    auto drawTextBox = [&](int x, int y, int width, int height) {
        // Draw white background
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
            glVertex2i(x, y);
            glVertex2i(x + width, y);
            glVertex2i(x + width, y + height);
            glVertex2i(x, y + height);
        glEnd();
        
        // Draw black border
        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2i(x, y);
            glVertex2i(x + width, y);
            glVertex2i(x + width, y + height);
            glVertex2i(x, y + height);
        glEnd();
    };

    
    // Title text with black color
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2i(width / 2 - 140, height - 100);
    const char *title = "WAVE SURVIVAL";
    for (const char *c = title; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }

    // Title box - wider for the title
    drawTextBox(width / 2 - 200, height - 130, 400, 60);
    
    
    // Objectives title with black color
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2i(width / 2 - 150, height - 150);
    const char *objectiveTitle = "GAME OBJECTIVES:";
    for (const char *c = objectiveTitle; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    
    // Objective text with black color
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2i(width / 2 - 180, height - 180);
    const char *objective1 = "- Survive and defeat all enemies in each wave";
    for (const char *c = objective1; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }

    
    glRasterPos2i(width / 2 - 180, height - 200);
    const char *objective2 = "- Complete all 3 waves to win the game";
    for (const char *c = objective2; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    glRasterPos2i(width / 2 - 180, height - 220);
    const char *objective3 = "- Earn points by defeating enemies quickly";
    for (const char *c = objective3; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }

    // Objectives section box
    drawTextBox(width / 2 - 220, height - 250, 440, 120);
    
    // Controls section
    int controlBoxWidth = 260;
    int controlBoxHeight = 180;
    int lineHeight = 20;
    
    
    // Left controls title
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2i(width / 4 - 30, height - 300);
    const char *leftControlsTitle = "MOVEMENT CONTROLS:";
    for (const char *c = leftControlsTitle; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    
    
    // Left column of controls with black text
    int leftX = width / 4 - 30;
    int controlY = height - 360;
    
    glRasterPos2i(leftX, controlY);
    const char *control1 = "WASD - Move character";
    for (const char *c = control1; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    glRasterPos2i(leftX, controlY - lineHeight);
    const char *control2 = "SPACE - Jump";
    for (const char *c = control2; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    glRasterPos2i(leftX, controlY - lineHeight*2);
    const char *control3 = "SHIFT - Sprint";
    for (const char *c = control3; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    glRasterPos2i(leftX, controlY - lineHeight*3);
    const char *control4 = "ESC - Pause game";
    for (const char *c = control4; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    // Left controls box
    drawTextBox(width / 4 - 50, height - 450, controlBoxWidth, controlBoxHeight);
    
    // Right controls title
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2i(3 * width / 4 - 150, height - 300);
    const char *rightControlsTitle = "COMBAT CONTROLS:";
    for (const char *c = rightControlsTitle; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    
    
    // Right column of controls with black text
    int rightX = 3 * width / 4 - 150;
    
    glRasterPos2i(rightX, controlY);
    const char *control5 = "1 - Switch to punch mode";
    for (const char *c = control5; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    glRasterPos2i(rightX, controlY - lineHeight);
    const char *control6 = "2 - Switch to gun mode";
    for (const char *c = control6; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    glRasterPos2i(rightX, controlY - lineHeight*2);
    const char *control7 = "Left Click - Attack/Shoot";
    for (const char *c = control7; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    glRasterPos2i(rightX, controlY - lineHeight*3);
    const char *control8 = "Right Click - Aim mode";
    for (const char *c = control8; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }

    // Right controls box
    drawTextBox(3 * width / 4 - 170, height - 450, controlBoxWidth, controlBoxHeight);
    
    
    // Gambar tombol "Start"
    int buttonWidth = 200;
    int buttonHeight = 80;
    int buttonX = width / 2 - buttonWidth / 2;
    int buttonY = height / 2 - buttonHeight / 2;
    
    // Tulis teks "Start" di tombol dengan warna putih (kontras dengan latar belakang biru)
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(buttonX + 75, buttonY + 45);
    const char *buttonText = "Start";
    for (const char *c = buttonText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }

    // Draw button with blue background
    glColor3f(0.3f, 0.5f, 0.8f);
    glBegin(GL_QUADS);
        glVertex2i(buttonX, buttonY);
        glVertex2i(buttonX + buttonWidth, buttonY);
        glVertex2i(buttonX + buttonWidth, buttonY + buttonHeight);
        glVertex2i(buttonX, buttonY + buttonHeight);
    glEnd();
    
    

    // Draw button border
    glLineWidth(2.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2i(buttonX, buttonY);
        glVertex2i(buttonX + buttonWidth, buttonY);
        glVertex2i(buttonX + buttonWidth, buttonY + buttonHeight);
        glVertex2i(buttonX, buttonY + buttonHeight);
    glEnd();

    drawBackground(width, height);
    
    // Kembalikan proyeksi dan modelview ke pengaturan semula
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glutSwapBuffers();
}

void drawWaveTransitionScreen(int width, int height) {
    // Semi-transparent overlay
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Draw dark semi-transparent background
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glBegin(GL_QUADS);
        glVertex2i(0, 0);
        glVertex2i(width, 0);
        glVertex2i(width, height);
        glVertex2i(0, height);
    glEnd();
    
    glDisable(GL_BLEND);
    
    // Set projection for 2D menu
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Draw wave transition message
    extern WaveSystem waveSystem;
    char waveMessage[100];
    sprintf(waveMessage, "WAVE %d COMPLETE", waveSystem.currentWave - 1);
    
    char nextWaveMessage[100];
    sprintf(nextWaveMessage, "PREPARING WAVE %d", waveSystem.currentWave);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Calculate text width for centering
    int textWidth = 0;
    for (const char* c = waveMessage; *c != '\0'; c++) {
        textWidth += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }
    
    glRasterPos2i(width / 2 - textWidth / 2, height / 2 + 20);
    for (const char *c = waveMessage; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }
    
    // Calculate text width for centering
    textWidth = 0;
    for (const char* c = nextWaveMessage; *c != '\0'; c++) {
        textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
    }
    
    glRasterPos2i(width / 2 - textWidth / 2, height / 2 - 20);
    for (const char *c = nextWaveMessage; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    
    // Restore matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void drawWinScreen(int width, int height) {
    // Draw background
    glClearColor(0.0f, 0.2f, 0.4f, 1.0f); // Dark blue background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Set projection for 2D menu
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Draw victory title
    const char* victoryText = "VICTORY!";
    const char* subtitleText = "You've completed all waves!";
    
    // Calculate text width for centering
    int textWidth = 0;
    for (const char* c = victoryText; *c != '\0'; c++) {
        textWidth += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }
    
    // Draw main title
    glColor3f(1.0f, 1.0f, 0.0f); // Gold color
    glRasterPos2i(width / 2 - textWidth / 2, height - 100);
    for (const char *c = victoryText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }
    
    // Calculate subtitle width
    textWidth = 0;
    for (const char* c = subtitleText; *c != '\0'; c++) {
        textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
    }
    
    // Draw subtitle
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(width / 2 - textWidth / 2, height - 140);
    for (const char *c = subtitleText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    
    // Draw buttons
    int buttonWidth = 200;
    int buttonHeight = 60;
    int buttonY = height / 2;
    
    // Get mouse position to highlight buttons
    int mouseX = glutGet(GLUT_WINDOW_X);
    int mouseY = height - glutGet(GLUT_WINDOW_Y); // Invert Y coordinate
    
    // Play Again button
    bool highlightRestart = (mouseX >= width/2 - buttonWidth/2 && mouseX <= width/2 + buttonWidth/2 &&
                           mouseY >= buttonY - buttonHeight/2 && mouseY <= buttonY + buttonHeight/2);
    
    drawButton("Play Again", width/2 - buttonWidth/2, buttonY - buttonHeight/2, 
              buttonWidth, buttonHeight, highlightRestart);
    
    // Exit button
    bool highlightExit = (mouseX >= width/2 - buttonWidth/2 && mouseX <= width/2 + buttonWidth/2 &&
                         mouseY >= buttonY - buttonHeight*2 && mouseY <= buttonY - buttonHeight);
    
    drawButton("Exit Game", width/2 - buttonWidth/2, buttonY - buttonHeight*2, 
              buttonWidth, buttonHeight, highlightExit);
    
    // Restore matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    glutSwapBuffers();
}

void drawButton(const char* text, int x, int y, int width, int height, bool highlight) {
    // Calculate text width for centering
    int textWidth = 0;
    for (const char* c = text; *c != '\0'; c++) {
        textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
    }
    
    // Draw text
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(x + (width - textWidth) / 2, y + height / 2 + 5);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    
    // Draw button background
    if (highlight) {
        glColor3f(0.4f, 0.6f, 1.0f); // Highlighted blue
    } else {
        glColor3f(0.3f, 0.3f, 0.3f); // Dark gray
    }
    
    glBegin(GL_QUADS);
        glVertex2i(x, y);
        glVertex2i(x + width, y);
        glVertex2i(x + width, y + height);
        glVertex2i(x, y + height);
    glEnd();
    
    // Draw button border
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2i(x, y);
        glVertex2i(x + width, y);
        glVertex2i(x + width, y + height);
        glVertex2i(x, y + height);
    glEnd();
    
    
}

void handleWinScreenClick(int button, int state, int x, int y, int windowWidth, int windowHeight) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int buttonWidth = 200;
        int buttonHeight = 60;
        int buttonY = windowHeight / 2;
        
        // Convert mouse coordinates
        int mouseY = windowHeight - y; // Invert Y coordinate
        
        // Play Again button
        if (x >= windowWidth/2 - buttonWidth/2 && x <= windowWidth/2 + buttonWidth/2 &&
            mouseY >= buttonY - buttonHeight/2 && mouseY <= buttonY + buttonHeight/2) {
            std::cout << "Play Again button clicked" << std::endl;
            resetGame();
            gameState = GAME_PLAYING;
            return;
        }
        
        // Exit button
        if (x >= windowWidth/2 - buttonWidth/2 && x <= windowWidth/2 + buttonWidth/2 &&
            mouseY >= buttonY - buttonHeight*2 && mouseY <= buttonY - buttonHeight) {
            std::cout << "Exit button clicked" << std::endl;
            exit(0);
        }
    }
}