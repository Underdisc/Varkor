#ifndef gfx_Material_h
#define gfx_Material_h

#include <functional>
#include <string>

#include "ds/Vector.h"
#include "gfx/Image.h"

namespace Gfx {

struct Material
{
  enum class TextureType
  {
    None,
    Diffuse,
    Specular,
  };
  struct Texture
  {
    TextureType mTextureType;
    Image mImage;
  };

  Material(Ds::Vector<Texture>&& textures);
  void VisitTextures(
    std::function<void(const Texture&, const std::string&, int)> visit) const;

private:
  // The textures must be ordered by their type.
  Ds::Vector<Texture> mTextures;
};

} // namespace Gfx

#endif
