/*
 * ==========================================================================
 * OBJECT SYSTEM IMPLEMENTATION
 * ==========================================================================
 * 
 * File: object.cpp
 * Author: Project Team
 * Date: May 2, 2025
 *  
 * DESCRIPTION:
 * -----------
 * Implementasi sistem objek untuk game, termasuk:
 * - Player character dengan animasi dan gerakan
 * - Kendaraan (car) dengan model 3D dan mekanika kendali
 * - Objek lingkungan seperti pohon dan dinding
 * - Sistem collision dengan hitbox
 * - Rendering objek dan tekstur
 * 
 * STRUCTURES: 
 * ----------
 * Player - Didefinisikan di object.h
 *   - Karakter pemain dengan properti posisi, rotasi, animasi, dan mode combat
 * 
 * Car - Didefinisikan di object.h
 *   - Kendaraan dengan properti posisi, rotasi, dan model 3D
 * 
 * Tree - Didefinisikan di object.h
 *   - Pohon dengan properti posisi dan ukuran
 * 
 * Wall - Didefinisikan di object.h
 *   - Dinding dengan properti posisi, ukuran, dan tekstur
 * 
 * BoundingBox - Didefinisikan di object.h
 *   - Kotak pembatas untuk deteksi tabrakan
 * 
 * GLOBAL VARIABLES:
 * ---------------
 * player              - Instance dari Player yang mengontrol karakter utama
 * boundingBoxes       - Vector yang menyimpan semua bounding box untuk collision detection
 * angle               - Sudut rotasi global untuk animasi
 * 
 * FUNCTIONS:
 * ---------
 * createHitbox()              - Membuat hitbox untuk collision detection
 * drawCar()                   - Menggambar model 3D kendaraan
 * drawPlayer()                - Menggambar model 3D player dengan animasi
 * drawWall()                  - Menggambar dinding bertekstur
 * drawSolidCylinder()         - Utilitas untuk menggambar silinder
 * drawTree()                  - Menggambar pohon 3D
 * drawFence()                 - Menggambar pagar pembatas
 * createPlayer()              - Membuat instance karakter player
 * changePlayerPosition()      - Memperbarui posisi player
 * createCar()                 - Membuat instance kendaraan
 * createTree()                - Membuat instance pohon
 * createWall()                - Membuat instance dinding
 * printGameObjectInfo()       - Mencetak informasi debug tentang objek game
 * 
 * DEPENDENCIES:
 * -----------
 * - object.h    - Header file dengan deklarasi struct dan fungsi objek
 * - render.h    - Header untuk fungsi rendering dan tekstur
 * - collision.h - Header untuk sistem collision
 * - GL/glut.h   - Library OpenGL untuk rendering 3D
 * - cmath       - Library math untuk operasi matematika
 * 
 * HOW TO USE:
 * ----------
 * 1. Gunakan createPlayer(), createCar(), createTree(), atau createWall() untuk membuat objek
 * 2. Panggil drawPlayer(), drawCar(), drawTree(), atau drawWall() untuk menampilkan objek
 * 3. Gunakan createHitbox() untuk menambahkan collision detection ke objek
 * 4. Gunakan changePlayerPosition() untuk memperbarui posisi player
 * 
 * RENDERING FEATURES:
 * -----------------
 * - Rendering model 3D untuk player, kendaraan, dan objek lingkungan
 * - Animasi lengan dan kaki untuk player
 * - Tekstur untuk dinding, pagar, dan objek lainnya
 * - Support untuk mode first-person dan third-person
 * - Rendering bayangan dengan siluet
 * 
 * ==========================================================================
 */

#include "object.h"
#include "render.h"
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <iostream>
#include "enemy.h"
#include "combat.h"

 
float angle = 0.0;  
Player player;  

std::vector<BoundingBox> boundingBoxes; // Vector to store bounding boxes
GLuint quartzTexture = 0; // Global variable for quartz texture

void createHitbox(float x, float y, float z, float width, 
float height, float depth) {
    // Create a hitbox using the given parameters
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(width, height, depth);
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for the hitbox
    glutWireCube(1.0f); // Draw a wireframe cube as the hitbox
    glPopMatrix();
    boundingBoxes.push_back({x, y, z, width, height, depth}); // Store the bounding box
}

