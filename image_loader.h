#include <string>
#include <cstdint>
#include <vector>
enum TEX_FORMAT
{
  IMG_IMAGE4UB,
  IMG_IMAGE4F,
  IMG_COMMON_LDR,
  IMG_COMMON_HDR
};

struct ImageFileInfo
{
  bool is_ok = false;
  bool is_normal_map = false;
  int width = 0u;
  int height = 0u;
  int channels = 0u;
  int bytesPerChannel = 0u;
  std::string path;
};

ImageFileInfo getImageInfo(const std::string& a_filename);
std::vector<unsigned char> loadImageLDR(const ImageFileInfo& info, bool flipVertical = false);
std::vector<float> loadImage4f(const std::string &filename);
std::vector<float> loadImageHDR(const ImageFileInfo& info);

bool saveImageLDR(const std::string& a_filename, const std::vector<unsigned char> &a_data,
  int width, int height, int channels);

bool saveImageHDR(const std::string& a_filename, const std::vector<float> &a_data,
  int width, int height, int channels);

void flipImageVertically(std::vector<unsigned char>& data, int width, int height, int channels);
std::vector<unsigned char> loadImage4ub(const std::string &filename, bool flipVertical = false);
