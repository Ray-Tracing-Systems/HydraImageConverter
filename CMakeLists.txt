cmake_minimum_required(VERSION 3.15)
project(HydraImageConverter)

# Явное указание версии Python
find_package(Python 3.10 EXACT REQUIRED COMPONENTS Interpreter Development.Module)

# Настройка OpenMP
find_package(OpenMP REQUIRED)

# Пути к зависимостям
add_subdirectory(../pybind11 pybind11)

# Создание Python модуля
pybind11_add_module(image_loader
    image_loader.cpp
    image_loader_bindings.cpp
)

# Настройки компиляции
target_compile_options(image_loader PRIVATE
    -O3
    -Wall
    ${OpenMP_CXX_FLAGS}
)

target_link_libraries(image_loader PRIVATE
    ${OpenMP_CXX_LIBRARIES}
    ${Python_LIBRARIES}
)

# Подключение заголовков
target_include_directories(image_loader PRIVATE
    ${Python_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../stb
)