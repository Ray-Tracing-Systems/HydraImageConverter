#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <cerrno>
#include "image_loader.h"

TEX_FORMAT getFormatFromExtension(const std::string &filename);

namespace fs = std::filesystem;

inline static float clamp(float u, float a, float b) { return std::min(std::max(a, u), b); }

bool hasValidExtension(const fs::path& filePath) {
    const std::string ext = filePath.extension().string();
    return ext == ".image4ub" || ext == ".image4f";
}

std::string findReferenceImage(const std::string& imagePath) {
    fs::path inputPath(imagePath);
    fs::path refPath = inputPath.parent_path() / (inputPath.stem().string() + "_ref.png");
    
    if (!fs::exists(refPath))
    {
        std::cerr << "Reference " << refPath << " not found.\n";        
        return "";
    }
    else 
        return refPath.string();  
    
}

bool compareImages(const std::vector<unsigned char>& img1, 
                  const std::vector<unsigned char>& img2) {
    if (img1.size() != img2.size()) {
        std::cerr << "Image size mismatch: " << img1.size() << " vs " << img2.size() << std::endl;
        return false;
    }
    return std::equal(img1.begin(), img1.end(), img2.begin());
}


void processImage(const std::string& inputPath) {
    std::cerr << "\nProcessing: " << inputPath << std::endl;
    fs::path outputPath = inputPath;
    outputPath.replace_extension(".png");

    auto info = getImageInfo(inputPath);
    if (!info.is_ok) {
        std::cerr << "Error: Failed to get image info for " << inputPath << std::endl;
        return;
    }

    std::cerr << "Dimensions: " << info.width << "x" << info.height 
              << " Channels: " << info.channels << std::endl;

    std::vector<unsigned char> image_ldr;
    std::vector<float> hdrImage;
    
    auto format = getFormatFromExtension(inputPath);

    if (format == IMG_IMAGE4UB)
    {
        image_ldr = loadImage4ub(info.path);
        if (image_ldr.empty()) {
            std::cerr << "Error: Failed to load image data" << std::endl;
            return;
        }
    }
    else if (format == IMG_IMAGE4F)
    {
        hdrImage = loadImage4f(info.path, info.channels);
        if (hdrImage.empty()) {
            std::cerr << "Error: Failed to load HDR image data" << std::endl;
            return;
        }

        image_ldr.resize(info.width * info.height * 4);

        for (size_t srcIdx = 0, dstIdx = 0; srcIdx < hdrImage.size(); srcIdx += info.channels, dstIdx += 4) 
        {
            // RGB → RGBA
            const float r = clamp(hdrImage[srcIdx], 0.0f, 1.0f);
            const float g = clamp(hdrImage[srcIdx + 1], 0.0f, 1.0f);
            const float b = clamp(hdrImage[srcIdx + 2], 0.0f, 1.0f);

            image_ldr[dstIdx]   = static_cast<unsigned char>(r * 255.0f); // R
            image_ldr[dstIdx+1] = static_cast<unsigned char>(g * 255.0f); // G
            image_ldr[dstIdx+2] = static_cast<unsigned char>(b * 255.0f); // B
            image_ldr[dstIdx+3] = 255; // Альфа    
            info.channels = 4;           
        }   
    }

    if (!saveImageLDR(outputPath.string(), image_ldr, info.width, info.height, info.channels)) {
        std::cerr << "Error: Failed to save output image" << std::endl;
        return;
    }

    // Comparative tests

    std::string refPath = findReferenceImage(inputPath);
    
    if (!refPath.empty()) 
    {
        std::cout << "Found reference image: " << refPath << std::endl;
        auto refInfo = getImageInfo(refPath);
        if (refInfo.is_ok)
        {
            auto refImage = loadImageLDR(refInfo);
            if (!refImage.empty()) 
                std::cout << (compareImages(image_ldr, refImage) ? "match: ok" : "match: FAILED") << std::endl;            
        }
    } 
    else 
        std::cout << "No reference image found for: " << inputPath << std::endl;
}




int main() {        
    const std::string textureDir = "Textures/";
    std::cerr << "Processing directory: " << fs::absolute(textureDir) << std::endl;
    for (const auto& entry : fs::directory_iterator(textureDir)) {
        if (entry.is_regular_file() && hasValidExtension(entry.path())) {
            processImage(entry.path().string());
        }
    }
    return 0;
}
