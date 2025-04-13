#!/bin/bash

BUILD_DIR="build"
PYTHON_EXECUTABLE=$(which python3.10)
PYTHON_INCLUDE_DIR=$(python3.10 -c "import sysconfig; print(sysconfig.get_path('include'))")
PYTHON_LIBRARY=$(python3.10 -c "import sysconfig; print(sysconfig.get_config_var('LIBDIR'))")

rm -rf ${BUILD_DIR}
mkdir -p ${BUILD_DIR}

cmake -B ${BUILD_DIR} \
    -DPython_EXECUTABLE=${PYTHON_EXECUTABLE} \
    -DPython_INCLUDE_DIR=${PYTHON_INCLUDE_DIR} \
    -DPython_LIBRARY=${PYTHON_LIBRARY} \
    -DCMAKE_BUILD_TYPE=Release

cmake --build ${BUILD_DIR} -- -j$(nproc)