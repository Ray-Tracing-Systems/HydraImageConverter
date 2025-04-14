#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <cerrno>
#include "image_loader.h"

TEX_FORMAT guessFormatFromExtension(const std::string &filename);

namespace fs = std::filesystem;

bool hasValidExtension(const fs::path& filePath) {
    const std::string ext = filePath.extension().string();
    return ext == ".image4ub" || ext == ".image4f" || ext == ".exr" || ext == ".hdr";
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
    std::cerr << "Processing: " << inputPath << std::endl;
    fs::path outputPath = inputPath;
    outputPath.replace_extension(".png");

    auto info = getImageInfo(inputPath);
    if (!info.is_ok) {
        std::cerr << "Error: Failed to get image info for " << inputPath << std::endl;
        return;
    }

    std::cerr << "Dimensions: " << info.width << "x" << info.height 
              << " Channels: " << info.channels << std::endl;

    std::vector<unsigned char> image;
    std::vector<float> hdrImage;
    
    if (guessFormatFromExtension(inputPath) == IMG_IMAGE4F || guessFormatFromExtension(inputPath) == IMG_COMMON_HDR) {
        hdrImage = loadImageHDR(info);
        if (hdrImage.empty()) {
            std::cerr << "Error: Failed to load HDR image data" << std::endl;
            return;
        }
        // Convert HDR to LDR for PNG output
        image.resize(hdrImage.size());
        for (size_t i = 0; i < hdrImage.size(); i++) {
            image[i] = static_cast<unsigned char>(std::min(255.0f, std::max(0.0f, hdrImage[i] * 255.0f)));
        }
    } else {
        image = loadImageLDR(info, true);
        if (image.empty()) {
            std::cerr << "Error: Failed to load image data" << std::endl;
            return;
        }
    }

    if (!saveImageLDR(outputPath.string(), image, info.width, info.height, info.channels)) {
        std::cerr << "Error: Failed to save output image" << std::endl;
        return;
    }

    std::string refPath = findReferenceImage(inputPath);
    
    if (!refPath.empty()) {
        std::cout << "Found reference image: " << refPath << std::endl;
        auto refInfo = getImageInfo(refPath);
        if (refInfo.is_ok) {
            auto refImage = loadImageLDR(refInfo);
            if (!refImage.empty()) {
                std::cout << (compareImages(image, refImage) 
                    ? "match: ok" 
                    : "match: FAILED") << std::endl;
            }
        }
    } else {
        std::cout << "No reference image found for: " << inputPath << std::endl;
    }
    std::cout << std::endl;
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
