#ifndef gfx_Material_h
#define gfx_Material_h

#include <assimp/material.h>
#include <string>

#include "gfx/UniformVector.h"
#include "rsl/Asset.h"
#include "rsl/ResourceId.h"
#include "vlk/Valkor.h"

namespace Gfx {

struct Material: public UniformVector {
  Material();
  Material(Material&& other);
  Material& operator=(Material&& other);

  static void EditConfig(Vlk::Value* configValP);
  Result Init(const Vlk::Explorer& configEx);
  Result Init(const ResId& shaderResId);
  Result Init(Material&& other);
  Result Init(
    const ResId& shaderResId,
    const std::string& materialName,
    const aiMaterial& assimpMat,
    const std::string& directory,
    const Ds::Vector<std::string>& embeddedImageNames);
  Result InitUniform(const Vlk::Explorer& uniformEx);

  ResId mShaderId;

private:
  static const char* GetUniformName(aiTextureType aiType);
};

} // namespace Gfx

#endif