void drawPlayer(const Player& player, bool isShadow) {
    glPushMatrix();

    // Add these lines to position the player correctly
    glTranslatef(player.position.x, player.position.y, player.position.z);
    glRotatef(player.rotationY, 0.0f, 1.0f, 0.0f);
    
    glScalef(0.8f, 0.8f, 0.8f); // Skala untuk memperkecil ukuran player
    
    // Badan
    glPushMatrix();
        if (!isShadow) glColor3f(0.2, 0.6, 0.8); // Blue (only if not shadow)
        glTranslatef(0.0f, 1.4f, 0.0f);
        glScalef(0.85f, 1.1f, 0.5f);
        glutSolidCube(1.0f);
    glPopMatrix();

    // Kepala
    glPushMatrix();
        if (!isShadow) glColor3f(1.0, 0.8, 0.6); // Warna kulit (hanya jika bukan bayangan)
        glTranslatef(0.0f, 2.4f, 0.0f);
        glRotatef(player.headRotation, 0.0f, 1.0f, 0.0f);
        glutSolidSphere(0.5f, 20, 20);
        
        // Mata kiri
        glPushMatrix();
            glTranslatef(-0.15f, 0.1f, 0.4f);
            
            if (!isShadow) {
                // Bagian putih mata
                glColor3f(1.0f, 1.0f, 1.0f); // Putih
                glutSolidSphere(0.1f, 10, 10);
                
                // Pupil
                glTranslatef(0.0f, 0.0f, 0.08f);
                glColor3f(0.0f, 0.0f, 0.0f); // Hitam
                glutSolidSphere(0.05f, 8, 8);
            } else {
                glutSolidSphere(0.1f, 10, 10); // Bayangan mata
            }
        glPopMatrix();
        
        // Mata kanan
        glPushMatrix();
            glTranslatef(0.15f, 0.1f, 0.4f); // Posisi mata kanan
            
            // Bagian putih mata
            if (!isShadow) {
                glColor3f(1.0f, 1.0f, 1.0f); // Putih
                glutSolidSphere(0.1f, 10, 10);
                
                // Pupil
                glTranslatef(0.0f, 0.0f, 0.08f);
                glColor3f(0.0f, 0.0f, 0.0f); // Hitam
                glutSolidSphere(0.05f, 8, 8);
            } else {
                glutSolidSphere(0.1f, 10, 10); // Bayangan mata
            }
        glPopMatrix();
        
        // Mulut

        glPushMatrix();
            if (!isShadow) {
                glTranslatef(0.0f, -0.23f, 0.45f); // Posisi mulut
                glColor3f(0.8f, 0.2f, 0.2f); // Merah untuk mulut
                
                // Membuat mulut sebagai elips tipis
                glScalef(0.2f, 0.05f, 0.05f);
                glutSolidSphere(0.5f, 10, 10);
            }
            else {
                glTranslatef(0.0f, -0.1f, 0.4f); // Posisi mulut
                glColor3f(0.0f, 0.0f, 0.0f); // Hitam untuk bayangan mulut
                glScalef(0.2f, 0.05f, 0.05f);
                glutSolidSphere(0.5f, 10, 10);
            }
        glPopMatrix();
        
    glPopMatrix();
    
    // Rambut
    glPushMatrix();
        if (!isShadow) glColor3f(0.0, 0.0, 0.0); // Warna hitam
        glTranslatef(0.0f, 2.6f, 0.0f);
        glRotatef(player.headRotation, 0.0f, 1.0f, 0.0f); // Rotasi kepala
        
        // Definisikan bidang potong (clipping plane) untuk memotong bagian bawah bola
        double clipPlane[] = {0.0, 1.0, 0.0, 0.0}; // Bidang XZ (y = 0)
        
        // Aktifkan clipping plane
        glEnable(GL_CLIP_PLANE0);
        glClipPlane(GL_CLIP_PLANE0, clipPlane);
        
        // Gambar bola
        glutSolidSphere(0.50f, 20, 20);
            
        // Nonaktifkan clipping plane
        glDisable(GL_CLIP_PLANE0);
    glPopMatrix();  
    
    // Left arm - Simpler design
    glPushMatrix();
        if (!isShadow) glColor3f(0.2, 0.6, 0.8); // Blue (only if not shadow)
        
        // Position at body side but not too far to the edge
        glTranslatef(0.5f, 1.9f, 0.0f);
        
        // Apply arm rotation at shoulder joint
        glRotatef(player.leftArmRotation, 1.0f, 0.0f, 0.0f);
        
        // Single segment for arm (simpler)
        glPushMatrix();
            glTranslatef(0.0f, -0.4f, 0.0f);
            glScalef(0.2f, 0.8f, 0.2f);
            glutSolidCube(1.0f);
        glPopMatrix();
        
        // Draw hand at end of arm
        if (!isShadow) {
            glColor3f(1.0f, 0.8f, 0.6f); // Skin color
            glTranslatef(0.0f, -0.8f, 0.0f);
            glutSolidSphere(0.1f, 8, 8);
        }
    glPopMatrix();

    // Right arm - Simpler design
    glPushMatrix();
        if (!isShadow) glColor3f(0.2, 0.6, 0.8); // Blue (only if not shadow)
        
        // Position at body side but not too far to the edge
        glTranslatef(-0.5f, 1.9f, 0.0f);
        
        // Apply appropriate arm rotation based on combat mode
        if (player.combatMode == SHOOTING_MODE) {
            // Aiming position for pistol (arm pointing forward)
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);  // Rotate arm to side position
        } else if (player.isPunching) {
            // Punching animation - arm extends forward based on progress
            float punchAngle = -90.0f * player.punchProgress; // Arm goes from 0 to -90 degrees
            glRotatef(punchAngle, 1.0f, 0.0f, 0.0f); // Forward punch motion
        } else {
            // Normal walking animation
            glRotatef(player.rightArmRotation, 1.0f, 0.0f, 0.0f);
        }
        
        // Single segment for arm (simpler)
        glPushMatrix();
            glTranslatef(0.0f, -0.4f, 0.0f);
            glScalef(0.2f, 0.8f, 0.2f);
            glutSolidCube(1.0f);
        glPopMatrix();
        
        // Draw hand or weapon depending on combat mode
        if (!isShadow) {
            glTranslatef(0.0f, -0.8f, 0.0f);
            
            if (player.combatMode == SHOOTING_MODE) {
                // For pistol aiming, we position the pistol forward along arm axis
                drawPistol(true);
            } else {
                // Draw hand or fist depending on punch state
                glColor3f(1.0f, 0.8f, 0.6f); // Skin color
                if (player.isPunching) {
                    // Fist (slightly larger and rectangular for punching)
                    glPushMatrix();
                    glScalef(0.12f, 0.12f, 0.15f);
                    glutSolidCube(1.0f);
                    glPopMatrix();
                } else {
                    // Normal hand (sphere)
                    glutSolidSphere(0.1f, 8, 8);
                }
            }
        }
    glPopMatrix();

    // Kaki kiri
    glPushMatrix();
        glColor3f(0.0, 0.0, 0.0); // Hitam
        glTranslatef(-0.2f, 0.8f, 0.0f); // Pindahkan ke pangkal kaki
        glRotatef(player.leftLegRotation, 1.0f, 0.0f, 0.0f); // Rotasi kaki kiri
        glTranslatef(0.0f, -0.4f, 0.0f); // Geser ke bawah untuk menggambar kaki
        glScalef(0.35f, 1.0f, 0.3f);
        glutSolidCube(1.0f);
    glPopMatrix();

    // Kaki kanan
    glPushMatrix();
        glColor3f(0.0, 0.0, 0.0);
        glTranslatef(0.2f, 0.8f, 0.0f); // Pindahkan ke pangkal kaki
        glRotatef(player.rightLegRotation, 1.0f, 0.0f, 0.0f); // Rotasi kaki kanan
        glTranslatef(0.0f, -0.4f, 0.0f); // Geser ke bawah untuk menggambar kaki
        glScalef(0.35f, 1.0f, 0.3f);
        glutSolidCube(1.0f);
    glPopMatrix();

    glPopMatrix();
}

