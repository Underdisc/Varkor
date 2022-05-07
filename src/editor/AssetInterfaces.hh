#include "editor/Utility.h"

namespace Editor {

template<typename T>
AssetInterface<T>::AssetInterface(): mSelectedId(AssLib::nDefaultAssetId)
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
  if (ImGui::Button(createLabel.str().c_str(), ImVec2(-1, 0))) {
    AssLib::CreateEmpty<T>(assetTypename, true);
  }

  // Display a list of all existing Assets.
  ImGui::BeginChild("List", ImVec2(-1, -1), true);
  AssetId removeAssetId = AssLib::nDefaultAssetId;
  AssetId nextAssetId = mSelectedId;
  for (auto& assetPair : AssLib::AssetBin<T>::smAssets) {
    // Ensure we are not on a required Asset.
    AssetId id = assetPair.Key();
    if (AssLib::IsRequiredId(id)) {
      continue;
    }

    // Display an entry in the Asset list.
    AssLib::Asset<T>& asset = assetPair.mValue;
    ShowStatus(asset);
    ImGui::SameLine();
    bool selected = id == mSelectedId;
    ImGui::PushID(id);
    if (ImGui::Selectable(asset.mName.c_str(), selected)) {
      if (selected) {
        nextAssetId = AssLib::nDefaultAssetId;
      }
      else {
        nextAssetId = id;
      }
    }
    ImGui::PopID();
    if (ImGui::BeginPopupContextItem()) {
      if (ImGui::Selectable("Remove")) {
        removeAssetId = id;
      }
      ImGui::EndPopup();
    }

    // Display edit options for the selected Asset.
    if (selected) {
      ImGui::Separator();
      InputText("Name", &asset.mName);
      EditInitInfo(id);
      if (ImGui::Button("Refresh", ImVec2(-1, 0))) {
        asset.mStatus = AssLib::Status::Unneeded;
      }
      ImGui::Separator();
    }
  }
  ImGui::EndChild();

  // Update the selected AssetId and remove an Asset if requested.
  mSelectedId = nextAssetId;
  if (removeAssetId != AssLib::nDefaultAssetId) {
    AssLib::Remove<T>(removeAssetId);
  }
  ImGui::End();
}

template<typename T>
void AssetInterface<T>::ShowStatus(const AssLib::Asset<T>& asset)
{
  ImGui::PushItemWidth(-1);
  const ImVec4 green(0.0f, 1.0f, 0.0f, 1.0f);
  const ImVec4 red(1.0f, 0.0f, 0.0f, 1.0f);
  const ImVec4 blue(0.0f, 0.5f, 1.0f, 1.0f);
  const ImVec4 white(1.0, 1.0f, 1.0f, 1.0f);
  switch (asset.mStatus) {
  case AssLib::Status::Unneeded: ImGui::TextColored(white, "|"); break;
  case AssLib::Status::Loading: ImGui::TextColored(blue, ">"); break;
  case AssLib::Status::Failed: ImGui::TextColored(red, "-"); break;
  case AssLib::Status::Live: ImGui::TextColored(green, "+"); break;
  }
  ImGui::PopItemWidth();
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
  for (const auto& assetPair : AssLib::AssetBin<T>::smAssets) {
    if (ImGui::Selectable(assetPair.mValue.mName.c_str())) {
      mCallback(assetPair.Key());
    }
  }
  if (ImGui::Selectable("Default")) {
    mCallback(AssLib::nDefaultAssetId);
  }
  ImGui::End();
}

} // namespace Editor
