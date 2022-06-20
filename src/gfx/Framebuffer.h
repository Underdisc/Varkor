#ifndef gfx_Framebuffer_h
#define gfx_Framebuffer_h

#include "ds/Vector.h"

namespace Gfx {

class Framebuffer
{
public:
  Framebuffer(
    int width, int height, unsigned int format, unsigned int pixelType);
  ~Framebuffer();
  void Resize(int width, int height);
  unsigned int Fbo() const;
  unsigned int ColorTbo() const;
  unsigned int Format() const;
  unsigned int PixelType() const;

private:
  void Init(int width, int height);
  void Purge();
  unsigned int mFbo;
  unsigned int mColorTbo;
  unsigned int mDepthTbo;
  unsigned int mFormat;
  unsigned int mPixelType;
};

} // namespace Gfx

#endif