void drawWall( GLfloat x1, GLfloat z1,  GLfloat x2, GLfloat z2, GLfloat y, GLfloat height, GLfloat thickness  ) {
    // Ensure texture is loaded
    if (woodTexture == 0) return;
    
    // Calculate length and angle
    float dx = x2 - x1;
    float dz = z2 - z1;   
    float length = sqrt(dx*dx + dz*dz);
    float angle = -(atan2(dz, dx) * 180.0f / M_PI);
    
    // Debug info
    std::cout << "Drawing wall from (" << x1 << "," << z1 << ") to (" 
              << x2 << "," << z2 << "), dx=" << dx << ", dz=" << dz
              << ", length=" << length << ", angle=" << angle << std::endl;
    
    // Create bounding box
    BoundingBox box = {
        std::min(x1, x2) - thickness/2, y, std::min(z1, z2) - thickness/2,
        std::max(x1, x2) + thickness/2, y + height, std::max(z1, z2) + thickness/2
    };
    boundingBoxes.push_back(box);
    
    // Enable texturing
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    
    // Define texture scale
    float textureScale = 2.0f;
    float repeatLength = length * textureScale;
    float repeatHeight = height * textureScale;
    float repeatThickness = thickness * textureScale;
    
    glPushMatrix();
    
    // Move to start position and rotate to align with the wall direction
    glTranslatef(x1, y, z1);
    glRotatef(angle, 0.0f, 1.0f, 0.0f);
    
    // Draw the wall as a box
    glColor3f(1.0f, 1.0f, 1.0f); // White for proper texture display
    
    // Front face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, thickness);
    glTexCoord2f(repeatLength, 0.0f); glVertex3f(length, 0.0f, thickness);
    glTexCoord2f(repeatLength, repeatHeight); glVertex3f(length, height, thickness);
    glTexCoord2f(0.0f, repeatHeight); glVertex3f(0.0f, height, thickness);
    glEnd();
    
    // Back face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(repeatLength, 0.0f); glVertex3f(length, 0.0f, 0.0f);
    glTexCoord2f(repeatLength, repeatHeight); glVertex3f(length, height, 0.0f);
    glTexCoord2f(0.0f, repeatHeight); glVertex3f(0.0f, height, 0.0f);
    glEnd();
    
    // Top face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, height, 0.0f);
    glTexCoord2f(repeatLength, 0.0f); glVertex3f(length, height, 0.0f);
    glTexCoord2f(repeatLength, repeatThickness); glVertex3f(length, height, thickness);
    glTexCoord2f(0.0f, repeatThickness); glVertex3f(0.0f, height, thickness);
    glEnd();
    
    // Bottom face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(repeatLength, 0.0f); glVertex3f(length, 0.0f, 0.0f);
    glTexCoord2f(repeatLength, repeatThickness); glVertex3f(length, 0.0f, thickness);
    glTexCoord2f(0.0f, repeatThickness); glVertex3f(0.0f, 0.0f, thickness);
    glEnd();
    
    // Left face
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(repeatThickness, 0.0f); glVertex3f(0.0f, 0.0f, thickness);
    glTexCoord2f(repeatThickness, repeatHeight); glVertex3f(0.0f, height, thickness);
    glTexCoord2f(0.0f, repeatHeight); glVertex3f(0.0f, height, 0.0f);
    glEnd();
    
    // Right face
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(length, 0.0f, 0.0f);
    glTexCoord2f(repeatThickness, 0.0f); glVertex3f(length, 0.0f, thickness);
    glTexCoord2f(repeatThickness, repeatHeight); glVertex3f(length, height, thickness);
    glTexCoord2f(0.0f, repeatHeight); glVertex3f(length, height, 0.0f);
    glEnd();
    
    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);
}

