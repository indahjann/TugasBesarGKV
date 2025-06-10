/*
 * ==========================================================================
 * RENDERING SYSTEM IMPLEMENTATION
 * ==========================================================================
 * 
 * File: render.cpp
 * Author: Project Team
 * Date: May 2, 2025
 * 
 * DESCRIPTION: 
 * -----------
 * Implementasi sistem rendering untuk game, termasuk:
 * - Manajemen tekstur untuk objek dan permukaan
 * - Rendering scene dengan objek 3D dan efek grafis
 * - Animasi karakter player dan musuh
 * - Sistem bayangan untuk objek dalam scene
 * - Rendering elemen UI seperti teks dan informasi
 * - Grid untuk terrain dasar dengan tekstur
 * 
 * GLOBAL VARIABLES:
 * --------------- 
 * checkImage[][]      - Data piksel untuk tekstur dasar
 * wallImage[][]       - Data piksel untuk tekstur dinding
 * roofImage[][]       - Data piksel untuk tekstur atap
 * texture[]           - Array ID tekstur OpenGL
 * woodTexture         - ID tekstur untuk material kayu
 * grassTexture        - ID tekstur untuk material rumput
 * shadowMat[]         - Matriks transformasi untuk bayangan
 * cars[]              - Daftar kendaraan pada scene
 * 
 * FUNCTIONS:
 * ---------
 * loadBMP()                 - Memuat tekstur dari file BMP
 * calculateShadowMatrix()   - Menghitung matriks proyeksi bayangan
 * drawPlayerShadowSilhouette() - Menggambar siluet bayangan player
 * updatePlayerAnimation()   - Memperbarui animasi player berdasarkan gerakan
 * updatePunchAnimation()    - Memperbarui animasi pukulan dan deteksi hit
 * drawText()                - Menggambar teks pada layar
 * calculateFPS()            - Menghitung dan mengembalikan frame rate
 * makeCheckImage()          - Membuat tekstur dasar checkerboard
 * makeWallImage()           - Membuat tekstur untuk dinding
 * makeRoofImage()           - Membuat tekstur untuk atap
 * loadTexture()             - Memuat semua tekstur ke memori GPU
 * Grid()                    - Menggambar grid terrain dasar dengan tekstur rumput
 * displayBoundingBoxesOnScreen() - Menampilkan informasi collision boxes
 * drawPlayerShadow()        - Menggambar bayangan player pada permukaan
 * renderScene()             - Fungsi utama untuk rendering seluruh scene
 * 
 * DEPENDENCIES:
 * -----------
 * - GL/glut.h     - Library OpenGL untuk rendering
 * - collision.h   - Header untuk akses ke fungsi collision detection
 * - object.h      - Header untuk akses ke objek game
 * - render.h      - Header untuk deklarasi fungsi rendering
 * - map.h         - Header untuk akses ke elemen peta
 * - enemy.h       - Header untuk akses ke musuh
 * - combat.h      - Header untuk akses ke sistem pertarungan
 * 
 * TEXTURE MANAGEMENT:
 * -----------------
 * - Tekstur prosedural untuk checkerboard, dinding, dan atap
 * - Tekstur dari file BMP untuk material kayu dan rumput
 * - Support untuk transformasi tekstur dan pengulangan (repeating)
 * 
 * SHADOW SYSTEM:
 * ------------
 * - Implementasi bayangan menggunakan stencil buffer
 * - Proyeksi bayangan pada permukaan datar
 * - Siluet bayangan untuk performa yang lebih baik
 * 
 * HOW TO USE:
 * ----------
 * 1. Panggil loadTexture() di awal program untuk memuat semua tekstur
 * 2. Panggil renderScene() di loop utama untuk menggambar seluruh scene
 * 3. Gunakan drawPlayerShadow() untuk menambahkan bayangan ke player
 * 4. Gunakan updatePlayerAnimation() untuk animasi player berdasarkan gerakan
 * 5. Panggil calculateFPS() untuk monitoring performa
 * 
 * ==========================================================================
 */

#include <GL/glut.h>
#include <cstdlib>
#include <iostream> 
#include <cmath>
#include <chrono>
#include <algorithm>
#include <GL/glext.h>
#include "collision.h"
#include "object.h"
#include "render.h"
#include "map.h"
#include "enemy.h"
#include "combat.h"
#include "scenery.h"

#define checkImageWidth 64
#define checkImageHeight 64
  
