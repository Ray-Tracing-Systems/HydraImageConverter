#include <string>
#include <cstdint>
#include <vector>

enum TEX_FORMAT {IMG_IMAGE4UB, IMG_IMAGE4F, IMG_PNG, IMG_OTHER};

struct ImageFileInfo
{
  bool is_ok = false;
  int width = 0u;
  int height = 0u;
  int channels = 0u;
  int bytesPerChannel = 0u;
  std::string path;
};

float clamp(float u, float a, float b);

ImageFileInfo getImageInfo(const std::string& a_filename);
std::vector<unsigned char> loadImageLDR(const ImageFileInfo& info);
std::vector<unsigned char> loadImage4ub(const std::string &filename);
std::vector<unsigned char> image4fToUchar(const std::string &filename, int& outChannels);
bool saveImageLDR(const std::string& a_filename, const std::vector<unsigned char> &a_data,
  int width, int height, int channels);

void flipImageVertically(std::vector<unsigned char>& data, int width, int height, int channels);