void drawRooftop(float x1, float z1, float x2, float z2, float y, float thickness) {
    // Ensure texture is loaded
    if (quartzTexture == 0) {
        quartzTexture = loadBMP("texture/quartz-block.bmp");
    }
    
    // Calculate dimensions
    float width = fabs(x2 - x1);
    float length = fabs(z2 - z1);
    
    // Define texture scale - same as in drawWall
    float textureScale = 4.0f;
    float repeatWidth = width * textureScale;
    float repeatLength = length * textureScale;
    float repeatThickness = thickness * textureScale;
    
    // Aktifkan tekstur
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, quartzTexture);
    
    // Gambar permukaan atas rooftop dengan tekstur
    glColor3f(1.0f, 1.0f, 1.0f); // White for proper texture display
    
    // Top face (atas)
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, y, z1);
    glTexCoord2f(repeatWidth, 0.0f); glVertex3f(x2, y, z1);
    glTexCoord2f(repeatWidth, repeatLength); glVertex3f(x2, y, z2);
    glTexCoord2f(0.0f, repeatLength); glVertex3f(x1, y, z2);
    glEnd();
    
    // Bottom face (bawah)
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, y-thickness, z1);
    glTexCoord2f(repeatWidth, 0.0f); glVertex3f(x2, y-thickness, z1);
    glTexCoord2f(repeatWidth, repeatLength); glVertex3f(x2, y-thickness, z2);
    glTexCoord2f(0.0f, repeatLength); glVertex3f(x1, y-thickness, z2);
    glEnd();
    
    // Front face (depan - z1)
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, y-thickness, z1);
    glTexCoord2f(repeatWidth, 0.0f); glVertex3f(x2, y-thickness, z1);
    glTexCoord2f(repeatWidth, repeatThickness); glVertex3f(x2, y, z1);
    glTexCoord2f(0.0f, repeatThickness); glVertex3f(x1, y, z1);
    glEnd();
    
    // Back face (belakang - z2)
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, y-thickness, z2);
    glTexCoord2f(repeatWidth, 0.0f); glVertex3f(x2, y-thickness, z2);
    glTexCoord2f(repeatWidth, repeatThickness); glVertex3f(x2, y, z2);
    glTexCoord2f(0.0f, repeatThickness); glVertex3f(x1, y, z2);
    glEnd();
    
    // Left face (kiri - x1)
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, y-thickness, z1);
    glTexCoord2f(repeatLength, 0.0f); glVertex3f(x1, y-thickness, z2);
    glTexCoord2f(repeatLength, repeatThickness); glVertex3f(x1, y, z2);
    glTexCoord2f(0.0f, repeatThickness); glVertex3f(x1, y, z1);
    glEnd();
    
    // Right face (kanan - x2)
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x2, y-thickness, z1);
    glTexCoord2f(repeatLength, 0.0f); glVertex3f(x2, y-thickness, z2);
    glTexCoord2f(repeatLength, repeatThickness); glVertex3f(x2, y, z2);
    glTexCoord2f(0.0f, repeatThickness); glVertex3f(x2, y, z1);
    glEnd();

    // Nonaktifkan tekstur
    glDisable(GL_TEXTURE_2D);
}

