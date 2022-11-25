#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "Options.h"
#include "editor/AssetInterface.h"
#include "editor/Editor.h"
#include "editor/LayerInterface.h"
#include "editor/LibraryInterface.h"
#include "editor/ResourceInterface.h"
#include "world/World.h"

namespace Editor {

LibraryInterface::LibraryInterface()
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
  Tree* vresTree = ShowExpandableEntry("vres", &mRootTree, false, 0);
  if (vresTree != nullptr) {
    ShowDirectory("", "vres/", vresTree, 1);
  }
  ShowDirectory(Options::nProjectDirectory + "res/", "", &mRootTree, 0);
  ImGui::End();

  ImGui::PopStyleVar();
  ImGui::PopStyleVar();
}

void LibraryInterface::Tree::ToggleSubTreeExpansion(const std::string& name)
{
  for (int i = 0; i < mSubTrees.Size(); ++i) {
    if (mSubTrees[i].mName == name) {
      mSubTrees.Remove(i);
      return;
    }
  }
  mSubTrees.Emplace();
  Tree* newTree = &mSubTrees.Top();
  newTree->mName = name;
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
    ShowEntry(dirEntry, rootPath, path, dirTree, indents);
  }
  if (indents > 0) {
    AddIndentLine(lineStartY, lineEndY, indents);
  }
  ImGui::PopID();
}

void LibraryInterface::ShowEntry(
  const std::filesystem::directory_entry& dirEntry,
  const std::string& rootPath,
  const std::string& path,
  Tree* dirTree,
  int indents)
{
  const std::filesystem::path& entryFullPath = dirEntry.path();
  std::string entryFullPathString = entryFullPath.string();
  size_t entryNameStart = entryFullPathString.find_last_of('/') + 1;
  std::string entryName = entryFullPathString.substr(entryNameStart);
  std::string entryPath = path + entryName;

  if (dirEntry.is_directory()) {
    bool isAsset = false;
    Tree* subdirTree =
      ShowExpandableEntry(entryPath, dirTree, isAsset, indents);
    if (subdirTree != nullptr) {
      ShowDirectory(rootPath, entryPath + '/', subdirTree, indents + 1);
    }
    return;
  }

  if (!dirEntry.is_regular_file()) {
    return;
  }

  if (entryFullPath.extension() == Rsl::nAssetExtension) {
    bool isAsset = true;
    Tree* assetTree = ShowExpandableEntry(entryPath, dirTree, isAsset, indents);
    // A popup window for initializing and sleeping the asset.
    std::string assetName = entryPath.substr(0, entryPath.size() - 2);
    Rsl::Asset& asset = Rsl::GetAsset(assetName);
    if (ImGui::BeginPopupContextItem()) {
      switch (asset.GetStatus()) {
      case Rsl::Asset::Status::Dormant:
        if (ImGui::Selectable("Init")) {
          asset.QueueInit();
        }
        break;
      case Rsl::Asset::Status::Failed:
      case Rsl::Asset::Status::Live:
        if (ImGui::Selectable("Sleep")) {
          asset.Sleep();
        }
        break;
      case Rsl::Asset::Status::Initializing:
        ImGui::Text("Initializing", ImVec2(-1, 0));
      }
      ImGui::EndPopup();
    }
    ImGui::SameLine();
    ShowStatus(asset.GetStatus());
    if (assetTree != nullptr) {
      ShowAsset(assetName, assetTree, indents + 1);
    }
    return;
  }

  if (entryFullPath.extension() == World::nLayerExtension) {
    ShowBasicEntry(entryName, false, indents);
    if (ImGui::BeginPopupContextItem()) {
      if (ImGui::Selectable("Load")) {
        VResult<World::LayerIt> result =
          World::LoadLayer(entryFullPathString.c_str());
        if (result.Success()) {
          nCoreInterface.OpenInterface<LayerInterface>(result.mValue);
        }
        else {
          LogError(result.mError.c_str());
        }
      }
      ImGui::EndPopup();
    }
    return;
  }

  ShowBasicEntry(entryName, false, indents);
}

void LibraryInterface::ShowAsset(
  const std::string& assetName, Tree* assetTree, int indents)
{
  ResId selectedResId;
  ResourceInterface* resourceInterface = FindInterface<ResourceInterface>();
  if (resourceInterface != nullptr) {
    selectedResId = resourceInterface->mResId;
  }

  // Display the asset's defined resources.
  ImGui::PushID(assetTree->mName.c_str());
  float lineStartY = ImGui::GetCursorScreenPos().y;
  float lineEndY;
  for (const Rsl::Asset::DefResInfo& defResInfo : assetTree->mAllDefResInfo) {
    lineEndY = ImGui::GetCursorScreenPos().y;
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
  }
  if (assetTree->mAllDefResInfo.Size() > 0) {
    AddIndentLine(lineStartY, lineEndY, indents);
  }
  ImGui::PopID();
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

LibraryInterface::Tree* LibraryInterface::ShowExpandableEntry(
  const std::string& path, Tree* parentTree, bool isAsset, int indents)
{
  ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();

  // Display the entry name.
  const float entrySymbolOffset = GetEntrySymbolOffset(indents);
  AddEntryLine(entrySymbolOffset, indents);
  std::string entryName = path.substr(path.find_last_of('/') + 1);
  ImGui::SetCursorPosX(ImGui::GetCursorPosX() + GetEntryNameOffset(indents));
  bool clicked = ImGui::Selectable(entryName.c_str());
  if (clicked) {
    parentTree->ToggleSubTreeExpansion(entryName);
  }
  Tree* entryTree = parentTree->TryGetSubTree(entryName);

  // Draw an open or closed arrow to indicate an expandable entry. The
  // selectable comes first because clicking it will change the arrow.
  const Vec2* points = mArrowPointsClosed;
  if (entryTree != nullptr) {
    points = mArrowPointsOpen;
  }
  ImVec2 center(
    cursorScreenPos.x + mHalfHeight + entrySymbolOffset,
    cursorScreenPos.y + mHalfHeight);
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  drawList->AddTriangleFilled(
    ImVec2(center.x + points[0][0], center.y + points[0][1]),
    ImVec2(center.x + points[1][0], center.y + points[1][1]),
    ImVec2(center.x + points[2][0], center.y + points[2][1]),
    mDecorationColor);

  // Exit if we are not expanding an asset.
  if (!clicked || !isAsset || entryTree == nullptr) {
    return entryTree;
  }

  // We need to get an asset's defined resources when an asset is expanded.
  std::string assetName = path.substr(0, path.size() - 2);
  const Rsl::Asset& asset = Rsl::GetAsset(assetName);
  VResult<Ds::Vector<Rsl::Asset::DefResInfo>> result = asset.GetAllDefResInfo();
  if (!result.Success()) {
    std::string error =
      "Asset \"" + assetName + " \" GetAllDefResInfo failed.\n" + result.mError;
    LogError(error.c_str());
    parentTree->ToggleSubTreeExpansion(entryName);
  }
  else {
    entryTree->mAllDefResInfo = std::move(result.mValue);
  }
  return entryTree;
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

} // namespace Editor