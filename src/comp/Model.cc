#include <imgui/imgui.h>
#include <sstream>

#include "AssetLibrary.h"
#include "editor/Asset.h"
#include "editor/Util.h"

#include "Model.h"

namespace Comp {

Model::Model(): mShaderId(AssetLibrary::nInvalidShaderId), mAsset() {}

void ChangeAssetCallback(const std::string& path, void* data)
{
  Model* comp = (Model*)data;
  AssetLibrary::AddResult result = AssetLibrary::AddModel(path);
  if (result.mSuccess)
  {
    comp->mAsset = path;
  } else
  {
    Editor::OpenPopup("Model Import Failed", result.mError.c_str());
  }
}

void ChangeShaderCallback(AssetLibrary::ShaderId shaderId, void* data)
{
  Model* comp = (Model*)data;
  comp->mShaderId = shaderId;
}

void Model::EditorHook()
{
  std::stringstream buttonLabel;
  buttonLabel << "Model: ";
  if (mAsset.empty())
  {
    buttonLabel << "None";
  } else
  {
    buttonLabel << mAsset;
  }
  if (ImGui::Button(buttonLabel.str().c_str(), ImVec2(-1.0f, 0.0f)))
  {
    Editor::StartFileSelection(ChangeAssetCallback, (void*)this);
  }

  buttonLabel.str("");
  buttonLabel << "Shader: ";
  const AssetLibrary::Shader* shaderAsset = AssetLibrary::GetShader(mShaderId);
  if (shaderAsset == nullptr)
  {
    buttonLabel << "None";
  } else
  {
    buttonLabel << shaderAsset->mName;
  }
  if (ImGui::Button(buttonLabel.str().c_str(), ImVec2(-1.0f, 0.0f)))
  {
    Editor::StartShaderSelection(ChangeShaderCallback, (void*)this);
  }
}

} // namespace Comp
