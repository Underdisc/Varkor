#include <imgui/imgui.h>
#include <sstream>

#include "AssetLibrary.h"
#include "editor/Asset.h"
#include "editor/Util.h"

#include "Model.h"

namespace Editor {
namespace Hook {

void ChangeModelCallback(AssetLibrary::AssetId modelId, void* data)
{
  Comp::Model* model = (Comp::Model*)data;
  model->mModelId = modelId;
}

void ChangeShaderCallback(AssetLibrary::AssetId shaderId, void* data)
{
  Comp::Model* model = (Comp::Model*)data;
  model->mShaderId = shaderId;
}

template<>
void Edit(Comp::Model* model)
{
  std::stringstream buttonLabel;
  buttonLabel << "Model: ";
  const AssetLibrary::ModelAsset* modelAsset =
    AssetLibrary::GetModel(model->mModelId);
  if (modelAsset == nullptr)
  {
    buttonLabel << "None";
  } else
  {
    buttonLabel << modelAsset->mName;
  }
  if (ImGui::Button(buttonLabel.str().c_str(), ImVec2(-1.0f, 0.0f)))
  {
    Editor::StartModelSelection(ChangeModelCallback, (void*)model);
  }

  buttonLabel.str("");
  buttonLabel << "Shader: ";
  const AssetLibrary::ShaderAsset* shaderAsset =
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
