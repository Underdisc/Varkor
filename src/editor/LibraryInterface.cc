#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "Options.h"
#include "editor/Editor.h"
#include "editor/LayerInterface.h"
#include "editor/LibraryInterface.h"
#include "editor/ResourceInterface.h"
#include "editor/Utility.h"
#include "rsl/Library.h"
#include "world/World.h"

namespace Editor {

LibraryInterface::LibraryInterface(): mRootTree("")
{
  mHeight = ImGui::GetFontSize();
  mHalfHeight = mHeight / 2.0f;
  mDecorationColor = ImGui::GetColorU32(ImGuiCol_Border);
  mLineThickness = 2.0f;

  // The points used for drawing the open and closed triangles.
  mArrowPointsClosed[0] = {-mHalfHeight, -mHalfHeight};
  mArrowPointsClosed[1] = {-mHalfHeight, mHalfHeight};
  mArrowPointsClosed[2] = {mHalfHeight, 0.0f};
  mArrowPointsOpen[0] = {-mHalfHeight, -mHalfHeight};
  mArrowPointsOpen[1] = {mHalfHeight, -mHalfHeight};
  mArrowPointsOpen[2] = {0.0f, mHalfHeight};
  const float scale = 0.5f;
  for (int i = 0; i < 3; ++i) {
    mArrowPointsClosed[i] *= scale;
    mArrowPointsOpen[i] *= scale;
  }
}

void LibraryInterface::Show()
{
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 4.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f));

  ImGui::Begin("Library", &mOpen);
  ShowDirectoryEntry("", "vres", &mRootTree, 0);
  std::string resDir = Rsl::ResDirectory();
  if (std::filesystem::is_directory(resDir)) {
    ShowDirectory(resDir + '/', "", &mRootTree, 0);
  }
  ImGui::End();

  ImGui::PopStyleVar();
  ImGui::PopStyleVar();
}

LibraryInterface::OpenAsset::OpenAsset(const std::string& name):
  mName(name), mShowDefinedResources(true)
{}

LibraryInterface::Tree::Tree(const std::string& name): mName(name) {}

LibraryInterface::Tree* LibraryInterface::Tree::ToggleSubTree(
  const std::string& name)
{
  for (int i = 0; i < mSubTrees.Size(); ++i) {
    if (mSubTrees[i].mName == name) {
      mSubTrees.Remove(i);
      return nullptr;
    }
  }
  mSubTrees.Emplace(name);
  return &mSubTrees.Top();
}

LibraryInterface::Tree* LibraryInterface::Tree::TryGetSubTree(
  const std::string& name)
{
  for (Tree& tree : mSubTrees) {
    if (tree.mName == name) {
      return &tree;
    }
  }
  return nullptr;
}

LibraryInterface::OpenAsset* LibraryInterface::Tree::ToggleOpenAsset(
  const std::string& assetName)
{
  for (int i = 0; i < mOpenAssets.Size(); ++i) {
    if (mOpenAssets[i].mName == assetName) {
      mOpenAssets.Remove(i);
      Rsl::RemConfig(assetName);
      return nullptr;
    }
  }

  VResult<Vlk::Value*> addConfigResult = Rsl::AddConfig(assetName);
  if (!addConfigResult.Success()) {
    LogError(addConfigResult.mError.c_str());
    return nullptr;
  }
  Result getAllDefResInfoResult =
    Rsl::Asset::GetAllDefResInfo(*addConfigResult.mValue);
  if (!getAllDefResInfoResult.Success()) {
    std::string error = "Asset \"" + assetName +
      "\" get all defined resource info failed.\n" +
      getAllDefResInfoResult.mError;
    LogError(error.c_str());
    Rsl::RemConfig(assetName);
    return nullptr;
  }
  mOpenAssets.Emplace(assetName);
  return &mOpenAssets.Top();
}

LibraryInterface::OpenAsset* LibraryInterface::Tree::TryGetOpenAsset(
  const std::string& name)
{
  for (OpenAsset& openAsset : mOpenAssets) {
    if (openAsset.mName == name) {
      return &openAsset;
    }
  }
  return nullptr;
}

