#include "image_loader.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb/stb_image_write.h"


////////////////////////////////////////////////////////////////////////////

TEX_FORMAT getFormatFromExtension(const std::string &filename)
{
    auto found = filename.find_last_of('.');
    std::string ext = filename.substr(found, filename.size());

    if(ext == ".image4ub")
        return IMG_IMAGE4UB;
    else if(ext == ".image4f")
        return IMG_IMAGE4F;
    else if(ext == ".png")
        return IMG_PNG;
    else
    {
        std::cerr << "Error: Format " << ext << " not support" << std::endl;
        return IMG_OTHER;
    }
}

ImageFileInfo getImageInfo(const std::string& a_filename)
{
    ImageFileInfo res = {};
    res.path = a_filename;
    res.is_ok = false;
    res.channels = 4;

    const size_t fileSize = std::filesystem::file_size(a_filename);

    std::ifstream file(a_filename, std::ios::binary);
    if(!file.good())
    {
        file.close();
        return res;
    }

    auto tex_format = getFormatFromExtension(a_filename);

    if (tex_format == IMG_IMAGE4UB)
    {
        file.read((char*)&res.width,  sizeof(uint32_t));
        file.read((char*)&res.height, sizeof(uint32_t));
        
        res.bytesPerChannel = sizeof(unsigned char);
        res.channels = 4;
        
        if(res.width > 0 && res.height > 0)
            res.is_ok = true;
    }
    else if (tex_format == IMG_IMAGE4F)
    {
        // Чтение ширины и высоты
        file.read((char*)&res.width, sizeof(uint32_t));
        file.read((char*)&res.height, sizeof(uint32_t));

        // Расчёт ожидаемого размера для 3 и 4 каналов
        size_t expectedSize3 = res.width * res.height * 3 * sizeof(float) + 8;
        size_t expectedSize4 = res.width * res.height * 4 * sizeof(float) + 8;

        // Определение числа каналов
        if (fileSize == expectedSize3) 
            res.channels = 3;
        else if (fileSize == expectedSize4) 
            res.channels = 4;
        else {
            std::cerr << "Некорректный размер файла: " << a_filename << std::endl;
            return res;
        }

        res.bytesPerChannel = sizeof(float);
        res.is_ok = true;
        return res;
    }
    else if (tex_format == IMG_PNG)
    {
        stbi_info(a_filename.c_str(), &res.width, &res.height, &res.channels);
        res.bytesPerChannel = sizeof(unsigned char);

        if(res.width > 0 && res.height > 0 && res.channels > 0)
            res.is_ok = true;
    }
    else
    {
        std::cerr << "Error: Format " << a_filename << " not support" << std::endl;
        res.is_ok = false;
    }
    
    file.close();

    return res;
}

void flipImageVertically(std::vector<unsigned char>& data, int width, int height, int channels) {    
    const int rowSize = width * channels;    
    for (int y = 0; y < height/2; y++) {
        unsigned char* top = data.data() + y * rowSize;
        unsigned char* bottom = data.data() + (height - 1 - y) * rowSize;
        std::swap_ranges(top, top + rowSize, bottom);
    }
}

void flipImageVertically(std::vector<float>& data, int width, int height, int channels) {
    const int rowSize = width * channels;
    for (int y = 0; y < height/2; y++) {
        float* top = data.data() + y * rowSize;
        float* bottom = data.data() + (height - 1 - y) * rowSize;
        std::swap_ranges(top, top + rowSize, bottom);
    }
}

std::vector<unsigned char> loadImage4ub(const std::string &filename)
{
    std::ifstream infile(filename, std::ios::binary);
    std::vector<unsigned char> result;

    if (infile.good())
    {
        uint32_t w, h;
        infile.read((char*)&w, sizeof(uint32_t));
        infile.read((char*)&h, sizeof(uint32_t));

        if (!infile) {
            std::cerr << "Ошибка чтения данных из файла!" << std::endl;
            return result;
        }

        result.resize(w * h * 4);
        infile.read((char*)result.data(), w * h * 4);        
        flipImageVertically(result, w, h, 4);
    }
    return result;
}

std::vector<float> loadImage4f(const std::string &filename, int& outChannels) {
    std::ifstream infile(filename, std::ios::binary);
    std::vector<float> result;

    if (infile.good()) 
    {
        uint32_t w, h;
        infile.read((char*)&w, sizeof(uint32_t));
        infile.read((char*)&h, sizeof(uint32_t));

        result.resize(w * h * outChannels);
        infile.read((char*)result.data(), w * h * outChannels * sizeof(float));

        if (!infile) {
            std::cerr << "Ошибка чтения данных из файла!" << std::endl;
            return result;
        }
        flipImageVertically(result, w, h, outChannels);
    }
    return result;
}


std::vector<unsigned char> loadImageLDR(const ImageFileInfo& info)
{
    int w, h, channels, req_channels;
    if(info.channels == 3)
        req_channels = 4;
    else
        req_channels = info.channels;

    unsigned char *pixels = stbi_load(info.path.c_str(), &w, &h, &channels, req_channels);
    if (!pixels) {
        std::cerr << "Error: Failed to load image data for " << info.path << std::endl;
        return std::vector<unsigned char>(); // Return empty vector
    }

    std::vector<unsigned char> result(w * h * req_channels);
    memcpy(result.data(), pixels, result.size());
    stbi_image_free(pixels);        
    return result;    
}



bool saveImageLDR(const std::string& a_filename, const std::vector<unsigned char> &a_data,
  int width, int height, int channels)
{
    return stbi_write_png(a_filename.c_str(), width, height, channels, a_data.data(), width * channels);
}

