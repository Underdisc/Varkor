#ifndef AssetLibrary_h
#define AssetLibrary_h

#include <string>

#include "ds/Map.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"

namespace AssetLibrary {

typedef int AssetId;
constexpr AssetId nInvalidAssetId = -1;

struct ShaderAsset
{
  std::string mName;
  std::string mVertexFile;
  std::string mFragmentFile;
  Gfx::Shader mShader;
};
extern Ds::Map<AssetId, ShaderAsset> nShaders;
void CreateEmptyShader();
const ShaderAsset* GetShader(AssetId shaderId);

struct ModelAsset
{
  std::string mName;
  std::string mPath;
  Gfx::Model mModel;
};
extern Ds::Map<AssetId, ModelAsset> nModels;
AssetId AddRequiredModel(const std::string& path);
void CreateEmptyModel();
const ModelAsset* GetModel(AssetId modelId);

} // namespace AssetLibrary

#endif
