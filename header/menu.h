#ifndef MENU_H
#define MENU_H

void drawMenu(int width, int height);
void handleMenuClick(int button, int state, int x, int y, int windowWidth, int windowHeight);

// Pause menu functions
void drawPauseMenu(int width, int height);
void handlePauseMenuClick(int button, int state, int x, int y, int windowWidth, int windowHeight);
void togglePauseMenu();

// New wave transition, win/lose screen functions
void drawWaveTransitionScreen(int width, int height);
void drawWinScreen(int width, int height);
void drawLoseScreen(int width, int height);
void handleWinScreenClick(int button, int state, int x, int y, int windowWidth, int windowHeight);
void handleLoseScreenClick(int button, int state, int x, int y, int windowWidth, int windowHeight);

// Helper function to draw buttons
void drawButton(const char* text, int x, int y, int width, int height, bool highlight);

// Game state variables
extern bool inMenu;
extern bool isPaused;

#endif