#include <sstream>

#include "gfx/Material.h"
#include "util/Utility.h"

namespace Gfx {

Material::Material(Ds::Vector<Texture>&& textures):
  mTextures(Util::Forward(textures))
{}

void Material::VisitTextures(
  std::function<void(const Texture&, const std::string&, int)> visit) const
{
  TextureType currentType = TextureType::Diffuse;
  int typeCount = 0;
  for (int i = 0; i < mTextures.Size(); ++i)
  {
    const Texture& texture = mTextures[i];
    if (currentType != texture.mTextureType)
    {
      currentType = texture.mTextureType;
      typeCount = 0;
    }
    std::stringstream uniformName;
    uniformName << "uMaterial.";
    switch (texture.mTextureType)
    {
    case TextureType::None: uniformName << "mNone"; break;
    case TextureType::Diffuse: uniformName << "mDiffuse"; break;
    case TextureType::Specular: uniformName << "mSpecular"; break;
    }
    uniformName << "[" << typeCount << "]";
    visit(texture, uniformName.str(), i);
    ++typeCount;
  }
}

} // namespace Gfx
