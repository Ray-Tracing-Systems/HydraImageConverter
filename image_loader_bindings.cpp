/*
image_loader_bindings.cpp (c) 2025
Desc: description
*/

#include <../pybind11/include/pybind11/pybind11.h>
#include <../pybind11/include/pybind11/stl.h>
#include "image_loader.h"

namespace py = pybind11;

PYBIND11_MODULE(image_loader, m) {
    py::class_<ImageFileInfo>(m, "ImageFileInfo")
        .def(py::init<>())
        .def_readwrite("is_ok", &ImageFileInfo::is_ok)
        .def_readwrite("is_normal_map", &ImageFileInfo::is_normal_map)
        .def_readwrite("width", &ImageFileInfo::width)
        .def_readwrite("height", &ImageFileInfo::height)
        .def_readwrite("channels", &ImageFileInfo::channels)
        .def_readwrite("bytes_per_channel", &ImageFileInfo::bytesPerChannel)
        .def_readwrite("path", &ImageFileInfo::path);

    py::enum_<TEX_FORMAT>(m, "TEX_FORMAT")
        .value("IMG_IMAGE4UB", TEX_FORMAT::IMG_IMAGE4UB)
        .value("IMG_IMAGE4F", TEX_FORMAT::IMG_IMAGE4F)
        .value("IMG_COMMON_LDR", TEX_FORMAT::IMG_COMMON_LDR)
        .value("IMG_COMMON_HDR", TEX_FORMAT::IMG_COMMON_HDR)
        .export_values();

    m.def("get_image_info", &getImageInfo, "Get image metadata",
          py::arg("filename"));
    
    m.def("load_image_ldr", &loadImageLDR, "Load LDR image data",
          py::arg("info"));
    
    m.def("load_image_hdr", &loadImageHDR, "Load HDR image data",
          py::arg("info"));
    
    m.def("save_image_ldr", &saveImageLDR, "Save LDR image",
          py::arg("filename"), py::arg("data"),
          py::arg("width"), py::arg("height"), py::arg("channels"));
    
    m.def("save_image_hdr", &saveImageHDR, "Save HDR image",
          py::arg("filename"), py::arg("data"),
          py::arg("width"), py::arg("height"), py::arg("channels"));
}