#ifndef CAMERA_H
#define CAMERA_H 

#include <GL/glut.h>     
#include <chrono> 
#include "object.h" // Include object.h for Car struct
   
// Camera parameters   
extern float cameraX, cameraY, cameraZ, cameraRatio;
extern float cameraLX, cameraLY, cameraLZ;
extern float cameraSensitivity;
extern int mouseX, mouseY;
extern bool isJumping;
extern std::chrono::time_point<std::chrono::steady_clock> jumpStartTime;
extern float cameraRotationAroundCar;
extern float cameraRotationY;
extern float cameraRotationX ;
extern float movementDirectionX;
extern float movementDirectionZ;
 

extern bool isFirstPersonView; // Track camera perspective
extern bool isThirdPersonView; // Track third-person view
extern bool isScopeMode;      // Track scope/aim mode
extern float defaultFOV;      // Default field of view
extern float scopeFOV;        // Field of view when scoped

// Jumping variables
extern float jumpSpeed;
extern float gravity;
extern float initialJumpVelocity;
extern float currentJumpVelocity;

// Add this function prototype
void updateCameraFirstPerson(const Player& player);

void cameraReshape(int w, int h);
void cameraCenterMouse(int w, int h);
void updateCamera(const Player& player);
void orientMe(float ang);
void mouseMove(int x, int y);
void mousePassiveMove(int x1, int y1);
void updateCameraForPlayer(const Player& player);
void movePlayerBasedOnCamera(Player& player, float forward, float right);
void updateCameraScopeThirdPerson(const Player& player);
bool checkCameraOcclusion(float startX, float startY, float startZ, float& endX, float& endY, float& endZ);


#endif 
 