GLubyte checkImage[checkImageWidth][checkImageHeight][3];
GLubyte wallImage[checkImageWidth][checkImageHeight][3];
GLubyte roofImage[checkImageWidth][checkImageHeight][3];
 
// textures 
GLuint texture[3];
 
Player* pla = &player;
GLuint woodTexture;
GLuint grassTexture; 
 
GLuint loadBMP(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        std::cerr << "Failed to load BMP file: " << filename << std::endl;
        return 0;
    }

    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, file); // Baca header BMP

    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    int imageSize = 3 * width * height;

    unsigned char* data = new unsigned char[imageSize];
    fread(data, sizeof(unsigned char), imageSize, file); // Baca data piksel
    fclose(file);

    // Balikkan data vertikal karena BMP disimpan terbalik
    for (int i = 0; i < height / 2; i++) {
        for (int j = 0; j < width * 3; j++) {
            std::swap(data[i * width * 3 + j], data[(height - i - 1) * width * 3 + j]);
        }
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    delete[] data;
    return textureID;
}

GLfloat shadowMat[16];

// Fungsi untuk menghitung matriks proyeksi bayangan
void calculateShadowMatrix(GLfloat shadowMat[16], GLfloat groundplane[4], GLfloat lightpos[4]) {
    GLfloat dot;
    
    // Menghitung dot product dari light position dan ground plane
    dot = groundplane[0] * lightpos[0] +
          groundplane[1] * lightpos[1] +
          groundplane[2] * lightpos[2] +
          groundplane[3] * lightpos[3];
    
    // Menghitung matriks proyeksi bayangan
    shadowMat[0]  = dot - lightpos[0] * groundplane[0];
    shadowMat[4]  = -lightpos[0] * groundplane[1];
    shadowMat[8]  = -lightpos[0] * groundplane[2];
    shadowMat[12] = -lightpos[0] * groundplane[3];
    
    shadowMat[1]  = -lightpos[1] * groundplane[0];
    shadowMat[5]  = dot - lightpos[1] * groundplane[1];
    shadowMat[9]  = -lightpos[1] * groundplane[2];
    shadowMat[13] = -lightpos[1] * groundplane[3];
    
    shadowMat[2]  = -lightpos[2] * groundplane[0];
    shadowMat[6]  = -lightpos[2] * groundplane[1];
    shadowMat[10] = dot - lightpos[2] * groundplane[2];
    shadowMat[14] = -lightpos[2] * groundplane[3];
    
    shadowMat[3]  = -lightpos[3] * groundplane[0];
    shadowMat[7]  = -lightpos[3] * groundplane[1];
    shadowMat[11] = -lightpos[3] * groundplane[2];
    shadowMat[15] = dot - lightpos[3] * groundplane[3];
}


void drawPlayerShadowSilhouette(const Player& player) {
    glPushMatrix();
    glScalef(0.8f, 0.05f, 0.8f); // Buat bayangan jauh lebih tipis (0.05f untuk ketebalan)
    
    // Gambar siluet tubuh sebagai elips sederhana
    GLUquadricObj *diskQuadric = gluNewQuadric();
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Rotasi disk agar menghadap ke atas
    gluDisk(diskQuadric, 0.0f, 0.4f, 32, 1); // Gambar disk sederhana untuk tubuh
    gluDeleteQuadric(diskQuadric);
    
    glPopMatrix();
}

void updatePlayerAnimation(Player& player, float deltaTime, bool isMoving) {
    static float animationTime = 0.0f;

    if (isMoving) {
        animationTime += deltaTime;

        // Animasi lengan dan kaki (ayunan)
        player.leftArmRotation = sin(animationTime * 5.0f) * 30.0f;  // Ayunan lengan kiri
        player.rightArmRotation = -sin(animationTime * 5.0f) * 30.0f; // Ayunan lengan kanan
        player.leftLegRotation = -sin(animationTime * 5.0f) * 30.0f;  // Ayunan kaki kiri
        player.rightLegRotation = sin(animationTime * 5.0f) * 30.0f;  // Ayunan kaki kanan
    } else {
        // Reset rotasi jika karakter diam
        player.leftArmRotation = 0.0f;
        player.rightArmRotation = 0.0f;
        player.leftLegRotation = 0.0f;
        player.rightLegRotation = 0.0f;
    }
}

