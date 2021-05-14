#include <sstream>
#include <string>

#include "Error.h"
#include "gfx/Model.h"
#include "util/Utility.h"

#include "AssetLibrary.h"

namespace AssetLibrary {

Ds::Map<AssetId, ShaderAsset> nShaders;
Ds::Map<AssetId, ModelAsset> nModels;
AssetId nShaderIdHandout = 0;
AssetId nModelIdHandout = 0;

void CreateEmptyShader()
{
  std::stringstream name;
  name << "Shader " << nShaderIdHandout;
  ShaderAsset shaderAsset;
  shaderAsset.mName = name.str();
  nShaders.Insert(nShaderIdHandout, shaderAsset);
  ++nShaderIdHandout;
}

const ShaderAsset* GetShader(AssetId shaderId)
{
  return nShaders.Find(shaderId);
}

void CreateEmptyModel()
{
  std::stringstream name;
  name << "Model " << nModelIdHandout;
  ModelAsset modelAsset;
  modelAsset.mName = name.str();
  nModels.Insert(nModelIdHandout++, Util::Move(modelAsset));
}

const ModelAsset* GetModel(AssetId modelId)
{
  return nModels.Find(modelId);
}

AssetId AddRequiredModel(const std::string& path)
{
  std::stringstream name;
  name << "Required Model" << std::endl;
  ModelAsset modelAsset;
  modelAsset.mName = name.str();
  modelAsset.mPath = path;
  Gfx::Model::InitResult result = modelAsset.mModel.Init(path);
  LogAbortIf(!result.mSuccess, result.mError.c_str());
  nModels.Insert(nModelIdHandout, Util::Move(modelAsset));
  return nModelIdHandout++;
}

} // namespace AssetLibrary
