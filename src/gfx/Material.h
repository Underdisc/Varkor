#ifndef gfx_Material_h
#define gfx_Material_h

#include <assimp/material.h>
#include <string>

#include "gfx/UniformVector.h"
#include "rsl/Asset.h"
#include "rsl/ResourceId.h"
#include "vlk/Valkor.h"

namespace Gfx {

struct Material
{
  Material();
  Material(Material&& other);
  Material& operator=(Material&& other);

  static void EditConfig(Vlk::Value* configValP);
  Result Init(const Vlk::Explorer& configEx);
  Result Init(const ResId& shaderResId);
  Result Init(Material&& other);
  static VResult<Material> Init(
    Rsl::Asset& asset,
    const ResId& shaderResId,
    const std::string& materialName,
    const aiMaterial& assimpMat,
    const std::string& directory);

  ResId mShaderId;
  UniformVector mUniforms;

private:
  static const char* GetUniformName(aiTextureType aiType);
};

} // namespace Gfx

#endif