void drawSolidCylinder(GLfloat radius, GLfloat height, int slices, int stacks)
{
    GLUquadricObj *quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluCylinder(quadric, radius, radius, height, slices, stacks);
    gluDeleteQuadric(quadric);
}

void drawTree(float x, float z) {
    // Draw tree shadow first
    drawTreeShadow(x, z, 1.0f);
    
    // Pohon
    glPushMatrix();
    glTranslatef(x, 0, z);

    // Batang pohon
    glPushMatrix();
    glColor3f(0.5f, 0.35f, 0.05f); // Warna coklat
    glRotatef(-90, 1, 0, 0);
    drawSolidCylinder(0.2, 2, 20, 20); // radius, height, slices, stacks
    glPopMatrix();

    // Daun pohon
    glPushMatrix();
    glTranslatef(0, 2, 0);
    glColor3f(0.0f, 0.5f, 0.0f);
    glRotatef(-90, 1, 0, 0);     // Warna hijau
    glutSolidCone(1, 2, 20, 20); // base, height, slices, stacks
    glPopMatrix();

    // Daun pohon
    glPushMatrix();
    glTranslatef(0, 1.5, 0);
    glColor3f(0.0f, 0.5f, 0.0f);
    glRotatef(-90, 1, 0, 0);     // Warna hijau
    glutSolidCone(1, 2, 20, 20); // base, height, slices, stacks
    glPopMatrix();

    // Daun pohon
    glPushMatrix();
    glTranslatef(0, 1, 0);
    glColor3f(0.0f, 0.5f, 0.0f);
    glRotatef(-90, 1, 0, 0);     // Warna hijau
    glutSolidCone(1, 2, 20, 20); // base, height, slices, stacks
    glPopMatrix();

    glPopMatrix();
}