void updatePunchAnimation(Player& player, float deltaTime) {
    if (player.isPunching) {
        float oldProgress = player.punchProgress;
        player.punchProgress += deltaTime * 4.0f;
        
        // Check if punch has just reached its peak (around 50% of animation)
        if (oldProgress < 0.5f && player.punchProgress >= 0.5f) {
            // Check for enemy hits at the peak of the punch
            for (auto& enemy : enemies) {
                checkPlayerAttack(player, enemy);
            }
        }
        
        if (player.punchProgress >= 1.0f) {
            player.isPunching = false;
            player.punchProgress = 0.0f;
        }
    }
}

void drawText(const char* text, int x, int y, void* font) {
    glRasterPos2i(x, y); 
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c); 
    }
}

float calculateFPS() {
    static int frameCount = 0;
    static float fps = 0.0f;
    static auto lastTime = std::chrono::steady_clock::now();

    frameCount++;
    auto currentTime = std::chrono::steady_clock::now();
    float elapsedTime = std::chrono::duration<float>(currentTime - lastTime).count();

    if (elapsedTime > 1.0f) {
        fps = frameCount / elapsedTime;
        frameCount = 0;
        lastTime = currentTime;
    }

    return fps;
}

void makeCheckImage(void) 
{
    int i, j;

    for (i = 0; i < checkImageWidth; i++)
    {
        for (j = 0; j < checkImageHeight; j++)
        {
            checkImage[i][j][0] = (GLubyte)0;   // Red
            checkImage[i][j][1] = (GLubyte)255; // Green
            checkImage[i][j][2] = (GLubyte)0;   // Blue
        }
    }
}

void makeWallImage(void)
{
    int i, j;
    for (i = 0; i < checkImageWidth; i++)
    {
        for (j = 0; j < checkImageHeight; j++)
        {
            // Example: Brownish color for wood
            wallImage[i][j][0] = (GLubyte)139; // Red
            wallImage[i][j][1] = (GLubyte)69;  // Green
            wallImage[i][j][2] = (GLubyte)19;  // Blue
        }
    }
}

void makeRoofImage(void)
{
    int i, j;
    for (i = 0; i < checkImageWidth; i++)
    {
        for (j = 0; j < checkImageHeight; j++)
        {
            // Example: Dark red for roof tiles
            roofImage[i][j][0] = (GLubyte)178; // Red
            roofImage[i][j][1] = (GLubyte)34;  // Green
            roofImage[i][j][2] = (GLubyte)34;  // Blue
        }
    }
}

void loadTexture(void) {
    makeCheckImage();
    makeWallImage();

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(2, texture);

    // Wall texture
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, checkImageWidth, checkImageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, wallImage);

    
    // Roof texture
    glBindTexture(GL_TEXTURE_2D, texture[2]); // Gunakan indeks 2 untuk rooftop
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, checkImageWidth, checkImageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, roofImage);

    if (texture[2] == 0) {
        std::cerr << "Failed to load rooftop texture!" << std::endl;
    } else {
        std::cout << "Rooftop texture loaded successfully. Texture ID: " << texture[2] << std::endl;
    }
    
    // Load wood texture
    woodTexture = loadBMP("texture/quartz-block.bmp"); // Ganti "wood.bmp" dengan path file BMP kayu
    grassTexture = loadBMP("texture/grass.bmp"); 
}

