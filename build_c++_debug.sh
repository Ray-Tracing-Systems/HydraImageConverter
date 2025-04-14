#!/bin/bash
set -e

# Удаляем папку есть есть
rm -rf build_cpp/Debug
# Ensure build directory exists
mkdir -p build_cpp/Debug

# Run cmake and build
cd /mnt/CE1E4D571E4D39AB/GitRepo/HydraImageConverter
cmake -B build_cpp/Debug -DCMAKE_BUILD_TYPE=Debug .
cmake --build build_cpp/Debug --target main

# Verify executable exists
if [ ! -f "build_cpp/Debug/main" ]; then
    echo "Error: Executable not built!"
    exit 1
fi

echo "Build completed successfully"
