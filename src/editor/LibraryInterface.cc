#include <imgui/imgui.h>

#include "editor/AssetInterface.h"
#include "editor/LibraryInterface.h"

namespace Editor {

LibraryInterface::LibraryInterface() {}

void LibraryInterface::Show()
{
  // Display all of the assets.
  ImGui::Begin("Library", &mOpen);
  AssetInterface* assetInterface = FindInterface<AssetInterface>();
  std::string removeAssetName = Rsl::nInvalidAssetName;
  for (Rsl::Asset& asset : Rsl::nAssets) {
    ShowStatus(asset.GetStatus());
    ImGui::SameLine();

    // Make the asset selectable for opening up a tab in the AssetInterface.
    bool selected = false;
    if (assetInterface != nullptr) {
      selected = assetInterface->IsAssetSelected(asset.GetName());
    }
    ImGui::PushID(asset.GetName().c_str());
    if (ImGui::Selectable(asset.GetName().c_str(), selected)) {
      if (assetInterface == nullptr) {
        assetInterface = OpenInterface<AssetInterface>();
      }
      if (selected) {
        assetInterface->DeselectAsset(asset.GetName());
      }
      else {
        Result result = assetInterface->SelectAsset(asset.GetName());
        LogErrorIf(!result.Success(), result.mError.c_str());
      }
    }
    ImGui::PopID();

    // Make the asset removable.
    if (ImGui::BeginPopupContextItem()) {
      if (ImGui::Selectable("Remove")) {
        removeAssetName = asset.GetName();
      }
      ImGui::EndPopup();
    }
  }

  if (removeAssetName != Rsl::nInvalidAssetName) {
    Rsl::RemoveAsset(removeAssetName);
    bool deselectAsset = assetInterface != nullptr &&
      assetInterface->IsAssetSelected(removeAssetName);
    if (deselectAsset) {
      assetInterface->DeselectAsset(removeAssetName);
    }
  }
  ImGui::End();
}

void LibraryInterface::ShowStatus(Rsl::Asset::Status status)
{
  ImGui::PushItemWidth(-1);
  const ImVec4 blue(0.0f, 0.5f, 1.0f, 1.0f);
  const ImVec4 purple(0.5f, 0.0f, 1.0f, 1.0f);
  const ImVec4 orange(1.0, 0.5f, 0.0f, 1.0f);
  const ImVec4 red(1.0f, 0.0f, 0.0f, 1.0f);
  const ImVec4 green(0.0f, 1.0f, 0.0f, 1.0f);
  typedef Rsl::Asset::Status Status;
  switch (status) {
  case Status::Dormant: ImGui::TextColored(blue, "o"); break;
  case Status::Queued: ImGui::TextColored(purple, "~"); break;
  case Status::Initializing: ImGui::TextColored(orange, ">"); break;
  case Status::Failed: ImGui::TextColored(red, "-"); break;
  case Status::Live: ImGui::TextColored(green, "+"); break;
  }
  ImGui::PopItemWidth();
}

} // namespace Editor