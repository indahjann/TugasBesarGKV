#ifndef INPUT_H
#define INPUT_H

#include "object.h"
 
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void processKeys(Player &player);
void mouseButton(int button, int state, int x, int y); 
void restartScene();
  
   
extern bool keys[256]; 
extern float movementDirectionX; 
extern float movementDirectionZ;
extern const float moveSpeed;
extern float speed;
extern float currentMoveSpeed;
extern bool isDragging;
extern bool isShiftPressed;
extern float jumpHeight;
extern float jumpVelocity;
extern bool isMoving;

#endif   
     