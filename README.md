[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/b842RB8g)

# Tugas Besar 1 IF2010 Pemrograman Berorientasi Objek

Nimonspoliiiiiii

(clone, checkout dev, and branch from dev yes)

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