void Grid() { 
    double i;
    const float Z_MIN = -300, Z_MAX = 500;
    const float X_MIN = -300, X_MAX = 500;
    const float gap = 1.5;
 
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, grassTexture);

    glColor3f(1.0, 1.0, 1.0); // Warna putih agar tekstur terlihat jelas
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f); // Normal menghadap ke atas
 
    float repeatX = 3000.0f; // Ulangi tekstur 10 kali di sumbu X
    float repeatY = 3000.0f; // Ulangi tekstur 10 kali di sumbu Y

    // Ulangi tekstur 50 kali di setiap sumbu
    glTexCoord2f(0.0f, 0.0f); glVertex3f(X_MIN, -0.11f, Z_MIN);
    glTexCoord2f(repeatX, 0.0f); glVertex3f(X_MAX, -0.11f, Z_MIN);
    glTexCoord2f(repeatX, repeatY); glVertex3f(X_MAX, -0.11f, Z_MAX);
    glTexCoord2f(0.0f, repeatY); glVertex3f(X_MIN, -0.11f, Z_MAX);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void drawPlayerShadow(const Player& player) {
    // Definisikan bidang lantai
    GLfloat floorPlane[4] = {0.0f, 1.0f, 0.0f, 0.11f};
    
    // Definisikan posisi sumber cahaya (arah cahaya dari samping kanan atas)
    GLfloat lightPos[4] = {50.0f, 100.0f, -30.0f, 1.0f}; // Ubah posisi cahaya
    
    // Hitung matriks bayangan
    calculateShadowMatrix(shadowMat, floorPlane, lightPos);
    
    // Simpan state saat ini
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    // Setup stencil buffer
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glStencilMask(0xFF);
    glDepthMask(GL_FALSE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    
    // Reset stencil buffer
    glClear(GL_STENCIL_BUFFER_BIT);
    
    // Gambar lantai ke stencil buffer
    glBegin(GL_QUADS);
    glVertex3f(-1000.0f, -0.11f, -1000.0f);
    glVertex3f( 1000.0f, -0.11f, -1000.0f);
    glVertex3f( 1000.0f, -0.11f,  1000.0f);
    glVertex3f(-1000.0f, -0.11f,  1000.0f);
    glEnd();
    
    // Setup untuk menggambar bayangan
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    
    // Nonaktifkan lighting dan texture
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    // Gunakan alpha blending untuk bayangan transparan
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Polygon offset untuk menghindari z-fighting
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.5, -1.5);
    
    glPushMatrix();
    
    // Terapkan matriks bayangan
    glMultMatrixf(shadowMat);
    
    // Translasi ke posisi player
    glTranslatef(player.position.x, player.position.y, player.position.z);
    glRotatef(player.rotationY, 0.0f, 1.0f, 0.0f);
    
    // Gambar bayangan dengan warna hitam yang lebih pekat
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f); // Tingkatkan alpha ke 0.7
    
    // Gunakan fungsi siluet
    drawPlayerShadowSilhouette(player);
    
    glPopMatrix();   
    // Kembalikan state
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_STENCIL_TEST);
    glPopAttrib();
}

void drawTreeShadow(float x, float z, float scale) {
    // Save current attributes
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    // Disable lighting and texture for shadow
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    // Enable blending for semi-transparent shadow
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Use polygon offset to prevent z-fighting with ground
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.5, -1.5);
    
    // Shadow color (dark with transparency)
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    
    glPushMatrix();
    // Position just above ground to prevent z-fighting
    glTranslatef(x, -0.09f, z);
    
    // No offset needed now since light is directly overhead
    // Apply scale - we're keeping the same scale for the shadow
    float shadowScale = scale * 1.2f;
    
    // Flatten the shadow onto the ground
    glScalef(shadowScale, 0.01f, shadowScale);
    
    // Rotate to lay flat on the ground
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    
    // Draw trunk shadow as a cylinder silhouette
    GLUquadricObj *quadric = gluNewQuadric();
    gluDisk(quadric, 0.0f, 0.2f, 12, 1);
    
    // Draw foliage shadow - directly below without offsets
    gluDisk(quadric, 0.0f, 0.9f, 16, 1);
    
    // With overhead light, we don't need to offset the overlapping disks
    // They should all be centered to create a circular shadow
    
    gluDeleteQuadric(quadric);
    glPopMatrix();
    
    // Restore previous state
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPopAttrib();
}

void drawPineTreeShadow(float x, float z, float scale) {
    // Save current attributes
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    // Disable lighting and texture for shadow
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    // Enable blending for semi-transparent shadow
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Use polygon offset to prevent z-fighting with ground
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.5, -1.5);
    
    // Shadow color (dark with transparency)
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    
    glPushMatrix();
    // Position just above ground to prevent z-fighting
    glTranslatef(x, -0.09f, z);
    
    // Apply scale
    float shadowScale = scale * 1.2f;
    
    // Flatten the shadow onto the ground
    glScalef(shadowScale, 0.01f, shadowScale);
    
    // Rotate to lay flat on the ground
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    
    // Draw trunk shadow
    GLUquadricObj *quadric = gluNewQuadric();
    gluDisk(quadric, 0.0f, 0.15f, 10, 1);
    
    // Draw pine tree shadow - nested concentric circles for conical shape
    // With overhead light, the shadow should be a series of concentric circles
    gluDisk(quadric, 0.0f, 0.9f, 16, 1); // Base layer
    gluDisk(quadric, 0.0f, 0.7f, 14, 1); // Middle layer
    gluDisk(quadric, 0.0f, 0.5f, 12, 1); // Upper layer
    gluDisk(quadric, 0.0f, 0.3f, 10, 1); // Top layer
    
    gluDeleteQuadric(quadric);
    glPopMatrix();
    
    // Restore previous state
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPopAttrib();
}

