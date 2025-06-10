#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

// Game states
enum GameState {
    GAME_MENU,
    GAME_PLAYING,
    GAME_PAUSED,
    GAME_WAVE_TRANSITION,
    GAME_WIN,
    GAME_LOSE
};

// Wave-based game system
struct WaveSystem {
    int currentWave;       // Current wave number (1-based)
    int totalWaves;        // Total number of waves
    int enemiesPerWave;    // Number of enemies to kill per wave
    int enemiesKilled;     // Number of enemies killed in current wave
    bool waveComplete;     // Flag to indicate if current wave is complete
    float transitionTimer; // Timer for wave transition screen
};

// Initialize wave system
void initGameSystem();

// Update game state
void updateGameState(float deltaTime);

// Start a new wave
void startNewWave();

// Check if current wave is complete
bool isWaveComplete();

// Check if game is won (all waves completed)
bool isGameWon();

// Reset game to initial state
void resetGame();

// Register enemy kill
void registerEnemyKill();

// External variables
extern GameState gameState;
extern WaveSystem waveSystem;

#endif // GAMEMANAGER_H