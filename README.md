# Lab3 - Data Compression and Error Correction

## Overview
This project is a C-based implementation of fundamental data processing algorithms. It focuses on data compression using Huffman coding, error detection and correction using Hamming codes, and data structure management using Min-Heaps. The goal of this repository is to demonstrate practical applications of computer science concepts in low-level programming.

## Features
- **Huffman Coding**: An efficient algorithm for lossless data compression, which reduces file sizes by assigning variable-length codes to characters based on their frequencies.
- **Hamming Codes**: A method for error correction that can detect and correct single-bit errors, ensuring data integrity during transmission or storage.
- **Min-Heap Data Structure**: A specialized tree-based structure used to manage priorities, which is essential for building the Huffman tree efficiently.
- **Core Utilities**: Helper functions that support data manipulation and processing throughout the project.

## Technologies Used
- C Programming Language
- CMake for build configuration

## Project Structure
- `lab3tic.c`: The main entry point of the application.
- `huffman.h` / `huffmanops.h`: Implementation of the Huffman compression algorithm.
- `hamming.h` / `xhamming.h`: Implementation of Hamming codes for error correction.
- `minheap.h`: Min-Heap data structure logic.
- `utilidades.h`: Common utility and helper functions.

## How to Build
To compile this project, you will need a C compiler (such as GCC) and CMake installed on your system.

1. Clone the repository to your local machine.
2. Open a terminal in the project directory.
3. Generate the build files using CMake:
   ```
   cmake .
   ```
4. Build the executable:
   ```
   cmake --build .
   ```
5. Run the generated executable file.
