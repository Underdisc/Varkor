#include <sstream>
#include <string>

#include "gfx/Model.h"
#include "util/Utility.h"

#include "AssetLibrary.h"

namespace AssetLibrary {

// Stores all of the shaders that have been added.
Ds::Map<ShaderId, Shader> nShaders;
// Used to hand out shader ids and is incremented after a shader is created.
ShaderId nShaderIdHandout = 0;

void CreateEmptyShader()
{
  std::stringstream name;
  name << "Shader " << nShaderIdHandout;
  Shader shaderAsset;
  shaderAsset.mName = name.str();
  nShaders.Insert(nShaderIdHandout, shaderAsset);
  ++nShaderIdHandout;
}

// Stores all of the models that have been added.
Ds::Map<std::string, Gfx::Model> nModels;

AddResult AddModel(const std::string& path)
{
  // Check to make sure the model hasn't already been added.
  const Gfx::Model* model = nModels.Find(path);
  AddResult result;
  if (model != nullptr)
  {
    result.mSuccess = true;
    return result;
  }

  // Load the requested model and add it to the loaded models if it loaded
  // successfully.
  Gfx::Model newModel(path, &result.mSuccess, &result.mError);
  if (!result.mSuccess)
  {
    return result;
  }
  nModels.Insert(path, Util::Move(newModel));
  return result;
}

const Gfx::Model* GetModel(const std::string& path)
{
  return nModels.Find(path);
}

} // namespace AssetLibrary
