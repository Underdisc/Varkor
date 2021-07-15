#include <glad/glad.h>
#include <imgui/imgui.h>
#include <sstream>

#include "AssetLibrary.h"
#include "editor/Asset.h"
#include "editor/Util.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"

namespace Editor {

template<typename T>
void ShowStatus(const AssLib::Asset<T>& asset)
{
  ImGui::PushItemWidth(-1);
  if (asset.mResource.Live())
  {
    ImVec4 green(0.0f, 1.0f, 0.0f, 1.0f);
    ImGui::TextColored(green, "+");
  } else
  {
    ImVec4 red(1.0f, 0.0f, 0.0f, 1.0f);
    ImGui::TextColored(red, "-");
  }
  ImGui::PopItemWidth();
}

template<typename T>
void EditAssetHeader(const std::string& typeName, AssLib::Asset<T>* asset)
{
  InputText("Name", &asset->mName);
  if (ImGui::Button("Init", ImVec2(-1.0f, 0.0f)))
  {
    Util::Result result = asset->Init();
    if (!result.Success())
    {
      OpenPopup(typeName + " Init Error", result.mError.c_str());
    }
  }
}

template<typename T>
void EditAssetPath(int pathIndex, const char* preLabel, AssetId id)
{
  std::stringstream buttonLabel;
  buttonLabel << preLabel;
  const std::string& currentPath = AssLib::GetAsset<T>(id).GetPath(pathIndex);
  if (currentPath.empty())
  {
    buttonLabel << "None";
  } else
  {
    buttonLabel << currentPath;
  }
  if (ImGui::Button(buttonLabel.str().c_str(), ImVec2(-1.0f, 0.0f)))
  {
    StartFileSelection(
      [id, pathIndex](const std::string& newPath)
      {
        AssLib::Asset<T>* asset = AssLib::TryGetAsset<T>(id);
        if (asset != nullptr)
        {
          asset->SetPath(pathIndex, newPath);
        }
      });
  }
}

template<typename T>
void EditAssetBody(AssetId id);
template<>
void EditAssetBody<Gfx::Shader>(AssetId id)
{
  EditAssetPath<Gfx::Shader>(0, "Vertex: ", id);
  EditAssetPath<Gfx::Shader>(1, "Fragment: ", id);
}
template<>
void EditAssetBody<Gfx::Model>(AssetId id)
{
  EditAssetPath<Gfx::Model>(0, "Model: ", id);
}

template<typename T>
void ShowAssets()
{
  // Display a button for creating a new Asset.
  std::stringstream createLabel;
  std::string assetTypeName = Util::GetShortTypename<T>();
  createLabel << "Create " << assetTypeName;
  if (ImGui::Button(createLabel.str().c_str(), ImVec2(-1, 0)))
  {
    AssLib::Create<T>(assetTypeName, true);
  }

  // Display a list of all existing Assets.
  ImGui::BeginChild("List", ImVec2(-1, -1), true);
  AssetId removeAssetId = AssLib::nInvalidAssetId;
  AssetId nextAssetId = AssetInfo<T>::smSelectionId;
  for (auto& assetPair : AssLib::AssetBin<T>::smAssets)
  {
    AssetId id = assetPair.Key();
    AssLib::Asset<T>& asset = assetPair.mValue;
    if (asset.Required())
    {
      continue;
    }

    // Display an entry in the Asset list.
    ShowStatus(asset);
    ImGui::SameLine();
    bool selected = id == AssetInfo<T>::smSelectionId;
    ImGui::PushID(id);
    if (ImGui::Selectable(asset.mName.c_str(), selected))
    {
      if (selected)
      {
        nextAssetId = AssLib::nInvalidAssetId;
      } else
      {
        nextAssetId = id;
      }
    }
    ImGui::PopID();
    if (ImGui::BeginPopupContextItem())
    {
      if (ImGui::Selectable("Remove"))
      {
        removeAssetId = id;
      }
      ImGui::EndPopup();
    }

    // Display edit options for the selected Asset.
    if (selected)
    {
      ImGui::PushItemWidth(-1);
      EditAssetHeader<T>(assetTypeName, &asset);
      EditAssetBody<T>(AssetInfo<T>::smSelectionId);
      ImGui::PopItemWidth();
    }
  }
  ImGui::EndChild();

  // Update the selected AssetId and remove an Asset if requested.
  AssetInfo<T>::smSelectionId = nextAssetId;
  if (removeAssetId != AssLib::nInvalidAssetId)
  {
    AssLib::Remove<T>(removeAssetId);
  }
}

template<typename T>
void SelectAssetWindow()
{
  if (AssetInfo<T>::smCallback == nullptr)
  {
    return;
  }
  // Display a window of all selectable Assets.
  bool continueWindow = true;
  std::string windowName = "Select ";
  windowName += Util::GetShortTypename<T>();
  ImGui::Begin(windowName.c_str(), &continueWindow);
  for (const auto& assetPair : AssLib::AssetBin<T>::smAssets)
  {
    if (assetPair.mValue.Required())
    {
      continue;
    }
    if (ImGui::Selectable(assetPair.mValue.mName.c_str()))
    {
      AssetInfo<T>::smCallback(assetPair.Key());
      continueWindow = false;
    }
  }
  ImGui::End();
  if (!continueWindow)
  {
    AssetInfo<T>::smCallback = nullptr;
  }
}

bool nShowAssetWindow = false;
void AssetWindows()
{
  if (nShowAssetWindow)
  {
    ImGui::Begin("Asset", &nShowAssetWindow);
    ImVec2 childSize(ImGui::GetWindowContentRegionWidth() * 0.5f, -1);
    ImGui::BeginChild("Shader", childSize, true);
    ShowAssets<Gfx::Shader>();
    ImGui::EndChild();
    ImGui::SameLine();

    ImGui::BeginChild("Model", childSize, true);
    ShowAssets<Gfx::Model>();
    ImGui::EndChild();
    ImGui::End();
  }
  SelectAssetWindow<Gfx::Shader>();
  SelectAssetWindow<Gfx::Model>();
}

} // namespace Editor
