/*
image_loader_bindings.cpp (c) 2025
Desc: Биндинг с++ методов к питону
*/

#include <../pybind11/include/pybind11/pybind11.h>
#include <../pybind11/include/pybind11/stl.h>

#include "image_loader.h" // Убедитесь, что путь корректен

namespace py = pybind11;

// Объявление функций из image_loader.h
extern ImageInfo get_image_info(const std::string& path);
extern std::vector<uint8_t> loadImage4ub(const std::string& path);
extern std::vector<float> loadImage4f(const std::string& path, int channels);
extern void save_image_ldr(const std::string& path, const std::vector<uint8_t>& data, int width, int height, int channels);

PYBIND11_MODULE(image_loader, m) {
    m.doc() = "HydraImageConverter Python Bindings";

    // Регистрация структуры ImageInfo
    py::class_<ImageInfo>(m, "ImageInfo")
        .def_readonly("width", &ImageInfo::width)
        .def_readonly("height", &ImageInfo::height)
        .def_readonly("channels", &ImageInfo::channels)
        .def_readonly("path", &ImageInfo::path);

    // Экспорт функций
    m.def("get_image_info", &get_image_info, "Get image metadata");
    m.def("loadImage4ub", &loadImage4ub, "Load 8-bit image");
    m.def("loadImage4f", &loadImage4f, "Load float image");
    m.def("save_image_ldr", &save_image_ldr, "Save LDR image");
}