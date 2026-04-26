[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/b842RB8g)

# Tugas Besar 1 IF2010 Pemrograman Berorientasi Objek

Nimonspoliiiiiii

## Build and Formatting

We use CMake + clang-format biar accecee

- Gunakan konfigurasi CMake yang sudah ada untuk kompilasi:

```bash
cmake -S . -B cmake-build
cmake --build cmake-build
```

- Format kode sebelum push!!!

```bash
clang-format -i src/**/*.cpp include/**/*.hpp test/**/*.cpp
```

Jika shell tidak mendukung glob `**`, gunakan pendekatan `find`:

## Unit Test

Semua file test dengan pola `test/**/*_test.cpp` akan otomatis didaftarkan sebagai target CMake saat configure.

- Configure project:

```bash
cmake -S . -B cmake-build
```

- Lihat daftar target test yang tersedia:

```bash
cmake --build cmake-build --target help
```

- Build satu target test (contoh: `property_test`):

```bash
cmake --build cmake-build --target property_test
```

- Build semua target test sekaligus:

```bash
cmake --build cmake-build --target all_tests
```

- Jalankan executable test:

```bash
./cmake-build/property_test
```

Jika build generator yang dipakai membuat binary di subfolder konfigurasi, jalankan `property_test.exe` dari folder seperti `cmake-build/Debug` atau `cmake-build/Release`.

Contoh PowerShell (Windows):

```powershell
.\cmake-build\property_test.exe
```

Contoh WSL/Linux shell:

```bash
./cmake-build/property_test
```

## GUI Mode (Raylib)

Permainan ini mendukung mode GUI dengan raylib. Untuk mengaktifkan mode GUI, pastikan raylib terinstal di sistem Anda.

### Instalasi Raylib

#### Linux (Debian/Ubuntu)

```bash
sudo apt-get install libraylib-dev
```

Jika paket tidak tersedia di distro Anda, build dari source:

```bash
git clone https://github.com/raysan5/raylib.git
cd raylib
mkdir build && cd build
cmake -DBUILD_SHARED_LIBS=ON ..
cmake --build . --target install
sudo ldconfig
```

#### macOS (Homebrew)

```bash
brew install raylib
```

#### Windows (MSYS2)

```bash
pacman -S mingw-w64-x86_64-raylib
```

Atau download pre-built binaries dari [raylib releases](https://github.com/raysan5/raylib/releases).

### Menjalankan Mode GUI

Setelah raylib terinstal, build dan jalankan:

```bash
cmake -S . -B cmake-build
cmake --build cmake-build
./cmake-build/game --gui
```

### Menjalankan Mode Console (tanpa GUI)

```bash
./cmake-build/game
```

Jika raylib tidak ditemukan saat build, program akan otomatis menggunakan mode console.
