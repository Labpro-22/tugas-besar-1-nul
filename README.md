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
clang-format -i src/**/*.cpp include/**/*.hpp
```

Jika shell tidak mendukung glob `**`, gunakan pendekatan `find`:
