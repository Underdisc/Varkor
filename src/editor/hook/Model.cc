#include <imgui/imgui.h>
#include <sstream>

#include "AssetLibrary.h"
#include "editor/AssetInterfaces.h"
#include "editor/hook/Model.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"

namespace Editor {

bool Hook<Comp::Model>::Edit(const World::Object& object)
{
  Comp::Model* model = object.GetComponent<Comp::Model>();
  std::stringstream buttonLabel;
  buttonLabel << "Model: ";
  AssLib::Asset<Gfx::Model>* modelAsset =
    AssLib::TryGetAsset<Gfx::Model>(model->mModelId);
  if (modelAsset == nullptr)
  {
    buttonLabel << "None";
  } else
  {
    buttonLabel << modelAsset->mName;
  }
  if (ImGui::Button(buttonLabel.str().c_str(), ImVec2(-1.0f, 0.0f)))
  {
    OpenInterface<SelectAssetInterface<Gfx::Model>>(
      [model](AssetId newModelId)
      {
        model->mModelId = newModelId;
      });
  }

  buttonLabel.str("");
  buttonLabel << "Shader: ";
  AssLib::Asset<Gfx::Shader>* shaderAsset =
    AssLib::TryGetAsset<Gfx::Shader>(model->mShaderId);
  if (shaderAsset == nullptr)
  {
    buttonLabel << "None";
  } else
  {
    buttonLabel << shaderAsset->mName;
  }
  if (ImGui::Button(buttonLabel.str().c_str(), ImVec2(-1.0f, 0.0f)))
  {
    OpenInterface<SelectAssetInterface<Gfx::Shader>>(
      [model](AssetId newShaderId)
      {
        model->mShaderId = newShaderId;
      });
  }
  return false;
}

} // namespace Editor
