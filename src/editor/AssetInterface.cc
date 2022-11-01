#include "editor/AssetInterface.h"
#include "editor/ResourceInterface.h"

namespace Editor {

AssetInterface::AssetInterface() {}

void AssetInterface::Show()
{
  ImGui::Begin("Asset", &mOpen);

  // Display tabs for all of the selected assets.
  ImGuiTabBarFlags flags =
    ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_AutoSelectNewTabs;
  if (!ImGui::BeginTabBar("Assets", flags)) {
    return;
  }
  std::string deselectedAssetName = Rsl::nInvalidAssetName;
  for (const SelectedAsset& selectedAsset : mSelectedAssets) {
    bool tabOpen = true;
    if (ImGui::BeginTabItem(selectedAsset.mName.c_str(), &tabOpen)) {
      ShowTabContent(selectedAsset);
      ImGui::EndTabItem();
    }
    if (!tabOpen) {
      deselectedAssetName = selectedAsset.mName;
    }
  }
  if (deselectedAssetName != Rsl::nInvalidAssetName) {
    DeselectAsset(deselectedAssetName);
  }
  ImGui::EndTabBar();
  ImGui::End();
}

void AssetInterface::ShowTabContent(const SelectedAsset& selectedAsset)
{
  Rsl::Asset& asset = Rsl::GetAsset(selectedAsset.mName);
  switch (asset.GetStatus()) {
  case Rsl::Asset::Status::Dormant:
    if (ImGui::Button("Init", ImVec2(-1, 0))) {
      asset.QueueInit();
    }
    break;
  case Rsl::Asset::Status::Failed:
  case Rsl::Asset::Status::Live:
    if (ImGui::Button("Refresh", ImVec2(-1, 0))) {
      asset.Sleep();
    }
    break;
  case Rsl::Asset::Status::Initializing:
    ImGui::Button("Initializing", ImVec2(-1, 0));
  }

  // Display a list of the resources owned by the asset.
  ImGuiTabBarFlags flags = ImGuiTabBarFlags_FittingPolicyResizeDown;
  if (!ImGui::BeginTabBar("ResourceTableTabs", flags)) {
    return;
  }
  if (ImGui::BeginTabItem("Defined Resources")) {
    ShowDefinedResources(selectedAsset);
    ImGui::EndTabItem();
  }
  if (ImGui::BeginTabItem("Initialized Resources")) {
    ShowInitializedResources(asset);
    ImGui::EndTabItem();
  }
  ImGui::EndTabBar();
}

void AssetInterface::ShowDefinedResources(const SelectedAsset& selectedAsset)
{
  ImGuiTableFlags flags = ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV |
    ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY;
  if (!ImGui::BeginTable("Resources", 2, flags)) {
    return;
  }

  // Table header.
  ImGui::TableSetupScrollFreeze(0, 1);
  ImGui::TableSetupColumn("Type");
  ImGui::TableSetupColumn("Name");
  ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
  for (int i = 0; i < 2; ++i) {
    ImGui::TableSetColumnIndex(i);
    ImGui::TableHeader(ImGui::TableGetColumnName(i));
  }

  // Get the currently selected resource.
  ResourceInterface* resInterface = FindInterface<ResourceInterface>();
  ResId selectedId = "";
  if (resInterface != nullptr) {
    selectedId = resInterface->mResId;
  }

  // Display the resource list.
  Vlk::Explorer assetEx(selectedAsset.mVal);
  for (int i = 0; i < assetEx.Size(); ++i) {
    ImGui::TableNextRow();
    Vlk::Explorer resEx = assetEx(i);
    Vlk::Explorer resTypeEx = resEx("Type");
    std::string resTypeString = resTypeEx.As<std::string>("Invalid");
    ImGui::TableNextColumn();
    ImGui::Text(resTypeString.c_str());

    std::string resName = resEx.Key();
    ResId currentId(selectedAsset.mName, resName);
    bool selected = currentId == selectedId;
    ImGui::TableNextColumn();
    if (ImGui::Selectable(resName.c_str(), selected)) {
      if (selected) {
        CloseInterface<ResourceInterface>();
      }
      else {
        OpenInterface<ResourceInterface>(currentId);
      }
    }
    Rsl::ResourceTypeId resTypeId = Rsl::GetResTypeId(resTypeString);
    DragResourceId(resTypeId, currentId);
  }
  ImGui::EndTable();
}

void AssetInterface::ShowInitializedResources(const Rsl::Asset& asset)
{
  // Only display initialized resources if the Asset's status is Live.
  if (asset.GetStatus() != Rsl::Asset::Status::Live) {
    if (asset.GetStatus() == Rsl::Asset::Status::Failed) {
      ImGui::Text("Asset initialization failed.");
    }
    else {
      ImGui::Text("Asset not initialized.");
    }
    return;
  }

  ImGuiTableFlags flags = ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV |
    ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY;
  bool showTable = ImGui::BeginTable("InitializedResources", 2, flags);
  if (!showTable) {
    return;
  }

  // Table header.
  ImGui::TableSetupScrollFreeze(0, 1);
  ImGui::TableSetupColumn("Type");
  ImGui::TableSetupColumn("Name");
  ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
  for (int i = 0; i < 2; ++i) {
    ImGui::TableSetColumnIndex(i);
    ImGui::TableHeader(ImGui::TableGetColumnName(i));
  }

  // Display the resource list.
  const Ds::Vector<Rsl::Asset::ResDesc>& resDescs = asset.GetResDescs();
  for (const Rsl::Asset::ResDesc& resDesc : resDescs) {
    ImGui::TableNextRow();
    const Rsl::ResTypeData& resTypeData =
      Rsl::GetResTypeData(resDesc.mResTypeId);
    ImGui::TableNextColumn();
    ImGui::Text(resTypeData.mName);

    ImGui::TableNextColumn();
    ImGui::Selectable(resDesc.mName.c_str());
    DragResourceId(resDesc.mResTypeId, ResId(asset.GetName(), resDesc.mName));
  }
  ImGui::EndTable();
}

Result AssetInterface::SelectAsset(const std::string& assetName)
{
  Rsl::Asset& asset = Rsl::GetAsset(assetName);
  ValueResult<Vlk::Value> result = asset.GetVlkValue();
  if (!result.Success()) {
    return result;
  }
  SelectedAsset newSelectedAsset;
  newSelectedAsset.mName = assetName;
  newSelectedAsset.mVal = std::move(result.mValue);
  mSelectedAssets.Push(std::move(newSelectedAsset));
  return Result();
}

void AssetInterface::DeselectAsset(const std::string& assetName)
{
  for (size_t i = 0; i < mSelectedAssets.Size(); ++i) {
    if (assetName == mSelectedAssets[i].mName) {
      mSelectedAssets.Remove(i);
      if (mSelectedAssets.Size() == 0) {
        mOpen = false;
      }
      return;
    }
  }
  std::string error = "\"" + assetName + "\" not selected.";
  LogAbort(error.c_str());
}

bool AssetInterface::IsAssetSelected(const std::string& assetName)
{
  for (const SelectedAsset& selectedAsset : mSelectedAssets) {
    if (assetName == selectedAsset.mName) {
      return true;
    }
  }
  return false;
}

} // namespace Editor