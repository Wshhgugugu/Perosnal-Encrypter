# Personal Encrypter

A C++ based encryption tool designed to secure personal files using linear algebra and password-derived chaos.

![Project Status](https://img.shields.io/badge/Status-Work_in_Progress-yellow) ![Language](https://img.shields.io/badge/Language-C++-blue) ![Platform](https://img.shields.io/badge/Platform-Windows%20|%20Linux%20|%20macOS-lightgrey)

## 📖 Introduction

**Personal Encrypter** is not just another XOR tool. It aims to implement a robust encryption mechanism where the user's password generates a unique transformation matrix. By utilizing the **Eigen** linear algebra library, the system creates invertible matrices seeded by the user's input, ensuring that the encryption key is mathematically tied to the password.

> **Note**: This project is currently in the **Alpha** stage. Core matrix generation logic is implemented, while the final file stream transformation algorithms are under active development.

## 🛠 Architecture & Design

### The Core Concept
The encryption process is designed around the following steps:
1.  **Seed Generation**: The user's password is hashed (using `std::hash`) to create a deterministic seed.
2.  **Matrix Construction**: A pseudo-random number generator (`std::mt19937`) uses this seed to construct a square matrix.
3.  **Invertibility Guarantee**: An identity matrix is added to the random matrix to ensure it remains invertible (a requirement for decryption).
4.  **Transformation**: The file data is treated as a vector and multiplied by this matrix to produce the encrypted output.

### Technical Highlights
* **Eigen Library Integration**: Uses high-performance matrix operations.
* **Cross-Platform Pathing**: Automatically detects Desktop paths on Windows (via `shlobj.h`) and Linux/macOS (via `HOME` env).
* **Stream Processing**: Handles file I/O using C++ standard streams for efficiency.

## 📂 Project Structure

```text
Perosnal-Encrypter/
├── Encryption 1/
│   ├── Encryption 1.cpp    # Main implementation logic
│   ├── Encryption 1.sln    # Visual Studio Solution file
│   └── ...                 # Project build files
├── main.cpp                # Alternate entry point (Prototype)
└── README.md               # Documentation

## Dependencies

This project uses the [Eigen](https://gitlab.com/libeigen/eigen) C++ library (version 5.0.0) for linear algebra.
Make sure to install Eigen 5.0.0 and add it to your compiler's include path.
