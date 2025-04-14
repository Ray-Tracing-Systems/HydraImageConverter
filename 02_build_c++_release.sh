#!/bin/bash
set -e

# Удаляем папку есть есть
rm -rf build_cpp/Release
# Ensure build directory exists
mkdir -p build_cpp/Release

# Run cmake and build
cd /mnt/CE1E4D571E4D39AB/GitRepo/HydraImageConverter
cmake -B build_cpp/Release -DCMAKE_BUILD_TYPE=Release .
cmake --build build_cpp/Release --target main

# Verify executable exists
if [ ! -f "build_cpp/Release/main" ]; then
    echo "Error: Executable not built!"
    exit 1
fi

echo "Build completed successfully"
