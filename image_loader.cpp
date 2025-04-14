#include "image_loader.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb/stb_image_write.h"

const int MAX_IMAGE_DIMENSION = 10000; // Set a reasonable limit for image dimensions

////////////////////////////////////////////////////////////////////////////

TEX_FORMAT guessFormatFromExtension(const std::string &filename)
{
    auto found = filename.find_last_of('.');
    if(found == std::string::npos)
    {
        std::cerr << "Image file with no extension: " << filename << " Assuming LDR format\n";
        return IMG_COMMON_LDR;
    }

    std::string ext = filename.substr(found, filename.size());

    if(ext == ".image4ub")
        return IMG_IMAGE4UB;
    else if(ext == ".image4f")
        return IMG_IMAGE4F;
    else if(ext == ".exr" || ext == ".hdr")
        return IMG_COMMON_HDR;
    else
        return IMG_COMMON_LDR;
}

ImageFileInfo getImageInfo(const std::string& a_filename)
{
    ImageFileInfo res = {};
    res.path = a_filename;
    res.is_ok = false;

    std::ifstream file(a_filename, std::ios::binary);
    if(!file.good())
    {
        file.close();
        return res;
    }

    auto tex_format = guessFormatFromExtension(a_filename);

    if (tex_format == IMG_IMAGE4UB || tex_format == IMG_IMAGE4F)
    {
        file.read((char*)&res.width,  sizeof(uint32_t));
        file.read((char*)&res.height, sizeof(uint32_t));
        res.channels = 4;

        if(tex_format == IMG_IMAGE4UB)
            res.bytesPerChannel = sizeof(unsigned char);
        else
            res.bytesPerChannel = sizeof(float);

        if(res.width >= 0 && res.height >= 0)
            res.is_ok = true;
    }
    else if (tex_format == IMG_COMMON_LDR)
    {
        stbi_info(a_filename.c_str(), &res.width, &res.height, &res.channels);
        res.bytesPerChannel = sizeof(unsigned char);

        if(res.width > 0 && res.height > 0 && res.channels > 0)
            res.is_ok = true;
    }
    else if (tex_format == IMG_COMMON_HDR && stbi_is_hdr(a_filename.c_str()))
    {
        //@TODO: test this
        stbi_info(a_filename.c_str(), &res.width, &res.height, &res.channels);
        res.bytesPerChannel = sizeof(float);

        if(res.width > 0 && res.height > 0 && res.channels > 0)
            res.is_ok = true;
    }
    file.close();

    return res;
}

void flipImageVertically(std::vector<unsigned char>& data, int width, int height, int channels) {
    if (data.empty() || width <= 0 || height <= 0) return;
    
    const int rowSize = width * channels;
    std::vector<unsigned char> tempRow(rowSize);
    
    for (int y = 0; y < height/2; y++) {
        unsigned char* top = data.data() + y * rowSize;
        unsigned char* bottom = data.data() + (height - 1 - y) * rowSize;
        std::swap_ranges(top, top + rowSize, bottom);
    }
}

void flipImageVertically(std::vector<float>& data, int width, int height, int channels) {
    if (data.empty() || width <= 0 || height <= 0) return;
    
    const int rowSize = width * channels;
    std::vector<float> tempRow(rowSize);
    
    for (int y = 0; y < height/2; y++) {
        float* top = data.data() + y * rowSize;
        float* bottom = data.data() + (height - 1 - y) * rowSize;
        std::swap_ranges(top, top + rowSize, bottom);
    }
}

std::vector<unsigned char> loadImage4ub(const std::string &filename, bool flipVertical)
{
    std::ifstream infile(filename, std::ios::binary);
    std::vector<unsigned char> result;
    if (infile.good())
    {
        int32_t w, h;
        infile.read((char*)&w, sizeof(int));
        infile.read((char*)&h, sizeof(int));

        if (w > MAX_IMAGE_DIMENSION || h > MAX_IMAGE_DIMENSION) {
            std::cerr << "Error: Image dimensions exceed maximum allowed size for " << filename << std::endl;
            return result; // Return empty vector
        }

        result.resize(w * h * 4);
        infile.read((char*)result.data(), w * h * 4);
        
        if (flipVertical) {
            flipImageVertically(result, w, h, 4);
        }
    }
    return result;
}

std::vector<unsigned char> loadImageLDR(const ImageFileInfo& info, bool flipVertical)
{
    auto tex_format = guessFormatFromExtension(info.path);
    if (tex_format == IMG_IMAGE4UB)
    {
        return loadImage4ub(info.path, flipVertical);
    }
    else
    {
        int w, h, channels, req_channels;
        if(info.channels == 3)
            req_channels = 4;
        else
            req_channels = info.channels;

        if (info.width > MAX_IMAGE_DIMENSION || info.height > MAX_IMAGE_DIMENSION) {
            std::cerr << "Error: Image dimensions exceed maximum allowed size for " << info.path << std::endl;
            return std::vector<unsigned char>(); // Return empty vector
        }

        unsigned char *pixels = stbi_load(info.path.c_str(), &w, &h, &channels, req_channels);
        if (!pixels) {
            std::cerr << "Error: Failed to load image data for " << info.path << std::endl;
            return std::vector<unsigned char>(); // Return empty vector
        }

        std::vector<unsigned char> result(w * h * req_channels);
        memcpy(result.data(), pixels, result.size());

        stbi_image_free(pixels);

        if (flipVertical) {
            flipImageVertically(result, w, h, req_channels);
        }
        return result;
    }
}

std::vector<float> loadImage4f(const std::string &filename)
{
    std::ifstream infile(filename, std::ios::binary);
    std::vector<float> result;
    if (infile.good())
    {
        int32_t w, h;
        infile.read((char*)&w, sizeof(int));
        infile.read((char*)&h, sizeof(int));

        if (w > MAX_IMAGE_DIMENSION || h > MAX_IMAGE_DIMENSION) {
            std::cerr << "Error: Image dimensions exceed maximum allowed size for " << filename << std::endl;
            return result;
        }

        result.resize(w * h * 4);
        infile.read((char*)result.data(), w * h * 4 * sizeof(float));
        
        // Flip image vertically to match other formats
        flipImageVertically(result, w, h, 4);
    }
    return result;
}

std::vector<float> loadImageHDR(const ImageFileInfo& info)
{
    int w, h, channels, req_channels;
    if(info.channels == 3)
        req_channels = 4;
    else
        req_channels = info.channels;

    if (guessFormatFromExtension(info.path) == IMG_IMAGE4F) {
        return loadImage4f(info.path);
    }
    
    float* pixels = stbi_loadf(info.path.c_str(), &w, &h, &channels, req_channels);
    if (!pixels) {
        std::cerr << "Error: Failed to load HDR image data for " << info.path << std::endl;
        return std::vector<float>(); // Return empty vector
    }

    std::vector<float> result(w * h * req_channels);
    memcpy(result.data(), pixels, result.size());

    stbi_image_free(pixels);

    return result;
}

bool saveImageLDR(const std::string& a_filename, const std::vector<unsigned char> &a_data,
  int width, int height, int channels)
{
    return stbi_write_png(a_filename.c_str(), width, height, channels, a_data.data(), width * channels);
}

bool saveImageHDR(const std::string& a_filename, const std::vector<float> &a_data,
  int width, int height, int channels)
{
    return stbi_write_hdr(a_filename.c_str(), width, height, channels, a_data.data());
}
