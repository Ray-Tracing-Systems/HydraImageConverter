#!/bin/bash

set -euo pipefail

BUILD_DIR="build_python"
PYTHON_MIN_VERSION="3.8"

# Find available Python
PYTHON_EXECUTABLE=$(python3 -c "import sys; assert sys.version_info >= (3,8); print(sys.executable)" || \
                   python -c "import sys; assert sys.version_info >= (3,8); print(sys.executable)" || \
                   (echo "ERROR: Python ${PYTHON_MIN_VERSION}+ required" >&2; exit 1))

# Get Python paths
PYTHON_INCLUDE_DIR=$(${PYTHON_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_path('include'))")
PYTHON_LIBRARY=$(${PYTHON_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_config_var('LIBDIR'))")

# Clean and create build directory
rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"

# Configure with CMake
echo "Configuring build with Python ${PYTHON_EXECUTABLE}"
cmake -B "${BUILD_DIR}" \
    -DPython_EXECUTABLE="${PYTHON_EXECUTABLE}" \
    -DPython_INCLUDE_DIR="${PYTHON_INCLUDE_DIR}" \
    -DPython_LIBRARY="${PYTHON_LIBRARY}" \
    -DCMAKE_BUILD_TYPE=Release

# Build
echo "Starting build..."
if cmake --build "${BUILD_DIR}" -- -j$(nproc); then
    echo "Build successful!"
    
    mkdir hydra_image_loader
    cd hydra_image_loader
    touch __init__.py
    
    cd ../"${BUILD_DIR}"
    ls -l image_loader.*.so  
    cp image_loader.*.so ../hydra_image_loader/
else
    echo "Build failed!" >&2
    exit 1
fi
