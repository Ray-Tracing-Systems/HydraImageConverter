cmake_minimum_required(VERSION 3.15)
project(HydraImageConverter)

# Set output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Configurable paths for dependencies
set(PYBIND11_PATH "../pybind11" CACHE PATH "Path to pybind11 directory")
set(STB_PATH "../stb" CACHE PATH "Path to stb directory")

# Flexible Python version (minimum 3.8)
find_package(Python 3.8 REQUIRED COMPONENTS Interpreter Development.Module)
if(NOT Python_FOUND)
    message(FATAL_ERROR "Python development packages not found. Please install Python 3.8+ with development headers")
endif()

# Optional OpenMP support
option(USE_OPENMP "Enable OpenMP parallelization" ON)
if(USE_OPENMP)
    find_package(OpenMP)
    if(OpenMP_FOUND)
        message(STATUS "OpenMP found - parallelization enabled")
    else()
        message(WARNING "OpenMP not found - building without parallelization")
    endif()
endif()

# Create image loader static library (for C++ builds)
add_library(image_loader_cpp STATIC
    image_loader.cpp
)

# Create main executable and link with C++ library
add_executable(main main.cpp)
target_link_libraries(main PRIVATE image_loader_cpp)

# Python bindings configuration (separate build)
if(BUILD_PYTHON_BINDINGS)
    # Add pybind11 subdirectory
    if(EXISTS ${PYBIND11_PATH}/CMakeLists.txt)
        add_subdirectory(${PYBIND11_PATH} pybind11)
    else()
        message(FATAL_ERROR "pybind11 not found at ${PYBIND11_PATH}")
    endif()

    # Create Python module (links with same source files but separate build)
    pybind11_add_module(image_loader
        image_loader_bindings.cpp
    )
    target_link_libraries(image_loader PRIVATE image_loader_cpp ${Python_LIBRARIES})
endif()

# Common settings for all targets
set(COMMON_COMPILE_OPTIONS
    -O3
    -Wall
    $<$<BOOL:${OpenMP_FOUND}>:${OpenMP_CXX_FLAGS}>
)

set(COMMON_INCLUDE_DIRS
    ${Python_INCLUDE_DIRS}
    ${STB_PATH}
)

# Apply common settings to all targets
get_property(all_targets DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
foreach(target IN LISTS all_targets)
    target_compile_options(${target} PRIVATE ${COMMON_COMPILE_OPTIONS})
    target_include_directories(${target} PRIVATE ${COMMON_INCLUDE_DIRS})
    if(OpenMP_FOUND)
        target_link_libraries(${target} PRIVATE OpenMP::OpenMP_CXX)
    endif()
endforeach()
