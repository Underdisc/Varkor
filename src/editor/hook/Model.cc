#include <imgui/imgui.h>
#include <sstream>

#include "AssetLibrary.h"
#include "editor/Asset.h"
#include "editor/Util.h"
#include "editor/hook/Model.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"

namespace Editor {
namespace Hook {

template<>
void Edit(Comp::Model* model)
{
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
    Editor::SelectAsset<Gfx::Model>(
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
    Editor::SelectAsset<Gfx::Shader>(
      [model](AssetId newShaderId)
      {
        model->mShaderId = newShaderId;
      });
  }
}

} // namespace Hook
} // namespace Editor