void drawFence(float startX, float startZ, float endX, float endZ, float height, float thickness, int numPosts) {
    // Hitung arah dan panjang pagar
    float dx = endX - startX;
    float dz = endZ - startZ;
    float length = sqrt(dx*dx + dz*dz);
    float angle = -(atan2(dz, dx) * 180.0f / M_PI);
    
    // Buat bounding box untuk pagar ini
    BoundingBox fenceBox = {
        std::min(startX, endX) - thickness/2, 0.0f, std::min(startZ, endZ) - thickness/2,
        std::max(startX, endX) + thickness/2, height, std::max(startZ, endZ) + thickness/2
    };
    boundingBoxes.push_back(fenceBox);
    
    glPushMatrix();
    
    // Posisikan di titik awal
    glTranslatef(startX, 0, startZ);
    
    // Rotasi untuk mengarah ke titik akhir
    glRotatef(angle, 0.0f, 1.0f, 0.0f);
    
    glColor3f(0.5f, 0.35f, 0.05f); // Warna coklat untuk pagar kayu
    
    // Hitung jarak antar tiang
    float postSpacing = length / (float)(numPosts - 1);
    
    // Gambar tiang-tiang vertikal
    for (int i = 0; i < numPosts; i++) {
        float posX = i * postSpacing;
        
        glPushMatrix();
        glTranslatef(posX, 0, 0);
        
        // Gambar tiang dengan silinder vertikal
        glPushMatrix();
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Putar untuk berdiri vertikal
        drawSolidCylinder(thickness/2.0f, height, 8, 4);
        glPopMatrix();
        
        glPopMatrix();
    }
    
    // Gambar palang horizontal (2 palang)
    float railHeights[2] = {height * 0.3f, height * 0.7f}; // Posisi palang pada 30% dan 70% tinggi
    
    for (int rail = 0; rail < 2; rail++) {
        glPushMatrix();
        // Posisikan di ketinggian yang tepat
        glTranslatef(length/2.0f, railHeights[rail], 0);
        
        // Rotasi palang untuk sejajar dengan pagar
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        
        // Geser ke posisi yang tepat
        glTranslatef(0.0f, 0.0f, -length/2.0f);
        
        // Gambar palang horizontal
        drawSolidCylinder(thickness/3.0f, length, 8, 4);
        
        glPopMatrix();
    }
    
    glPopMatrix();
}

