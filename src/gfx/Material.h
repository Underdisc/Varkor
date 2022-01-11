#ifndef gfx_Material_h
#define gfx_Material_h

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
  struct TextureGroup
  {
    TextureType mType;
    Ds::Vector<Image> mImages;
  };
  Ds::Vector<TextureGroup> mGroups;
};

} // namespace Gfx

#endif
