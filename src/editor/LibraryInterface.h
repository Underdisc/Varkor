#ifndef editor_LibraryInterface_h
#define editor_LibraryInterface_h

#include <filesystem>
#include <imgui/imgui.h>

#include "editor/Interface.h"
#include "rsl/Asset.h"

namespace Editor {

struct LibraryInterface: public Interface
{
public:
  LibraryInterface();
  void Show();

private:
  // This represents an expanded asset within the Library tree.
  struct OpenAsset
  {
    OpenAsset(const std::string& name);
    std::string mName;
    bool mShowDefinedResources;
  };

  // This represents an expanded directory within the Library tree.
  struct Tree
  {
    Tree(const std::string& name);
    std::string mName;
    Ds::Vector<Tree> mSubTrees;
    Ds::Vector<OpenAsset> mOpenAssets;

    Tree* ToggleSubTree(const std::string& name);
    Tree* TryGetSubTree(const std::string& name);
    OpenAsset* ToggleOpenAsset(const std::string& name);
    OpenAsset* TryGetOpenAsset(const std::string& name);
  };

  Tree mRootTree;
  float mHeight;
  float mHalfHeight;
  ImU32 mDecorationColor;
  float mLineThickness;
  Vec2 mArrowPointsClosed[3];
  Vec2 mArrowPointsOpen[3];

  void ShowDirectory(
    const std::string& rootPath,
    const std::string& path,
    Tree* dirTree,
    int indents);
  Result ShowDefinedResources(const OpenAsset& openAsset, int indents);
  Result ShowInitializedResources(const OpenAsset& openAsset, int indents);

  bool ShowBasicEntry(const std::string& name, bool selected, int indents);
  void ShowDirectoryEntry(
    const std::string& rootPath,
    const std::string& path,
    Tree* parentTree,
    int indents);
  void ShowAssetEntry(
    const std::string& path,
    const std::string& entryName,
    Tree* parentTree,
    int indents);
  void ShowLayerEntry(
    const std::string& rootPath,
    const std::string& path,
    const std::string& entryName,
    int indents);

  void ShowStatus(Rsl::Asset::Status status);
  void ShowResourceType(Rsl::ResTypeId resTypeId);

  float GetLineOffset(int indents);
  float GetEntryNameOffset(int indents);
  float GetEntrySymbolOffset(int indents);
  void AddIndentLine(float startScreenPosY, float endScreenPosY, int indents);
  void AddEntryLine(float endOffset, int indents);
  void AddArrow(bool open, float symbolOffset, ImVec2 cursorStartScreenPos);
};

} // namespace Editor

#endif