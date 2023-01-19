#include <iostream>
#include "image_loader.h"



int main()
{
  auto info = getImageInfo("Textures/chunk_00002.image4ub");

  std::cout << "Width: "    << info.width     << std::endl;
  std::cout << "Height: "   << info.height    << std::endl;
  std::cout << "Channels: " << info.channels  << std::endl;;

  auto image = loadImageLDR(info);
  saveImageLDR("Textures/chunk_00002.png", image, info.width, info.height, info.channels);
}

