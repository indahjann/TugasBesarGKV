/*
 * ==========================================================================
 * MAP SYSTEM IMPLEMENTATION
 * ==========================================================================
 * 
 * File: map.cpp
 * Author: Project Team
 * Date: May 2, 2025
 * 
 * DESCRIPTION: 
 * -----------
 * Implementasi sistem peta (map) untuk game, termasuk:
 * - Pengaturan layout peta dasar
 * - Pendefinisian dinding (walls) untuk struktur bangunan
 * - Pendefinisian pagar (fences) untuk membatasi area permainan
 * - Rendering elemen-elemen peta
 * 
 * STRUCTURES:
 * ---------- 
 * WallDefinition - Didefinisikan di map.h
 *   - Struktur untuk mendefinisikan dinding dengan titik awal, titik akhir, tinggi, dan ketebalan
 * 
 * FenceDefinition - Didefinisikan di map.h
 *   - Struktur untuk mendefinisikan pagar dengan titik awal, titik akhir, tinggi, ketebalan, dan jumlah tiang
 * 
 * GLOBAL VARIABLES:
 * ---------------
 * walls  - Vector yang menyimpan semua definisi dinding
 * fences - Vector yang menyimpan semua definisi pagar
 * 
 * FUNCTIONS:
 * ---------
 * initMap()       - Menginisialisasi layout peta dengan dinding-dinding dasar
 *                   Membuat struktur rumah sederhana dengan empat dinding
 * 
 * drawBorderMap() - Menambahkan pagar pembatas di sekeliling area permainan
 *                   Membuat empat pagar pembatas (depan, belakang, kiri, kanan)
 * 
 * drawMap()       - Menggambar seluruh elemen peta
 *                   Memanggil fungsi drawWall() dan drawFence() untuk setiap elemen
 * 
 * DEPENDENCIES:
 * -----------
 * - map.h       - Header file dengan deklarasi fungsi dan struct untuk peta
 * - object.h    - Header untuk akses ke fungsi drawWall dan drawFence
 * - render.h    - Header untuk fungsi rendering
 * - GL/glut.h   - Library OpenGL untuk fungsi rendering
 * - vector      - STL untuk menyimpan koleksi dinding dan pagar
 * 
 * HOW TO USE:
 * ----------
 * 1. Panggil initMap() di awal game untuk menginisialisasi layout dasar
 * 2. Panggil drawBorderMap() untuk menambahkan pagar pembatas area permainan
 * 3. Panggil drawMap() di loop rendering untuk menampilkan seluruh elemen peta
 * 4. Untuk menambahkan dinding atau pagar baru, tambahkan di initMap() atau drawBorderMap()
 * 
 * MAP LAYOUT:
 * ----------
 * - Struktur rumah sederhana di tengah peta
 * - Area permainan dibatasi oleh pagar dengan ukuran 200x200 units (-100 sampai 100)
 * - Format dinding: {x1, z1, x2, z2, baseY, height, thickness}
 * - Format pagar: {startX, startZ, endX, endZ, height, thickness, numPosts}
 * 
 * ==========================================================================
 */

#include <GL/glut.h>
#include "object.h"
#include "render.h"
#include "map.h"
#include <vector>
#include <cmath>

// Vector to store all wall definitions
std::vector<WallDefinition> walls;
std::vector<FenceDefinition> fences; 
std::vector<Rooftop> rooftops;
std::vector<StaircaseDefinition> staircases;