void drawStaircase(float startX, float startZ, float width, float stepDepth, float stepHeight, int numSteps, float baseY) {
    // Ensure texture is loaded
    if (woodTexture == 0) {
        woodTexture = loadBMP("texture/quartz-block.bmp");
    }
    
    // Enable texturing
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    
    // Draw each step
    glColor3f(1.0f, 1.0f, 1.0f);  // White for proper texture display
    // Define texture coordinates
    float textureScale = 1.0f;
    float repeatWidth = width * textureScale;
    float repeatDepth = stepDepth * textureScale;
    float repeatHeight = stepHeight * textureScale;

    for (int i = 0; i < numSteps; i++) {
        float currentHeight = baseY + (i * stepHeight);
        float stepZ = startZ + (i * stepDepth);
        
        // Draw horizontal part (tread)
        glPushMatrix();
        // Top face of step
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(startX, currentHeight + stepHeight, stepZ);
        glTexCoord2f(repeatWidth, 0.0f); glVertex3f(startX + width, currentHeight + stepHeight, stepZ);
        glTexCoord2f(repeatWidth, repeatDepth); glVertex3f(startX + width, currentHeight + stepHeight, stepZ + stepDepth);
        glTexCoord2f(0.0f, repeatDepth); glVertex3f(startX, currentHeight + stepHeight, stepZ + stepDepth);
        glEnd();
        
        // Front face of step (riser)
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(startX, currentHeight, stepZ);
        glTexCoord2f(repeatWidth, 0.0f); glVertex3f(startX + width, currentHeight, stepZ);
        glTexCoord2f(repeatWidth, repeatHeight); glVertex3f(startX + width, currentHeight + stepHeight, stepZ);
        glTexCoord2f(0.0f, repeatHeight); glVertex3f(startX, currentHeight + stepHeight, stepZ);
        glEnd();
        
        // Side faces (left and right)
        // Left side
        glBegin(GL_QUADS);
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(startX, currentHeight, stepZ);
        glTexCoord2f(repeatDepth, 0.0f); glVertex3f(startX, currentHeight, stepZ + stepDepth);
        glTexCoord2f(repeatDepth, repeatHeight); glVertex3f(startX, currentHeight + stepHeight, stepZ + stepDepth);
        glTexCoord2f(0.0f, repeatHeight); glVertex3f(startX, currentHeight + stepHeight, stepZ);
        glEnd();
        
        // Right side
        glBegin(GL_QUADS);
        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(startX + width, currentHeight, stepZ);
        glTexCoord2f(repeatDepth, 0.0f); glVertex3f(startX + width, currentHeight, stepZ + stepDepth);
        glTexCoord2f(repeatDepth, repeatHeight); glVertex3f(startX + width, currentHeight + stepHeight, stepZ + stepDepth);
        glTexCoord2f(0.0f, repeatHeight); glVertex3f(startX + width, currentHeight + stepHeight, stepZ);
        glEnd();
        glPopMatrix();
    }
    
    // // Draw supports for the staircase (side walls)
    // float totalHeight = numSteps * stepHeight;
    // float totalLength = numSteps * stepDepth;
    
    // // Left support wall
    // glBegin(GL_QUADS);
    // glNormal3f(-1.0f, 0.0f, 0.0f);
    // glTexCoord2f(0.0f, 0.0f); glVertex3f(startX - 0.2f, baseY, startZ);
    // glTexCoord2f(totalLength * textureScale, 0.0f); glVertex3f(startX - 0.2f, baseY, startZ + totalLength);
    // glTexCoord2f(totalLength * textureScale, totalHeight * textureScale); glVertex3f(startX - 0.2f, baseY + totalHeight, startZ + totalLength);
    // glTexCoord2f(0.0f, totalHeight * textureScale); glVertex3f(startX - 0.2f, baseY + totalHeight, startZ);
    // glEnd();
    
    // // Right support wall
    // glBegin(GL_QUADS);
    // glNormal3f(1.0f, 0.0f, 0.0f);
    // glTexCoord2f(0.0f, 0.0f); glVertex3f(startX + width + 0.2f, baseY, startZ);
    // glTexCoord2f(totalLength * textureScale, 0.0f); glVertex3f(startX + width + 0.2f, baseY, startZ + totalLength);
    // glTexCoord2f(totalLength * textureScale, totalHeight * textureScale); glVertex3f(startX + width + 0.2f, baseY + totalHeight, startZ + totalLength);
    // glTexCoord2f(0.0f, totalHeight * textureScale); glVertex3f(startX + width + 0.2f, baseY + totalHeight, startZ);
    // glEnd();
    
    // Disable texturing
    glDisable(GL_TEXTURE_2D);
}

Player createPlayer(const std::string& name, float x, float y, float z) {
    Player player;
    player.name = name;
    player.position = {x, y, z};
    player.speed = 0.0f; // Initial speed
    player.rotation = 0.0f; // Initial rotation
    player.draw = drawPlayer; // Assign the draw function
    return player;
}

void changePlayerPosition(Player& player, float dx, float dy, float dz) {
    player.position.x += dx;
    player.position.y += dy;
    player.position.z += dz;
}

Tree createTree(const std::string& name, float x, float y, float z, float scale, float height, float trunkRadius, const Color& color) {
    Tree tree;
    tree.name = name;
    tree.position = {x, y, z};
    tree.scale = scale;
    tree.height = height;
    tree.trunkRadius = trunkRadius;
    tree.color = color;
    return tree;
}

Wall createWall(const std::string& name, float x, float y, float z, float scale, float width, float height, float thickness, const Color& color) {
    Wall wall;
    wall.name = name; 
    wall.position = {x, y, z};
    wall.scale = scale;
    wall.width = width;
    wall.height = height;
    wall.thickness = thickness;
    wall.color = color;
    return wall;
}

Rooftop createRooftop(GLfloat x1, GLfloat z1, GLfloat x2, GLfloat z2, GLfloat y, GLfloat thickness, const Color& color) {
    Rooftop rooftop;
    rooftop.x1 = x1;
    rooftop.z1 = z1;
    rooftop.x2 = x2;
    rooftop.z2 = z2;
    rooftop.y = y;
    rooftop.thickness = thickness;
    //rooftop.color = color;
    return rooftop;
}
