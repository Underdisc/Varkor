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
  if (ImGui::Button(createLabel.str().c_str(), ImVec2(-1, 0))) {
    AssLib::CreateSerializable<T>(assetTypename);
  }

  // Display a list of all existing Assets.
  ImGui::BeginChild("List", ImVec2(-1, -1), true);
  AssetId removeAssetId = AssLib::nDefaultAssetId;
  AssetId lastId = AssLib::nInvalidAssetId;
  AssetId nextSelectedId = mSelectedId;
  for (auto& assetPair : AssLib::AssetBin<T>::smAssets) {
    // Display a line between serializable and nonserializable assets.
    AssLib::Asset<T>& asset = assetPair.mValue;
    AssetId id = assetPair.Key();
    if (AssLib::SerializableId(id) && !AssLib::SerializableId(lastId)) {
      ImGui::Separator();
    }
    lastId = id;

    // Display an entry in the Asset list.
    ShowStatus(asset.mStatus);
    ImGui::SameLine();
    bool selected = id == mSelectedId;
    ImGui::PushID(id);
    if (ImGui::Selectable(asset.mName.c_str(), selected)) {
      if (selected) {
        nextSelectedId = AssLib::nInvalidAssetId;
      }
      else {
        nextSelectedId = id;
      }
    }
    ImGui::PopID();
    if (AssLib::SerializableId(id) && ImGui::BeginPopupContextItem()) {
      if (ImGui::Selectable("Remove")) {
        removeAssetId = id;
      }
      ImGui::EndPopup();
    }
    if (ImGui::BeginDragDropSource()) {
      std::stringstream payloadType;
      payloadType << assetTypename << "AssetId";
      ImGui::SetDragDropPayload(
        payloadType.str().c_str(), &id, sizeof(AssLib::AssetId));
      ImGui::Text(asset.mName.c_str());
      ImGui::EndDragDropSource();
    }

    // Display edit options for the selected Asset.
    if (selected && id != AssLib::nDefaultAssetId) {
      ImGui::Separator();
      if (AssLib::SerializableId(id)) {
        InputText("Name", &asset.mName);
        EditInitInfo(id);
      }
      if (ImGui::Button("Refresh", ImVec2(-1, 0))) {
        asset.mStatus = AssLib::Status::Unneeded;
      }
      ImGui::Separator();
    }
  }
  ImGui::EndChild();

  // Update the selected AssetId and remove an Asset if requested.
  mSelectedId = nextSelectedId;
  if (removeAssetId != AssLib::nDefaultAssetId) {
    AssLib::Remove<T>(removeAssetId);
  }
  ImGui::End();
}

template<typename T>
void AssetInterface<T>::ShowStatus(AssLib::Status status)
{
  ImGui::PushItemWidth(-1);
  const ImVec4 green(0.0f, 1.0f, 0.0f, 1.0f);
  const ImVec4 red(1.0f, 0.0f, 0.0f, 1.0f);
  const ImVec4 blue(0.0f, 0.5f, 1.0f, 1.0f);
  const ImVec4 white(1.0, 1.0f, 1.0f, 1.0f);
  switch (status) {
  case AssLib::Status::Unneeded: ImGui::TextColored(white, "|"); break;
  case AssLib::Status::Loading: ImGui::TextColored(blue, ">"); break;
  case AssLib::Status::Failed: ImGui::TextColored(red, "-"); break;
  case AssLib::Status::Live: ImGui::TextColored(green, "+"); break;
  }
  ImGui::PopItemWidth();
}

template<typename T>
void DropAssetWidget(AssetId* currentId)
{
  std::stringstream buttonLabel;
  buttonLabel << Util::GetShortTypename<T>() << ": ";
  AssLib::Asset<T>* asset = AssLib::TryGetAsset<T>(*currentId);
  if (asset == nullptr) {
    buttonLabel << "Invalid";
  }
  else {
    buttonLabel << asset->mName;
  }
  ImGui::Button(buttonLabel.str().c_str(), ImVec2(-1, 0));
  if (ImGui::BeginDragDropTarget()) {
    std::stringstream payloadType;
    payloadType << Util::GetShortTypename<T>() << "AssetId";
    const ImGuiPayload* payload =
      ImGui::AcceptDragDropPayload(payloadType.str().c_str());
    if (payload != nullptr) {
      *currentId = *(AssetId*)payload->Data;
    }
    ImGui::EndDragDropTarget();
  }
}

} // namespace Editor
