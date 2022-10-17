#ifndef gfx_Image_h
#define gfx_Image_h

#include <glad/glad.h>

#include "vlk/Valkor.h"

namespace Gfx {

struct Image
{
public:
  Image();
  Image(Image&& other);
  Image& operator=(Image&& other);
  ~Image();

  struct Config
  {
    unsigned char* mData;
    int mWidth;
    int mHeight;
    GLenum mInternalFormat;
    GLenum mFormat;
    GLint mPixelAlignment;
  };
  static void EditConfig(Vlk::Value* configValP);
  Result Init(const Vlk::Explorer& configEx);
  Result Init(const std::string& file);
  void Init(const Config& config);

  GLuint Id() const;
  float Aspect() const;

private:
  int mWidth;
  int mHeight;
  GLuint mId;
};

} // namespace Gfx

#endif
