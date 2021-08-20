#include "editor/Utility.h"

namespace Editor {

template<typename T>
AssetInterface<T>::AssetInterface(): mSelectedId(AssLib::nInvalidAssetId)
{}

template<typename T>
void AssetInterface<T>::Show()
{
  std::string assetTypename = Util::GetShortTypename<T>();
  std::stringstream windowLabel;
  windowLabel << assetTypename << " Assets";
  ImGui::Begin(windowLabel.str().c_str(), &mOpen);

  // Display a button for creating a new Asset.
  std::stringstream createLabel;
  createLabel << "Create " << assetTypename;
  if (ImGui::Button(createLabel.str().c_str(), ImVec2(-1, 0)))
  {
    AssLib::Create<T>(assetTypename, true);
  }

  // Display a list of all existing Assets.
  ImGui::BeginChild("List", ImVec2(-1, -1), true);
  AssetId removeAssetId = AssLib::nInvalidAssetId;
  AssetId nextAssetId = mSelectedId;
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
    bool selected = id == mSelectedId;
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
      EditAssetHeader(&asset);
      EditAssetPaths(mSelectedId);
      ImGui::PopItemWidth();
    }
  }
  ImGui::EndChild();

  // Update the selected AssetId and remove an Asset if requested.
  mSelectedId = nextAssetId;
  if (removeAssetId != AssLib::nInvalidAssetId)
  {
    AssLib::Remove<T>(removeAssetId);
  }
  ImGui::End();
}

template<typename T>
void AssetInterface<T>::ShowStatus(const AssLib::Asset<T>& asset)
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
void AssetInterface<T>::EditAssetHeader(AssLib::Asset<T>* asset)
{
  InputText("Name", &asset->mName);
  if (ImGui::Button("Init", ImVec2(-1.0f, 0.0f)))
  {
    Util::Result result = asset->Init();
    if (!result.Success())
    {
      std::string assetTypename = Util::GetShortTypename<T>();
      OpenInterface<PopupInterface>(
        assetTypename + " Init Error", result.mError.c_str());
    }
  }
}

template<typename T>
void AssetInterface<T>::EditAssetPaths(AssetId id)
{
  for (int i = 0; i < T::smInitPathCount; ++i)
  {
    std::stringstream buttonLabel;
    buttonLabel << T::smPathNames[i] << ": ";
    const std::string& currentPath = AssLib::GetAsset<T>(id).GetPath(i);
    if (currentPath.empty())
    {
      buttonLabel << "None";
    } else
    {
      buttonLabel << currentPath;
    }
    if (!ImGui::Button(buttonLabel.str().c_str(), ImVec2(-1.0f, 0.0f)))
    {
      continue;
    }
    OpenInterface<FileInterface>(
      [id, i](const std::string& newPath)
      {
        AssLib::Asset<T>* asset = AssLib::TryGetAsset<T>(id);
        if (asset != nullptr)
        {
          asset->SetPath(i, newPath);
        }
      },
      FileInterface::AccessType::Select);
  }
}

template<typename T>
SelectAssetInterface<T>::SelectAssetInterface(
  std::function<void(AssetId)> callback):
  mCallback(callback)
{}

template<typename T>
void SelectAssetInterface<T>::Show()
{
  // Display a window of all selectable Assets.
  std::string windowName = "Select ";
  windowName += Util::GetShortTypename<T>();
  ImGui::Begin(windowName.c_str(), &mOpen);
  for (const auto& assetPair : AssLib::AssetBin<T>::smAssets)
  {
    if (assetPair.mValue.Required())
    {
      continue;
    }
    if (ImGui::Selectable(assetPair.mValue.mName.c_str()))
    {
      mCallback(assetPair.Key());
    }
  }
  ImGui::End();
}

} // namespace Editor
