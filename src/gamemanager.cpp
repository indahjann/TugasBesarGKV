#include "gamemanager.h"
#include "enemy.h"
#include <iostream>

// Global game state
GameState gameState = GAME_MENU;
WaveSystem waveSystem;

// Initialize wave system
void initGameSystem() {
    waveSystem.currentWave = 1;
    waveSystem.totalWaves = 3;
    waveSystem.enemiesPerWave = 5;
    waveSystem.enemiesKilled = 0;
    waveSystem.waveComplete = false;
    waveSystem.transitionTimer = 0.0f;
    
    // Initialize first wave
    startNewWave();
    
    std::cout << "Game system initialized: " << waveSystem.totalWaves 
              << " waves with " << waveSystem.enemiesPerWave 
              << " enemies per wave." << std::endl;
}

// Update game state
void updateGameState(float deltaTime) {
    // Handle wave transition
    if (gameState == GAME_WAVE_TRANSITION) {
        waveSystem.transitionTimer -= deltaTime;
        
        if (waveSystem.transitionTimer <= 0.0f) {
            if (waveSystem.currentWave > waveSystem.totalWaves) {
                // All waves complete - player wins
                gameState = GAME_WIN;
            } else {
                // Start the next wave
                gameState = GAME_PLAYING;
                startNewWave();
            }
        }
    }
    
    // Check if current wave is complete
    if (gameState == GAME_PLAYING && isWaveComplete()) {
        // Prepare for next wave
        waveSystem.currentWave++;
        waveSystem.enemiesKilled = 0;
        waveSystem.waveComplete = false;
        waveSystem.transitionTimer = 3.0f; // 3 seconds transition time
        
        // Clear any remaining enemies
        clearAllEnemies();
        
        if (waveSystem.currentWave > waveSystem.totalWaves) {
            // All waves complete - player wins
            gameState = GAME_WIN;
        } else {
            // Show transition to next wave
            gameState = GAME_WAVE_TRANSITION;
        }
    }
}

// Start a new wave
void startNewWave() {
    // Reset wave state
    waveSystem.enemiesKilled = 0;
    waveSystem.waveComplete = false;
    
    // Spawn enemies for the new wave
    spawnWaveEnemies(waveSystem.enemiesPerWave);
    
    std::cout << "Starting Wave " << waveSystem.currentWave 
              << " of " << waveSystem.totalWaves << std::endl;
}

// Check if current wave is complete
bool isWaveComplete() {
    return waveSystem.enemiesKilled >= waveSystem.enemiesPerWave;
}

// Check if game is won (all waves completed)
bool isGameWon() {
    return waveSystem.currentWave > waveSystem.totalWaves;
}

// Reset game to initial state
void resetGame() {
    // Clear enemies
    clearAllEnemies();
    
    // Reset wave system
    waveSystem.currentWave = 1;
    waveSystem.enemiesKilled = 0;
    waveSystem.waveComplete = false;
    waveSystem.transitionTimer = 0.0f;
    
    // Start first wave
    startNewWave();
    
    // Reset game state
    gameState = GAME_PLAYING;
}

// Register enemy kill
void registerEnemyKill() {
    waveSystem.enemiesKilled++;
    
    std::cout << "Enemy killed: " << waveSystem.enemiesKilled 
              << " / " << waveSystem.enemiesPerWave << std::endl;
    
    // Check if wave is complete
    if (waveSystem.enemiesKilled >= waveSystem.enemiesPerWave) {
        waveSystem.waveComplete = true;
        std::cout << "Wave " << waveSystem.currentWave << " complete!" << std::endl;
    }
}