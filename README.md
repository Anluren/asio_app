# Asio LevelDB Socket Server

This project is a C++ socket server using Asio for networking and LevelDB for persistent storage.

## Build Instructions

1. Install dependencies:
   - Asio (header-only, or via your package manager)
   - LevelDB (`sudo apt install libleveldb-dev`)
   - CMake (`sudo apt install cmake`)
2. Build:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```
3. Run:
   ```bash
   ./asio_app
   ```

## Features
- Accepts commands and data from clients over TCP
- Stores received data in LevelDB

## TODO
- Implement full server logic
