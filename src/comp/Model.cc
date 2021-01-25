#include <imgui/imgui.h>

#include "AssetLibrary.h"
#include "editor/Util.h"

#include "Model.h"

namespace Comp {

void ChangeAssetCallback(const std::string& path, void* data)
{
  Model* comp = (Model*)data;
  AssetLibrary::AddResult result = AssetLibrary::AddModel(path);
  if (result.mSuccess)
  {
    comp->mAsset = path;
  } else
  {
    Editor::OpenPopup("Model Import Failed", result.mError);
  }
}

void Model::EditorHook()
{
  if (ImGui::Button("Select Model", ImVec2(-1, 0)))
  {
    Editor::StartFileSelection(ChangeAssetCallback, (void*)this);
  }
  if (mAsset.empty())
  {
    ImGui::Text("Current Model: None");
  } else
  {
    ImGui::Text("Current Model: %s", mAsset.c_str());
  }
}

} // namespace Comp