void LibraryInterface::ShowDirectory(
  const std::string& rootPath,
  const std::string& path,
  Tree* dirTree,
  int indents)
{
  // Go through all entries in the directory.
  ImGui::PushID(dirTree->mName.c_str());
  float lineStartY = ImGui::GetCursorScreenPos().y;
  float lineEndY;
  std::filesystem::directory_iterator dirIter(rootPath + path);
  for (const std::filesystem::directory_entry& dirEntry : dirIter) {
    lineEndY = ImGui::GetCursorScreenPos().y;

    std::string entryName = dirEntry.path().filename().string();
    std::string entryPath = path + entryName;
    if (dirEntry.is_directory()) {
      ShowDirectoryEntry(rootPath, entryPath, dirTree, indents);
      continue;
    }
    if (!dirEntry.is_regular_file()) {
      continue;
    }
    if (dirEntry.path().extension() == Rsl::nAssetExtension) {
      ShowAssetEntry(entryPath, entryName, dirTree, indents);
      continue;
    }
    if (dirEntry.path().extension() == World::nLayerExtension) {
      ShowLayerEntry(rootPath, entryPath, entryName, indents);
      continue;
    }
    ShowBasicEntry(entryName, false, indents);
    DragResourceFile(entryPath);
  }

  if (indents > 0) {
    AddIndentLine(lineStartY, lineEndY, indents);
  }
  ImGui::PopID();
}

Result LibraryInterface::ShowDefinedResources(
  const OpenAsset& openAsset, int indents)
{
  // Get the asset's defined resource information.
  const std::string& assetName = openAsset.mName;
  Vlk::Value& assetVal = Rsl::GetConfig(assetName);
  Vlk::Explorer assetEx(assetVal);
  VResult<Ds::Vector<Rsl::Asset::DefResInfo>> result =
    Rsl::Asset::GetAllDefResInfo(assetEx);
  if (!result.Success()) {
    return Result(
      "Asset \"" + assetName + "\" get all defined resource info failed.\n" +
      result.mError);
  }
  Ds::Vector<Rsl::Asset::DefResInfo>& allDefResInfo = result.mValue;

  // Get the currently selected resource if there is one.
  ResId selectedResId;
  ResourceInterface* resourceInterface = FindInterface<ResourceInterface>();
  if (resourceInterface != nullptr) {
    selectedResId = resourceInterface->mResId;
  }

  // Display the defined resources.
  ImGui::PushID(assetName.c_str());
  float lineStartY = ImGui::GetCursorScreenPos().y;
  float lineEndY;
  for (int i = 0; i < allDefResInfo.Size(); ++i) {
    ImGui::PushID(i);
    lineEndY = ImGui::GetCursorScreenPos().y;

    // Display an entry for the resource.
    const Rsl::Asset::DefResInfo& defResInfo = allDefResInfo[i];
    ResId resId(assetName, defResInfo.mName);
    bool selected = selectedResId == resId;
    bool entryClicked = ShowBasicEntry(defResInfo.mName, selected, indents);
    if (entryClicked) {
      if (!selected) {
        OpenInterface<ResourceInterface>(resId);
      }
      else {
        CloseInterface<ResourceInterface>();
      }
    }
    DragResourceId(defResInfo.mTypeId, resId);

    // An option for removing the resource.
    if (ImGui::BeginPopupContextItem()) {
      if (ImGui::Selectable("Remove")) {
        allDefResInfo.Remove(i);
        assetVal.RemoveValue(i);
        Rsl::WriteConfig(assetName);
        --i;
      }
      ImGui::EndPopup();
    }
    ImGui::PopID();

    ImGui::SameLine();
    ShowResourceType(defResInfo.mTypeId);
  }

  if (allDefResInfo.Size() > 0) {
    AddIndentLine(lineStartY, lineEndY, indents);
  }
  ImGui::PopID();
  return Result();
}

Result LibraryInterface::ShowInitializedResources(
  const OpenAsset& openAsset, int indents)
{
  // Ensure that we can get the resource descriptors from an existing asset.
  const std::string& assetName = openAsset.mName;
  const Rsl::Asset* asset = Rsl::TryGetAsset(assetName);
  if (asset == nullptr) {
    return Result("The asset hasn't been added to the library.");
  }
  if (asset->GetStatus() != Rsl::Asset::Status::Live) {
    return Result("The asset isn't Live.");
  }

  // Display all of the initialized resources.
  ImGui::PushID(assetName.c_str());
  float lineStartY = ImGui::GetCursorScreenPos().y;
  float lineEndY;
  const Ds::Vector<Rsl::Asset::ResDesc>& resDescs = asset->GetResDescs();
  for (const Rsl::Asset::ResDesc& resDesc : resDescs) {
    lineEndY = ImGui::GetCursorScreenPos().y;

    ShowBasicEntry(resDesc.mName, false, indents);
    ResId resId(assetName, resDesc.mName);
    DragResourceId(resDesc.mResTypeId, resId);

    ImGui::SameLine();
    ShowResourceType(resDesc.mResTypeId);
  }

  if (resDescs.Size() > 0) {
    AddIndentLine(lineStartY, lineEndY, indents);
  }
  ImGui::PopID();
  return Result();
}

