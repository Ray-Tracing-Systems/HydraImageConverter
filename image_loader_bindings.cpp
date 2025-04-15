/*
image_loader_bindings.cpp (c) 2025
Desc: Биндинг с++ методов к питону
*/

#include <../pybind11/include/pybind11/pybind11.h>
#include <../pybind11/include/pybind11/stl.h>

#include "image_loader.h" // Убедитесь, что путь корректен

namespace py = pybind11;

// Объявление функций из image_loader.h
extern ImageFileInfo getImageInfo(const std::string& path);
extern std::vector<unsigned char> loadImage4ub(const std::string &filename);
extern std::vector<unsigned char> image4fToUchar(const std::string &filename, int& outChannels);
extern bool saveImageLDR(const std::string& path, const std::vector<uint8_t>& data, int width, int height, int channels);

PYBIND11_MODULE(image_loader, m) {
    m.doc() = "HydraImageConverter Python Bindings";

    // Регистрация структуры ImageFileInfo
    py::class_<ImageFileInfo>(m, "ImageFileInfo")
        .def_readonly("width", &ImageFileInfo::width)
        .def_readonly("height", &ImageFileInfo::height)
        .def_readonly("channels", &ImageFileInfo::channels)
        .def_readonly("path", &ImageFileInfo::path)
        .def_readonly("is_ok", &ImageFileInfo::is_ok);
      

    // Экспорт функций
    m.def("getImageInfo", &getImageInfo, "Get image metadata");
    m.def("loadImage4ub", &loadImage4ub, "Load 8-bit image");
    m.def("image4fToUchar", &image4fToUchar, "Load image4f and convert to 8-bit");
    m.def("saveImageLDR", &saveImageLDR, "Save LDR image");
}
