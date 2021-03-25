#ifndef gfx_Framebuffer_h
#define gfx_Framebuffer_h

#include <glad/glad.h>

namespace Gfx {

class Framebuffer
{
public:
  Framebuffer(int width, int height);
  ~Framebuffer();
  unsigned int Fbo();
  GLenum Format();
  GLenum PixelType();

private:
  unsigned int mFbo;
  unsigned int mColorTbo;
  unsigned int mDepthTbo;
  GLenum mFormat;
  GLenum mPixelType;
};

} // namespace Gfx

#endif