bool LibraryInterface::ShowBasicEntry(
  const std::string& name, bool selected, int indents)
{
  const float entryNameOffset = GetEntryNameOffset(indents);
  const ImGuiStyle& imStyle = ImGui::GetStyle();
  AddEntryLine(entryNameOffset - imStyle.ItemSpacing.x, indents);
  ImGui::SetCursorPosX(ImGui::GetCursorPosX() + entryNameOffset);
  return ImGui::Selectable(name.c_str(), &selected);
}

void LibraryInterface::ShowDirectoryEntry(
  const std::string& rootPath,
  const std::string& path,
  Tree* parentTree,
  int indents)
{
  ImVec2 cursorStartScreenPos = ImGui::GetCursorScreenPos();

  // Display the selectable entry.
  const float entrySymbolOffset = GetEntrySymbolOffset(indents);
  AddEntryLine(entrySymbolOffset, indents);
  std::string entryName = path.substr(path.find_last_of('/') + 1);
  ImGui::SetCursorPosX(ImGui::GetCursorPosX() + GetEntryNameOffset(indents));
  Tree* entryTree;
  if (ImGui::Selectable(entryName.c_str())) {
    entryTree = parentTree->ToggleSubTree(entryName);
  }
  else {
    entryTree = parentTree->TryGetSubTree(entryName);
  }
  AddArrow(entryTree != nullptr, entrySymbolOffset, cursorStartScreenPos);

  if (entryTree != nullptr) {
    ShowDirectory(rootPath, path + '/', entryTree, indents + 1);
  }
}

void LibraryInterface::ShowAssetEntry(
  const std::string& path,
  const std::string& entryName,
  Tree* parentTree,
  int indents)
{
  ImVec2 cursorStartScreenPos = ImGui::GetCursorScreenPos();

  // Display the selectable entry.
  const float entrySymbolOffset = GetEntrySymbolOffset(indents);
  AddEntryLine(entrySymbolOffset, indents);
  std::string assetName = path.substr(0, path.size() - 2);
  ImGui::SetCursorPosX(ImGui::GetCursorPosX() + GetEntryNameOffset(indents));
  OpenAsset* openAsset;
  if (ImGui::Selectable(entryName.c_str())) {
    openAsset = parentTree->ToggleOpenAsset(assetName);
  }
  else {
    openAsset = parentTree->TryGetOpenAsset(assetName);
  }
  bool open = openAsset != nullptr;

  // A popup window for different options.
  Rsl::Asset::Status status = Rsl::GetAssetStatus(assetName);
  if (ImGui::BeginPopupContextItem()) {
    // Initilialze or sleep the asset.
    switch (status) {
    case Rsl::Asset::Status::Dormant:
      if (ImGui::Selectable("Init")) {
        Rsl::QueueAsset(assetName);
      }
      break;
    case Rsl::Asset::Status::Failed:
    case Rsl::Asset::Status::Live:
      if (ImGui::Selectable("Sleep")) {
        Rsl::RemAsset(assetName);
      }
      break;
    case Rsl::Asset::Status::Initializing: ImGui::TextDisabled("Initializing");
    default: break;
    }

    // Add a resource to the asset.
    if (ImGui::Selectable("Create Resource")) {
      VResult<Vlk::Value*> addConfigResult = Rsl::AddConfig(assetName);
      if (addConfigResult.Success()) {
        Vlk::Value newResVal;
        newResVal("Name") = "NewResource";
        newResVal("Type") = "Invalid";
        addConfigResult.mValue->EmplaceValue(std::move(newResVal));
        Rsl::WriteConfig(assetName);
        Rsl::RemConfig(assetName);
      }
      else {
        LogError(addConfigResult.mError.c_str());
      }
    }

    // Switch between defined or initialized resources.
    const char* definedText = "Show Defined Resources";
    const char* initializedText = "Show Initialized Resources";
    if (open && status == Rsl::Asset::Status::Live) {
      if (openAsset->mShowDefinedResources) {
        ImGui::TextDisabled("%s", definedText);
        if (ImGui::Selectable(initializedText)) {
          openAsset->mShowDefinedResources = false;
        }
      }
      else {
        if (ImGui::Selectable(definedText)) {
          openAsset->mShowDefinedResources = true;
        }
        ImGui::TextDisabled("%s", initializedText);
      }
    }
    ImGui::EndPopup();
  }

  // Display the status of the asset.
  ImGui::SameLine();
  ShowStatus(status);

  // Show the list of defined or initialized resources.
  if (open) {
    Result result;
    if (openAsset->mShowDefinedResources) {
      result = ShowDefinedResources(*openAsset, indents + 1);
      if (!result.Success()) {
        std::string error = "Failed to show asset \"" + openAsset->mName +
          "\" defined resources." + result.mError;
        LogError(error.c_str());
      }
    }
    else {
      result = ShowInitializedResources(*openAsset, indents + 1);
      if (!result.Success()) {
        std::string error = "Failed to show asset \"" + openAsset->mName +
          "\" initialized resources." + result.mError;
        LogError(error.c_str());
      }
    }
    if (!result.Success()) {
      parentTree->ToggleOpenAsset(openAsset->mName);
    }
  }
  AddArrow(open, entrySymbolOffset, cursorStartScreenPos);
}

