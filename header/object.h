#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>
#include <GL/glut.h>
 

// Struct untuk mendefinisikan posisi 3D
struct Position {
    float x; 
    float y; 
    float z;
};   
  
// Struct untuk mendefinisikan warna
struct Color {
    float r;
    float g;
    float b;
};
   
// Struct untuk mendefinisikan objek dasar
struct GameObject {
    std::string name;  // Nama objek 
    Position position; // Posisi objek
    Color color;       // Warna objek
    float scale;       // Skala objek
};
 
enum CombatMode {
    PUNCH_MODE,
    SHOOTING_MODE
};

struct Player : public GameObject {
    float speed;       // Kecepatan pemain
    float rotation;    // Rotasi pemain
    Position position;
    float rotationY;

    // Rotasi untuk animasi
    float leftArmRotation;
    float rightArmRotation;
    float leftLegRotation;
    float rightLegRotation;
    float headRotation;

    // Animasi memukul
    bool isPunching = false;       // Apakah sedang memukul
    float punchProgress = 0.0f;    // Progres animasi memukul (0.0 - 1.0)

    void (*draw)(const Player&, bool); // Pointer ke fungsi untuk menggambar pemain

    CombatMode combatMode = PUNCH_MODE;
    bool isShooting = false;
    float shootCooldown = 0.0f;
};


// Struct untuk pohon
struct Tree : public GameObject {
    float height;      // Tinggi pohon
    float trunkRadius; // Radius batang pohon
};

// Struct untuk dinding
struct Wall : public GameObject {
    float width;       // Lebar dinding
    float height;      // Tinggi dinding
    float thickness;   // Ketebalan dinding
};

// Struct untuk rooftop
struct Rooftop {
    float x1, z1;       // Koordinat sudut pertama (kiri bawah)
    float x2, z2;       // Koordinat sudut diagonal (kanan atas)
    float y;            // Tinggi rooftop
    float thickness;    // Ketebalan rooftop
};

// Struct untuk menyimpan semua objek dalam game
struct GameWorld {    // Daftar mobil
    std::vector<Tree> trees;     // Daftar pohon
    std::vector<Wall> walls;     // Daftar dinding
};

struct BoundingBox {
    float minX, minY, minZ; // Koordinat minimum
    float maxX, maxY, maxZ; // Koordinat maksimum
};


extern std::vector<BoundingBox> boundingBoxes; // Daftar bounding box untuk collision detection

// Fungsi
void drawTree(const Tree& tree);
void drawWall(GLfloat x1, GLfloat z1, GLfloat x2, GLfloat z2, GLfloat y, GLfloat height, GLfloat thickness);
void drawRooftop(float x1, float z1, float x2, float z2, float y, float thickness);
void drawFence(float startX, float startZ, float endX, float endZ, float height, float thickness, int numPosts);
void drawSolidCylinder(GLfloat radius, GLfloat height, int slices, int stacks);
Tree createTree(const std::string& name, float x, float y, float z, float scale, float height, float trunkRadius, const Color& color);
Wall createWall(const std::string& name, float x, float y, float z, float scale, float width, float height, float thickness, const Color& color);
Rooftop createRooftop(float x1, float z1, float x2, float z2, float y, float thickness);
Player createPlayer(const std::string& name, float x, float y, float z);
void printGameObjectInfo(const GameObject& obj);
void changePlayerPosition(Player& player, float dx, float dy, float dz);
void drawPlayer(const Player& player, bool isShadow = false); // Function to draw the player
extern Player player; // Declare player globally 

void drawStaircase(float startX, float startZ, float width, float stepDepth, float stepHeight, int numSteps, float baseY);
#endif    
    
  
