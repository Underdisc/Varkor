#include <imgui/imgui.h>
#include <sstream>

#include "AssetLibrary.h"
#include "editor/Asset.h"
#include "editor/Util.h"

#include "Model.h"

namespace Editor {
namespace Hook {

void ChangeAssetCallback(const std::string& path, void* data)
{
  Comp::Model* model = (Comp::Model*)data;
  AssetLibrary::AddResult result = AssetLibrary::AddModel(path);
  if (result.mSuccess)
  {
    model->mAsset = path;
  } else
  {
    Editor::OpenPopup("Model Import Failed", result.mError.c_str());
  }
}

void ChangeShaderCallback(AssetLibrary::ShaderId shaderId, void* data)
{
  Comp::Model* model = (Comp::Model*)data;
  model->mShaderId = shaderId;
}

template<>
void Edit(Comp::Model* model)
{
  std::stringstream buttonLabel;
  buttonLabel << "Model: ";
  if (model->mAsset.empty())
  {
    buttonLabel << "None";
  } else
  {
    buttonLabel << model->mAsset;
  }
  if (ImGui::Button(buttonLabel.str().c_str(), ImVec2(-1.0f, 0.0f)))
  {
    Editor::StartFileSelection(ChangeAssetCallback, (void*)model);
  }

  buttonLabel.str("");
  buttonLabel << "Shader: ";
  const AssetLibrary::Shader* shaderAsset =
    AssetLibrary::GetShader(model->mShaderId);
  if (shaderAsset == nullptr)
  {
    buttonLabel << "None";
  } else
  {
    buttonLabel << shaderAsset->mName;
  }
  if (ImGui::Button(buttonLabel.str().c_str(), ImVec2(-1.0f, 0.0f)))
  {
    Editor::StartShaderSelection(ChangeShaderCallback, (void*)model);
  }
}

} // namespace Hook
} // namespace Editor
