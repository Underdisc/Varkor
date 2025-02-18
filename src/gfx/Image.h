#ifndef gfx_Image_h
#define gfx_Image_h

#include <glad/glad.h>

#include "vlk/Valkor.h"

namespace Gfx {

struct Image {
public:
  Image();
  Image(Image&& other);
  Image& operator=(Image&& other);
  ~Image();

  static void EditConfig(Vlk::Value* configValP);
  Result Init(const Vlk::Explorer& configEx);
  Result Init(const std::string& file);
  Result InitDDS(FILE* fileStream);
  Result Init(const void* fileData, int size);
  Result Init(const void* imageData, int width, int height, int channels);
  Result Init(
    const void* imageData,
    int width,
    int height,
    GLenum internalFormat,
    GLenum format,
    GLint pixelAlignment);
  void CreateTexutre();

  GLuint Id() const;
  float Aspect() const;

private:
  int mWidth;
  int mHeight;
  GLuint mId;
};

} // namespace Gfx

#endif
