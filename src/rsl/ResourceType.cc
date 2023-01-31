#include "rsl/ResourceType.h"
#include "gfx/Cubemap.h"
#include "gfx/Font.h"
#include "gfx/Image.h"
#include "gfx/Material.h"
#include "gfx/Mesh.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"

namespace Rsl {

ResourceTypeData nResourceTypeData[(int)ResTypeId::Count];

template<typename T>
void ResourceType<T>::Register(ResTypeId resTypeId, const char* name)
{
  smResTypeId = resTypeId;

  ResourceTypeData newTypeData;
  newTypeData.mSize = sizeof(T);
  newTypeData.mName = name;
  newTypeData.mMoveConstruct = Util::MoveConstruct<T>;
  newTypeData.mDestruct = Util::Destruct<T>;
  nResourceTypeData[(int)resTypeId] = newTypeData;
}

void RegisterResourceTypes()
{
  ResourceType<Gfx::Cubemap>::Register(ResTypeId::Cubemap, "Cubemap");
  ResourceType<Gfx::Font>::Register(ResTypeId::Font, "Font");
  ResourceType<Gfx::Image>::Register(ResTypeId::Image, "Image");
  ResourceType<Gfx::Material>::Register(ResTypeId::Material, "Material");
  ResourceType<Gfx::Mesh>::Register(ResTypeId::Mesh, "Mesh");
  ResourceType<Gfx::Model>::Register(ResTypeId::Model, "Model");
  ResourceType<Gfx::Shader>::Register(ResTypeId::Shader, "Shader");
}

ResTypeId GetResTypeId(const std::string& resTypeName)
{
  for (int i = 0; i < (int)ResTypeId::Count; ++i) {
    if (resTypeName == nResourceTypeData[i].mName) {
      return (ResTypeId)i;
    }
  }
  return ResTypeId::Invalid;
}

const ResourceTypeData& GetResTypeData(ResTypeId resTypeId)
{
  bool invalid = resTypeId == ResTypeId::Invalid;
  LogAbortIf(invalid, "Type has an invalid ResourceTypeId.");
  return nResourceTypeData[(int)resTypeId];
}

const ResTypeData& GetResTypeData(const std::string& resTypeName)
{
  ResTypeId resTypeId = GetResTypeId(resTypeName);
  return GetResTypeData(resTypeId);
}

} // namespace Rsl