void LibraryInterface::ShowLayerEntry(
  const std::string& rootPath,
  const std::string& path,
  const std::string& entryName,
  int indents)
{
  ShowBasicEntry(entryName, false, indents);
  DragResourceFile(path);
  if (ImGui::BeginPopupContextItem()) {
    if (ImGui::Selectable("Load")) {
      std::string fullPath = rootPath + path;
      VResult<World::LayerIt> result = World::LoadLayer(fullPath.c_str());
      if (result.Success()) {
        nCoreInterface.OpenInterface<LayerInterface>(result.mValue);
      }
      else {
        LogError(result.mError.c_str());
      }
    }
    ImGui::EndPopup();
  }
}

void LibraryInterface::ShowStatus(Rsl::Asset::Status status)
{
  ImGui::PushItemWidth(-1);
  const ImVec4 gray(0.5f, 0.5f, 0.5f, 1.0f);
  const ImVec4 purple(0.5f, 0.0f, 1.0f, 1.0f);
  const ImVec4 orange(1.0, 0.5f, 0.0f, 1.0f);
  const ImVec4 red(1.0f, 0.0f, 0.0f, 1.0f);
  const ImVec4 green(0.0f, 1.0f, 0.0f, 1.0f);
  typedef Rsl::Asset::Status Status;
  switch (status) {
  case Status::Dormant: ImGui::TextColored(gray, "o"); break;
  case Status::Queued: ImGui::TextColored(purple, "~"); break;
  case Status::Initializing: ImGui::TextColored(orange, ">"); break;
  case Status::Failed: ImGui::TextColored(red, "-"); break;
  case Status::Live: ImGui::TextColored(green, "+"); break;
  }
  ImGui::PopItemWidth();
}

void LibraryInterface::ShowResourceType(Rsl::ResTypeId resTypeId)
{
  const Rsl::ResTypeData& resTypeData = Rsl::GetResTypeData(resTypeId);
  std::string resTypeLabel = "(";
  resTypeLabel += resTypeData.mName;
  resTypeLabel += ")";
  ImGui::TextDisabled(resTypeLabel.c_str());
}

float LibraryInterface::GetLineOffset(int indents)
{
  const ImGuiStyle& imStyle = ImGui::GetStyle();
  return (float)(indents - 1) * imStyle.IndentSpacing + mHalfHeight;
}

float LibraryInterface::GetEntryNameOffset(int indents)
{
  const ImGuiStyle& imStyle = ImGui::GetStyle();
  return (float)indents * imStyle.IndentSpacing + mHeight;
}

float LibraryInterface::GetEntrySymbolOffset(int indents)
{
  const ImGuiStyle& imStyle = ImGui::GetStyle();
  return (float)indents * imStyle.IndentSpacing;
}

void LibraryInterface::AddIndentLine(
  float startScreenPosY, float endScreenPosY, int indents)
{
  ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
  const ImGuiStyle& style = ImGui::GetStyle();
  const float lineOffset = GetLineOffset(indents);
  ImVec2 lineStart(cursorScreenPos.x + lineOffset, startScreenPosY - 4.0f);
  ImVec2 lineEnd(lineStart.x, endScreenPosY + mHalfHeight);
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  drawList->AddLine(lineStart, lineEnd, mDecorationColor, mLineThickness);
}

void LibraryInterface::AddEntryLine(float endOffset, int indents)
{
  ImVec2 screenCursorPos = ImGui::GetCursorScreenPos();
  const float lineOffset = GetLineOffset(indents);
  ImVec2 lineStart(
    screenCursorPos.x + lineOffset - 1.0f, screenCursorPos.y + mHalfHeight);
  ImVec2 lineEnd(screenCursorPos.x + endOffset, lineStart.y);
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  drawList->AddLine(lineStart, lineEnd, mDecorationColor, mLineThickness);
}

void LibraryInterface::AddArrow(
  bool open, float symbolOffset, ImVec2 cursorStartScreenPos)
{
  const Vec2* points = mArrowPointsClosed;
  if (open) {
    points = mArrowPointsOpen;
  }
  ImVec2 center(
    cursorStartScreenPos.x + mHalfHeight + symbolOffset,
    cursorStartScreenPos.y + mHalfHeight);
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  drawList->AddTriangleFilled(
    ImVec2(center.x + points[0][0], center.y + points[0][1]),
    ImVec2(center.x + points[1][0], center.y + points[1][1]),
    ImVec2(center.x + points[2][0], center.y + points[2][1]),
    mDecorationColor);
}

} // namespace Editor