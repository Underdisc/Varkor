#ifndef gfx_Texture_h
#define gfx_Texture_h

#include <string>

namespace Gfx {

enum class TextureType
{
  None,
  Diffuse,
  Specular,
};

class Texture
{
public:
  Texture(const char* textureFile, TextureType type = TextureType::None);
  ~Texture();
  void Purge();
  int Width() const;
  int Height() const;
  int Channels() const;
  unsigned int Id() const;
  TextureType Type() const;

  // todo: This value is only included here to act as a way to know if a texture
  // has been loaded or not. The only code that makes use of this is the Model
  // loading code. This value should be removed once there is a more concrete
  // way to handle the loading of assets without duplicates.
  std::string mFile;

private:
  int mWidth;
  int mHeight;
  int mChannels;
  unsigned int mId;
  TextureType mType;
};

} // namespace Gfx

#endif