void initMap() {
    walls.clear(); 
    rooftops.clear();
    staircases.clear(); 
     
     
    float baseY = 0.0f;       
    float wallHeight = 5.9f;  
    float wallThickness = 0.5f;
    

    // ===== LANTAI 1 =====
    // Dinding luar
    walls.push_back({-10.0f, 10.0f, -10.0f, 30.0f, baseY, wallHeight, wallThickness}); // West wall
    walls.push_back({10.0f, 10.0f, 10.0f, 30.0f, baseY, wallHeight, wallThickness});   // East wall
    walls.push_back({-10.0f, 30.0f, 10.0f, 30.0f, baseY, wallHeight, wallThickness});    // North wall (digeser sedikit ke dalam)

    // Dinding selatan dengan pintu (dibagi dua)
    walls.push_back({-10.0f, 10.0f, -2.5f, 10.0f, baseY, wallHeight, wallThickness});   // Left of door (digeser sedikit ke dalam)
    walls.push_back({0.0f, 10.0f, 9.5f, 10.0f, baseY, wallHeight, wallThickness});     // Right of door (digeser sedikit ke dalam)
    walls.push_back({5.0f, 10.1f, 5.0f, 17.0f, baseY, wallHeight, wallThickness});     // Right of door (digeser sedikit ke dalam)

    // Pintu dalam
    walls.push_back({-2.5f, 10.1f, -2.5f, 17.0f, baseY, wallHeight, wallThickness});     // Central divider bottom
    walls.push_back({0.0f, 10.0f, 0.0f, 25.0f, baseY, wallHeight, wallThickness});     // Central divider top
    walls.push_back({-5.0f, 20.1f, -5.0f, 25.0f, baseY, wallHeight, wallThickness});     // Central divider top

    // Dinding horizontal dengan pintu (digeser sedikit untuk menghindari tumpang tindih)
    walls.push_back({-9.5f, 20.0f, -5.0f, 20.0f, baseY, wallHeight, wallThickness});   // Left section
    // walls.push_back({-2.0f, 20.0f, -0.25f, 20.0f, baseY, wallHeight, wallThickness});  // Middle left
    // walls.push_back({0.25f, 20.0f, 2.0f, 20.0f, baseY, wallHeight, wallThickness});    // Middle right
    walls.push_back({5.0f, 20.0f, 9.5f, 20.0f, baseY, wallHeight, wallThickness});     // Right section

     // ===== ATAP LANTAI 1 =====
    float roofThickness = 0.5f;
    float firstFloorRoofY = baseY + wallHeight; // Tambah offset kecil untuk menghindari z-fighting

    // Atap lantai 1 (full coverage dengan margin dari dinding)
    rooftops.push_back({-9.99f, 10.1f, 9.99f, 29.5f, firstFloorRoofY, roofThickness}); // Atap penuh lantai 1


    // ===== LANTAI 2 =====
    float secondFloorBaseY = firstFloorRoofY; // Mulai dari atas atap lantai 1
    float secondFloorHeight = 5.9f;

    // Dinding luar lantai 2
    //walls.push_back({-10.0f, 20.0f, -10.0f, 30.0f, secondFloorBaseY, secondFloorHeight, wallThickness}); // West wall
    walls.push_back({10.0f, 20.0f, 10.0f, 30.0f, secondFloorBaseY, secondFloorHeight, wallThickness});   // East wall
    walls.push_back({-10.0f, 30.0f, 10.0f, 30.0f, secondFloorBaseY, secondFloorHeight, wallThickness});  // North wall

    // Pintu ke rooftop (gap di dinding selatan lantai 2)
    walls.push_back({-10.0f, 20.0f, -2.0f, 20.0f, secondFloorBaseY, secondFloorHeight, wallThickness});
    walls.push_back({2.0f, 20.0f, 9.75f, 20.0f, secondFloorBaseY, secondFloorHeight, wallThickness});

    // Atap lantai 2
    float secondFloorRoofY = secondFloorBaseY + secondFloorHeight;
    rooftops.push_back({-9.5f, 20.1f, 9.75f, 29.9f, secondFloorRoofY, roofThickness});

    // Atap bergigi
    float toothHeight = 1.0f; // Tinggi gigi
    float toothWidth = 2.0f;  // Lebar gigi
    float gapWidth = 1.0f;    // Jarak antar gigi
    float secondFloorTopY = secondFloorBaseY + secondFloorHeight; // Posisi atas lantai 2

    // Pola bergigi di atas dinding utara lantai 2 (sepanjang z=30)
    for (float x = -10.0f; x <= 10.0f - toothWidth; x += toothWidth + gapWidth) {
        walls.push_back({x, 30.0f, x + toothWidth, 30.0f, secondFloorTopY, toothHeight, wallThickness});
    }

    // Pola bergigi di atas dinding timur lantai 2 (x=10)
    for (float z = 20.0f; z <= 30.0f - toothWidth; z += toothWidth + gapWidth) {
        walls.push_back({10.0f, z, 10.0f, z + toothWidth, secondFloorTopY, toothHeight, wallThickness});
    }

    // Pola bergigi di atas dinding selatan lantai 2 (z=20)
    for (float x = -10.0f; x <= 10.0f - toothWidth; x += toothWidth + gapWidth) {
        walls.push_back({x, 20.0f, x + toothWidth, 20.0f, secondFloorTopY, toothHeight, wallThickness});
    }

    // Pola bergigi di atas dinding barat lantai 2 (x=-10)
    for (float z = 20.0f; z <= 30.0f - toothWidth; z += toothWidth + gapWidth) {
        walls.push_back({-10.0f, z, -10.0f, z + toothWidth, secondFloorTopY, toothHeight, wallThickness});
    }


    // Atap bergigi lantai 1
    // Gigi di sisi depan (z=10) termasuk area pintu
	for (float x = -10.0f; x <= 10.0f - toothWidth; x += toothWidth + gapWidth) {
	    walls.push_back({
	        x, 10.0f,
	        x + toothWidth, 10.0f,
	        firstFloorRoofY,
	        toothHeight,
	        wallThickness
	    });
	}

    // Gigi di sisi kanan (x=10)
    for (float z = 10.0f; z <= 20.0f - toothWidth; z += toothWidth + gapWidth) {
        walls.push_back({
            10.0f, z,
            10.0f, z + toothWidth,
            firstFloorRoofY,
            toothHeight,
            wallThickness
        });
    }

    // Gigi di sisi kiri (x=-10)
    for (float z = 10.0f; z <= 20.0f - toothWidth; z += toothWidth + gapWidth) {
        walls.push_back({
            -10.0f, z,
            -10.0f, z + toothWidth,
            firstFloorRoofY,
            toothHeight,
            wallThickness
        });
    }

    // ===== ADD EXTERIOR STAIRCASE =====
    float stairStartX = -13.0f;
    float stairStartZ = 10.0f;
    float stairWidth = 2.5f;
    float stairStepHeight = 0.4f;
    float stairStepDepth = 0.8f;

    // Hitung jumlah step untuk mencapai atap lantai 1
    float targetHeight = firstFloorRoofY; // Ubah target ke tinggi atap lantai 1
    int stairSteps = ceil(targetHeight / stairStepHeight);

    // Tambah tangga utama
    staircases.push_back({
        stairStartX, 
        stairStartZ, 
        stairWidth, 
        stairStepDepth, 
        stairStepHeight, 
        stairSteps, 
        baseY
    });

    // Hitung posisi landing platform
    float stairEndZ = stairStartZ + (stairStepDepth * stairSteps);

    // Landing platform yang menghubungkan ke atap lantai 1
    rooftops.push_back({
        stairStartX,                     // Mulai dari posisi tangga
        stairEndZ,                       // Mulai setelah tangga terakhir (tidak menumpuk)
        -10.0f,                          // Sampai ke dinding
        stairEndZ + 2.0f,               // Area landing yang cukup
        firstFloorRoofY + 0.1f,   // Sejajar dengan step terakhir
        roofThickness
    });

        // Add side walls to the staircase for visual appearance
    float barrierY = baseY;
    float barrierHeight = stairSteps * stairStepHeight + 2.0f; // Tambah tinggi barrier
    float barrierThickness = 0.3f;

    // Back barrier untuk landing platform
    walls.push_back({
        stairStartX - 0.1f,                    // Left edge
        stairEndZ + 2.0f,                      // Back of landing platform
        stairStartX + stairWidth + 0.1f,       // Right edge
        stairEndZ + 2.0f,                      // Same Z coordinate
        barrierY, 
        barrierHeight,                         // Barrier lebih tinggi
        barrierThickness
    });


    // Left side wall
    walls.push_back({
        stairStartX - 0.1f,                  // Left edge
        stairStartZ,                         // Start of stairs
        stairStartX - 0.1f,                  // Same X coordinate
        stairEndZ + 2.0f,                    // Extended to end of landing
        barrierY, 
        barrierHeight,                       // Barrier lebih tinggi
        barrierThickness
    });

    // Bagian bawah dinding barat lantai 2
    walls.push_back({
        -10.0f,                         // x1
        20.0f,                          // z1
        -10.0f,                         // x2 
        stairEndZ - 0.5f,              // z2 - posisi pintu sesuai landing platform
        secondFloorBaseY,              // y
        secondFloorHeight,             // height
        wallThickness                  // thickness
    });

    // Bagian atas dinding barat lantai 2
    walls.push_back({
        -10.0f,                         // x1
        stairEndZ + 2.5f,              // z1 - setelah pintu
        -10.0f,                         // x2
        30.0f,                          // z2
        secondFloorBaseY,              // y
        secondFloorHeight,             // height
        wallThickness                  // thickness
    });

    // Tambahkan frame atas pintu
    walls.push_back({
        -10.0f,                         // x1
        stairEndZ - 0.5f,              // z1
        -10.0f,                         // x2
        stairEndZ + 2.5f,              // z2
        secondFloorBaseY + 3.5f,       // y - posisi frame atas
        secondFloorHeight - 3.5f,      // height - ketinggian frame
        wallThickness                  // thickness
    });

}

