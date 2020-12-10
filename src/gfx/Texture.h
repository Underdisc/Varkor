#ifndef gfx_Texture_h
#define gfx_Texture_h

namespace Gfx {

class Texture
{
public:
  Texture(const char* textureFile);
  ~Texture();
  void Purge();
  int Width() const;
  int Height() const;
  int Channels() const;
  unsigned int Id() const;

private:
  int mWidth;
  int mHeight;
  int mChannels;
  unsigned int mId;
};

} // namespace Gfx

#endif