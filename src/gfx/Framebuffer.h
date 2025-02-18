#ifndef gfx_Framebuffer_h
#define gfx_Framebuffer_h

#include <glad/glad.h>

namespace Gfx {

class Framebuffer {
public:
  struct Options {
    int mWidth;
    int mHeight;
    GLint mInternalFormat;
    GLenum mFormat;
    GLenum mPixelType;
    bool mMultisample;
  };

  Framebuffer();
  Framebuffer(const Options& options);
  ~Framebuffer();
  void Init(const Options& options);
  void Resize(int width, int height);
  GLuint Fbo() const;
  GLuint ColorTbo() const;
  GLenum Format() const;
  GLenum PixelType() const;

private:
  void Purge();
  GLuint mFbo;
  GLuint mColorTbo;
  GLuint mDepthTbo;
  Options mOptions;
};

} // namespace Gfx

#endif