void drawBorderMap() {
    // Gunakan titik awal dan akhir untuk setiap sisi pagar
    
    // Sisi depan
    fences.push_back({-100.0f, 100.0f, 100.0f, 100.0f, 2.0f, 0.2f, 140});
    
    // Sisi belakang
    fences.push_back({-100.0f, -100.0f, 100.0f, -100.0f, 2.0f, 0.2f, 140});
    
    // Sisi kiri
    fences.push_back({-100.0f, 100.0f, -100.0f, -100.0f, 2.0f, 0.2f, 140});
    
    // Sisi kanan
    fences.push_back({100.0f, 100.0f, 100.0f, -100.0f, 2.0f, 0.2f, 140});
    
    // Contoh pagar diagonal
    // drawFence(-5.0f, 5.0f, 5.0f, 15.0f, 2.0f, 0.2f, 8);
}

void drawMap() {
    // Draw all walls based on defined layout
    for (const auto& wall : walls) {
        drawWall(wall.x1, wall.z1, wall.x2, wall.z2, wall.y, wall.height, wall.thickness);
    }

    for (const auto& fence : fences) {
        drawFence(fence.startX, fence.startZ, fence.endX, fence.endZ, fence.height, fence.thickness, fence.numPosts);
    }

    for (const auto& rooftop : rooftops) {
        drawRooftop(rooftop.x1, rooftop.z1, rooftop.x2, rooftop.z2, rooftop.y, rooftop.thickness);
    }
    
    // Draw all staircases
    for (const auto& stair : staircases) {
        drawStaircase(stair.startX, stair.startZ, stair.width, stair.stepDepth, stair.stepHeight, stair.numSteps, stair.baseY);
    }
}