void renderScene() {
    std::cout << "Rendering scene" << std::endl;
    
    // Clear the screen and depth buffer
    glClearColor(0.529f, 0.808f, 0.922f, 1.0f); // Light blue sky
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // Draw skybox
    drawSkybox();
    
    // Draw ground grid 
    Grid();
    
    // Draw distant scenery first (buildings, mountains)
    drawDistantScenery();
    
    // Draw all scenery objects
    drawScenery();
    
    // Draw the map (walls, fences, etc)
    drawMap();
    
    // Draw bullets
    drawBullets();

    // Draw enemies
    for (auto& enemy : enemies) {
        if (enemy.isAlive) {
            drawEnemy(enemy);
        }
    }

    // Draw player only if not in first-person mode
    extern bool isFirstPersonView;
    if (!isFirstPersonView) {
        // Draw player and player shadow
        drawPlayer(player);
        drawPlayerShadow(player);
    }
    
    std::cout << "Scene rendered" << std::endl;
}

void drawSkybox() {
    // Save matrix state
    glPushMatrix();
    
    // Move skybox with camera position
    extern float cameraX, cameraY, cameraZ;
    glTranslatef(cameraX, 0, cameraZ);
    
    // Disable lighting for skybox
    glDisable(GL_LIGHTING);
    
    // Draw sky as a gradient
    glBegin(GL_QUADS);
    
    // Top - lighter blue
    glColor3f(0.4f, 0.7f, 1.0f);
    glVertex3f(-500.0f, 300.0f, -500.0f);
    glVertex3f(-500.0f, 300.0f,  500.0f);
    glVertex3f( 500.0f, 300.0f,  500.0f);
    glVertex3f( 500.0f, 300.0f, -500.0f);

    // Front, blend from top to horizon
    glColor3f(0.4f, 0.7f, 1.0f);
    glVertex3f(-500.0f, 300.0f, -500.0f);
    glVertex3f( 500.0f, 300.0f, -500.0f);
    glColor3f(0.7f, 0.85f, 1.0f);  // Lighter at horizon
    glVertex3f( 500.0f, 0.0f, -500.0f);
    glVertex3f(-500.0f, 0.0f, -500.0f);

    // Back
    glColor3f(0.4f, 0.7f, 1.0f);
    glVertex3f(-500.0f, 300.0f, 500.0f);
    glVertex3f( 500.0f, 300.0f, 500.0f);
    glColor3f(0.7f, 0.85f, 1.0f);
    glVertex3f( 500.0f, 0.0f, 500.0f);
    glVertex3f(-500.0f, 0.0f, 500.0f);

    // Left
    glColor3f(0.4f, 0.7f, 1.0f);
    glVertex3f(-500.0f, 300.0f, -500.0f);
    glVertex3f(-500.0f, 300.0f,  500.0f);
    glColor3f(0.7f, 0.85f, 1.0f);
    glVertex3f(-500.0f, 0.0f,  500.0f);
    glVertex3f(-500.0f, 0.0f, -500.0f);

    // Right
    glColor3f(0.4f, 0.7f, 1.0f);
    glVertex3f(500.0f, 300.0f, -500.0f);
    glVertex3f(500.0f, 300.0f,  500.0f);
    glColor3f(0.7f, 0.85f, 1.0f);
    glVertex3f(500.0f, 0.0f,  500.0f);
    glVertex3f(500.0f, 0.0f, -500.0f);
    
    glEnd();
    
    // Re-enable lighting
    glEnable(GL_LIGHTING);
    
    glPopMatrix();
}

void drawDistantScenery() {
    glPushMatrix();
    
    glDisable(GL_LIGHTING);
    
    // Draw mountain range
    for (int i = 0; i < 10; i++) {
        float mountainX = -400 + i * 100;
        float mountainZ = -350;
        float mountainHeight = 120 + (i % 3) * 40;  // Varied heights
        
        // Blue-gray mountain color
        glColor3f(0.5f, 0.55f, 0.6f);
        
        // Simple triangle for distant mountain
        glBegin(GL_TRIANGLES);
        glVertex3f(mountainX - 80, 0, mountainZ);
        glVertex3f(mountainX + 80, 0, mountainZ);
        glVertex3f(mountainX, mountainHeight, mountainZ);
        glEnd();
    }
    
    glEnable(GL_LIGHTING);
    
    glPopMatrix();
}


