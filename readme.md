# [Unknown Game] - Proyek Grafika Komputasi Visual

Dokumentasi ini menjelaskan proyek game sederhana yang dibuat menggunakan C/C++ dengan library OpenGL dan GLUT sebagai bagian dari tugas besar mata kuliah Grafika dan Komputasi Visual.

**Tim Pengembang:**
*   Gibran Ahmad Maulana - 24060123130112
*   Haidar Ali Laudza - 24060123140151
*   Indah Nurul Janah - 24060123120009

**Daftar Isi:**
1.  [Pendahuluan](#1-pendahuluan)
2.  [Setup & Instalasi](#2-setup--instalasi)
3.  [Struktur Proyek](#3-struktur-proyek)
4.  [Konsep Inti & Modul](#4-konsep-inti--modul)
5.  [Progres Saat Ini](#5-progres-saat-ini-sekitar-50)
6.  [Rencana Selanjutnya (TODO)](#6-rencana-selanjutnya-todo)
7.  [Cara Berkontribusi (Opsional)](#7-cara-berkontribusi-opsional)
8.  [Referensi & Sumber Daya (Opsional)](#8-referensi--sumber-daya-opsional)

---

## 1. Pendahuluan

### 1.1. Deskripsi Game
[Jelaskan secara singkat konsep game kamu di sini. Apa genre-nya? Apa tujuan pemain? Apa saja fitur utamanya? Contoh: "Ini adalah game arcade 2D di mana pemain mengendalikan pesawat untuk menghindari rintangan dan mengumpulkan poin."]

### 1.2. Tujuan Proyek
[Sebutkan tujuan utama dari pengembangan proyek ini sesuai tugas. Contoh: "Mengimplementasikan konsep dasar grafika komputer seperti transformasi, viewing, pencahayaan (jika ada), interaksi pengguna, dan rendering objek 2D/3D menggunakan OpenGL dan GLUT."]

### 1.3. Teknologi Utama
*   **Bahasa Pemrograman:** C++
*   **Grafika API:** OpenGL
*   **Windowing & Input:** GLUT
*   **Compiler:** GCC, MinGW
*   **IDE:** Dev-C++

---

## 2. Setup & Instalasi

### 2.1. Prasyarat (Tersedia di folder Install)
Pastikan sistem Anda memiliki:
*   IDE Dev-C++ (File installer ada pada foler Install)
*   Library GLUT terinstal.
    *   Cara Menginstall GLUT : 
        *   Salin folder "lib" pada folder GLUTMingw32, pastekan pada folder C:\Dev-Cpp\ (timpa/replace)
        *   Kemudian salin isi folder lib (4 file) yang terdapat pada C:\Dev-Cpp\lib, tempelkan pada folder C:\Dev-Cpp\MinGW32\lib
        *   Salin folder "include" pada folder GLUTMingw32, lalu tempelkan pada C:\Dev-Cpp (timpa/replace)
        *   Salin isi dari folder "include" yang terdapat pada C:\Dev-Cpp\MinGW32\include (ada 2 folder), lalu tempel pada folder C:\Dev-Cpp\MinGW32\include\
*   Salin file glut32.dll pada direktori C:\Windows\System32 untuk windows 32 bit, dan C:\Windows\SysWOW64 untuk windows 64 bit
*   Install "glut-7.6-3.DevPak" dengan menggunakan package manager yang terdapat pada Dev-C++

### 2.2. Kompilasi Proyek

Compile proyek dengan menekan tombol f11 pada aplikasi Dev-C++, apabila terdapat error, maka atur pada Compiler Options dengan cara: 
* Buka Compiler Options pada tab Tools
* Centang "Add the following commands when calling the compiler:"
* Isikan -std=gnu++11
* Klik OK

### 2.3. Menjalankan Game

Setelah kompilasi berhasil, aplikasi akan otomatis terbuka. Atau bisa dengan membuka file tubes.exe yang ada pada folder

## 3. Struktur Proyek

Berikut adalah gambaran struktur direktori dan file utama dalam proyek ini:

.
├── src/                 # Folder berisi file source code (.cpp)
│   ├── main.cpp         # File utama, inisialisasi GLUT, game loop
│   ├── player.cpp       # Logika untuk pemain (jika dipisah)
│   ├── objects.cpp      # Logika untuk objek lain (rintangan, item, dll)
│   └── ...              # File source code lainnya
├── include/             # Folder berisi file header (.h atau .hpp)
│   ├── player.h         # Definisi struct/class pemain
│   ├── objects.h        # Definisi struct/class objek lain
│   └── ...              # File header lainnya
├── assets/              # Folder berisi aset (jika ada, misal: tekstur, suara)
│   └── textures/
│       └── player_texture.bmp
├── README.md            # File dokumentasi ini
├── Makefile             # (Jika menggunakan Make) File untuk build otomatis
└── tubes.exe            # (Hasil kompilasi) File executable game


main.cpp: Jelaskan peran utamanya (inisialisasi OpenGL/GLUT, registrasi callback, glutMainLoop()).



4. Konsep Inti & Modul

Bagian ini menjelaskan komponen-komponen utama dan cara kerjanya dalam game.

4.1. Game Loop Utama (main.cpp)

Inisialisasi: Pengaturan window GLUT (glutInit, glutInitDisplayMode, glutInitWindowSize, glutCreateWindow).

Callback GLUT:

glutDisplayFunc(display): Fungsi yang dipanggil untuk me-render frame. [Sebutkan nama fungsi render utama kamu].

glutReshapeFunc(reshape): Menangani perubahan ukuran window. [Sebutkan nama fungsinya].

glutKeyboardFunc(keyboard): Menangani input keyboard (tombol ditekan). [Sebutkan nama fungsinya].

glutSpecialFunc(specialKeys): Menangani input tombol spesial (panah, F1, dll.). [Sebutkan nama fungsinya].

glutMouseFunc(mouse): Menangani input klik mouse. [Sebutkan nama fungsinya].

glutMotionFunc(mouseMotion): Menangani input gerakan mouse saat tombol ditekan. [Sebutkan nama fungsinya].

glutPassiveMotionFunc(passiveMouseMotion): Menangani input gerakan mouse saat tidak ada tombol ditekan.

glutIdleFunc(idle): Fungsi yang dipanggil saat tidak ada event lain (biasanya untuk update logika game). [Sebutkan nama fungsinya atau jelaskan jika update dilakukan di timer/display].

glutTimerFunc(timer): (Jika digunakan) Untuk update logika game secara berkala.

glutMainLoop(): Memulai loop event GLUT.

4.2. Rendering (display() atau fungsi render lainnya)

Pembersihan Buffer: glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT).

Pengaturan Kamera/View: Menggunakan gluLookAt atau matriks transformasi glMatrixMode(GL_PROJECTION), glLoadIdentity(), gluPerspective/glOrtho, glMatrixMode(GL_MODELVIEW), glLoadIdentity(). [Jelaskan bagaimana view di-setup].

Penggambaran Objek:

Menggunakan OpenGL Primitives (glBegin, glEnd, glVertex, glColor).

Transformasi objek (glPushMatrix, glPopMatrix, glTranslatef, glRotatef, glScalef).

[Jelaskan cara objek-objek utama (pemain, musuh, latar) digambar].

[Sebutkan jika menggunakan pencahayaan (glEnable(GL_LIGHTING)) atau tekstur (glEnable(GL_TEXTURE_2D))].

Swap Buffer: glutSwapBuffers().

4.3. Input Handling (Fungsi callback keyboard/mouse)

[Jelaskan bagaimana input dari keyboard/mouse diproses untuk mengontrol pemain atau interaksi lainnya].

[Sebutkan variabel global atau state yang diubah oleh fungsi input].

4.4. Logika Game & State (Fungsi idle() atau timer() atau di dalam display())

[Jelaskan bagaimana state game di-update setiap frame/interval].

[Bagaimana posisi objek diperbarui?].

[Bagaimana deteksi kolisi dilakukan (jika ada)?].

[Bagaimana skor dihitung (jika ada)?].

[Apakah ada state game yang berbeda (misal: Menu Utama, Gameplay, Game Over)? Bagaimana transisinya diatur?].

4.5. Representasi Objek (Structs/Classes di .h / .cpp)

[Jelaskan bagaimana entitas dalam game (pemain, musuh, peluru, item) direpresentasikan dalam kode. Apakah menggunakan struct atau class?].

[Sebutkan atribut penting dari setiap struct/class (posisi, kecepatan, ukuran, warna, status, dll.)].

[Contoh singkat definisi struct/class jika perlu].

5. Progres Saat Ini (Sekitar 50%)

Berikut adalah fitur-fitur yang sudah diimplementasikan hingga saat ini:

Inisialisasi window GLUT dan OpenGL context.

Penggambaran latar belakang statis/dinamis.

Penggambaran objek pemain.

Kontrol gerakan dasar pemain menggunakan [keyboard/mouse].

[Tambahkan fitur lain yang sudah selesai, misal: Penggambaran rintangan awal, sistem skor sederhana, dll.].

[Sebutkan bagian kode mana yang sudah relatif stabil].

6. Rencana Selanjutnya (TODO)

Berikut adalah fitur-fitur atau perbaikan yang perlu dikerjakan:

Gameplay:

Implementasi spawn rintangan/musuh secara acak/terpola.

Implementasi deteksi kolisi antara pemain dan rintangan/musuh.

Implementasi sistem skor yang lebih baik / nyawa pemain.

Implementasi kondisi menang/kalah (Game Over).

Tambahkan [Fitur Gameplay Spesifik Lainnya].

Grafis:

Tambahkan tekstur pada objek [pemain/rintangan/latar].

Implementasi efek pencahayaan sederhana (jika relevan).

Perbaiki/tambahkan animasi sederhana untuk [objek tertentu].

Buat tampilan UI/HUD untuk skor/nyawa.

Kode & Struktur:

Refactor kode [bagian tertentu] agar lebih modular/rapi.

Optimasi rendering jika terasa lambat.

Tambahkan komentar pada bagian kode yang kompleks.

Lain-lain:

Tambahkan suara/musik (mungkin di luar scope OpenGL/GLUT dasar).

Implementasi layar Menu Utama.

(Tandai [x] jika sudah selesai, atau biarkan [ ] untuk yang belum. Tambahkan/ubah sesuai kebutuhan proyekmu).

7. Cara Berkontribusi (Opsional)

Jika bekerja dalam tim, bagian ini bisa membantu menyelaraskan cara kerja:

Komunikasi: Gunakan [Platform Komunikasi, misal: WhatsApp Group, Discord] untuk koordinasi.

Version Control (Sangat Direkomendasikan): Gunakan Git dan [Platform Hosting, misal: GitHub, GitLab].

Branching Strategy: [Misal: Buat branch baru untuk setiap fitur besar (feature/nama-fitur), merge ke main/develop setelah selesai dan di-review].

Coding Style: Usahakan konsisten dengan gaya kode yang sudah ada (penamaan variabel, indentasi, dll.). [Bisa sebutkan panduan gaya spesifik jika ada].

Testing: Pastikan kode yang baru ditambahkan tidak merusak fungsionalitas yang sudah ada sebelum di-merge.

8. Referensi & Sumber Daya (Opsional)

[Link ke tutorial OpenGL yang kamu ikuti]

[Link ke dokumentasi GLUT/FreeGLUT]

[Link ke sumber aset (jika ada)]

[Referensi lain yang relevan]

**Cara Menggunakan Kerangka Ini:**

1.  **Salin dan Tempel:** Salin teks di atas ke dalam file baru bernama `README.md` di folder utama proyekmu.
2.  **Isi Bagian yang Kosong:** Ganti semua teks yang ada di dalam kurung siku `[...]` dengan informasi spesifik proyekmu.
3.  **Hapus/Tambah Bagian:** Sesuaikan bagian-bagian dokumentasi. Jika ada bagian yang tidak relevan (misalnya tidak ada aset), hapus saja. Jika ada konsep penting lain di proyekmu, tambahkan bagian baru.
4.  **Detailkan:** Berikan penjelasan yang cukup detail pada bagian "Konsep Inti & Modul" dan "Struktur Proyek". Semakin jelas, semakin mudah temanmu memahami.
5.  **Update Secara Berkala:** Saat ada perubahan besar atau progres baru, jangan lupa update file `README.md` ini, terutama bagian "Progres Saat Ini" dan "TODO".

Dengan dokumentasi seperti ini, temanmu akan punya panduan yang jelas untuk memahami apa yang sudah kamu kerjakan dan bagaimana melanjutkan pengembangan game tersebut. Semoga berhasil dengan tugas besarnya!
IGNORE_WHEN_COPYING_START
content_copy
download
Use code with caution.
IGNORE_WHEN_COPYING_END