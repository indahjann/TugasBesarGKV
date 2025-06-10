/*
 * ==========================================================================
 * LIGHTING SYSTEM IMPLEMENTATION
 * ==========================================================================
 * 
 * File: lighting.cpp
 * Author: Project Team
 * Date: May 2, 2025
 * 
 * DESCRIPTION:
 * -----------
 * Implementasi sistem pencahayaan (lighting) untuk game, termasuk:
 * - Pengaturan cahaya ambient, diffuse, dan specular
 * - Pengaturan material untuk objek di scene
 * - Posisi dan arah pencahayaan
 * - Konfigurasi pencahayaan model dan bayangan
 * 
 * LIGHTING PARAMETERS:  
 * ------------------
 * light_ambient[]  - Nilai warna untuk cahaya ambient (latar)
 * light_diffuse[]  - Nilai warna untuk cahaya diffuse (menyebar)
 * light_specular[] - Nilai warna untuk cahaya specular (kilau)
 * light_position[] - Posisi sumber cahaya (directional light)
 * mat_ambient[]    - Nilai untuk ambient material
 * mat_diffuse[]    - Nilai untuk diffuse material
 * mat_specular[]   - Nilai untuk specular material
 * high_shininess[] - Nilai shininess untuk material (kilau)
 * 
 * FUNCTIONS:
 * ---------
 * initLighting() - Menginisialisasi sistem pencahayaan dan mengaktifkan fitur OpenGL
 *                  yang diperlukan untuk rendering dengan pencahayaan
 * 
 * DEPENDENCIES:
 * -----------
 * - lighting.h - Header file dengan deklarasi fungsi pencahayaan
 * - GL/glut.h  - Library OpenGL untuk fungsi pencahayaan
 * 
 * HOW TO USE:
 * ----------
 * 1. Panggil initLighting() setelah inisialisasi OpenGL dan sebelum rendering
 * 2. Untuk menonaktifkan pencahayaan sementara, gunakan glDisable(GL_LIGHTING)
 * 3. Untuk mengaktifkannya kembali, gunakan glEnable(GL_LIGHTING)
 * 4. Pengaturan material dapat dimodifikasi per-objek dengan fungsi glMaterialfv()
 * 
 * NOTES:
 * -----
 * - Pencahayaan yang digunakan adalah directional light (GL_LIGHT0)
 * - Sistem pencahayaan mendukung two-sided lighting dan local viewer model
 * - Normalisasi vektor normal diaktifkan untuk pencahayaan yang akurat
 * 
 * ==========================================================================
 */

#include "lighting.h"

// Lighting parameters
const GLfloat light_ambient[] = {0.6f, 0.6f, 0.6f, 1.0f};    // Reduced ambient light
const GLfloat light_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};    // Reduced diffuse light
const GLfloat light_specular[] = {0.5f, 0.5f, 0.5f, 1.0f};   // Reduced specular light
const GLfloat light_position[] = {0.0f, 10.0f, 0.0f, 0.0f}; // Directional light
const GLfloat mat_ambient[] = {0.7f, 0.7f, 0.7f, 1.0f};
const GLfloat mat_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
const GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat high_shininess[] = {100.0f};
    
void initLighting() 
{  
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_LIGHT0); 
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    glEnable(GL_LIGHT_MODEL_LOCAL_VIEWER);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}  
