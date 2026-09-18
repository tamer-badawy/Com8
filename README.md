# 🚀 Com8 | A High-Performance C++ CHIP-8 Virtual Hardware Emulator

![C++](https://img.shields.io/badge/C++-17-blue?logo=cplusplus)
![Raylib](https://img.shields.io/badge/Raylib-6.0-black?logo=raylib)
[![Multi-Platform Build CI](https://github.com/tamer-badawy/Com8/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/tamer-badawy/Com8/actions/workflows/cmake-multi-platform.yml)

**Com8** is a clean, modular CHIP-8 interpreter engine built from scratch using **Modern C++** and **Raylib** for multimedia rendering. Inspired by the minimalism of classic MS-DOS `.COM` executable memory layouts and low-level `ORG 100h` systems programming architectures, this application emulates a virtual CPU, call stack tracking, memory arrays, and hardware registers entirely in software.

---

## 🕹️ System Architecture Map

```text
       [ Game ROM File ] ──> Loaded at Address 0x200 (512 Byte Offset)
                                     │
    ┌──────────────────────── Target Memory Map ────────────────────────┐
    │ 0x000 - 0x1FF: Hexadecimal Font Sprites Array (80 Bytes total)    │
    │ 0x200 - 0xFFF: Program Space Area (Execution Instructions Area)   │
    └───────────────────────────────────────────────────────────────────┘
                                     │
    ┌────────────────────────── Core Loop ──────────────────────────────┐
    │  1. FETCH   -> Compiles two 8-bit array slots into a 16-bit Opcode │
    │  2. DECODE  -> Extract Register Bitmasks (X, Y, NNN variables)    │
    │  3. EXECUTE -> Handle Bitwise Arithmetic, XOR Draw, and Branches  │
    └───────────────────────────────────────────────────────────────────┘
```

## ✨ Core Features & Highlights

- **Encapsulated Architecture:** The entire virtual console engine—including general purpose registers (V₀ through \(V_F\)), 4KB memory arrays, hardware delay/sound timers, and call stacks—is isolated inside a single modular object container.
- **Persistent Logic Seeds:** Optimized execution lifecycle by binding the random number generation engine (`std::mt19937`) directly to class instances instead of repetitive stack construction loops.
- **Pixel-Level XOR Collision Detonation:** Full implementations of the traditional `0xDXYN` drawing system, tracking exact alpha channel canvas bit-inversions to fire collision indicators.
- **Non-Blocking Input Listeners:** The keyboard pause execution opcode (`0xFX0A`) loops safely without blocking core operating system thread processes, ensuring seamless sound and canvas background state iterations.

---

## ⌨️ Controller Translation Configuration

The traditional 1970s hex grid is mapped intuitively to a modern 4x4 matrix block on your standard layout keyboard:

```text
Original CHIP-8 Keypad          Modern Keyboard Mapping
┌───┬───┬───┬───┐              ┌───┬───┬───┬───┐
│ 1 │ 2 │ 3 │ C │   ─── ───>   │ 1 │ 2 │ 3 │ 4 │
├───┼───┼───┼───┤              ├───┼───┼───┼───┤
│ 4 │ 5 │ 6 │ D │   ─── ───>   │ Q │ W │ E │ R │
├───┼───┼───┼───┤              ├───┼───┼───┼───┤
│ 7 │ 8 │ 9 │ E │   ─── ───>   │ A │ S │ D │ F │
├───┼───┼───┼───┤              ├───┼───┼───┼───┤
│ A │ 0 │ B │ F │   ─── ───>   │ Z │ X │ C │ V │
└───┴───┴───┴───┘              └───┴───┴───┴───┘
```

---

## 🛠️ Build & Installation Procedures

### Prerequisites

Ensure you have installed a standard C++ compiler and the **Raylib development binaries**:

- **Linux (Ubuntu/Debian):** `sudo apt install libraylib-dev`
- **macOS (Homebrew):** `brew install raylib`
- **Windows (vcpkg):** `vcpkg install raylib:x64-windows`

### Compiling via CMake

Run these standard compilation recipes inside your project directory terminal profile:

```bash
# 1. Generate local build trees
cmake -B build

# 2. Compile the binaries
cmake --build build

# 3. Launch target application
./build/Com8 path_to_rom.ch8
```
