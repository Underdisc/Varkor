#ifndef editor_LibraryInterface_h
#define editor_LibraryInterface_h

#include <imgui/imgui.h>

#include "editor/WindowInterface.h"
#include "rsl/Asset.h"

namespace Editor {

struct LibraryInterface: public WindowInterface
{
public:
  LibraryInterface();
  void Show();

private:
  // This can represent an expanded directory or asset.
  struct Tree
  {
    std::string mName;

    // The expanded subdirectories for when the Tree represents a directory.
    Ds::Vector<Tree> mSubTrees;
    // The defined resource information for when the Tree represents an asset.
    Ds::Vector<Rsl::Asset::DefResInfo> mAllDefResInfo;

    void ToggleSubTreeExpansion(const std::string& name);
    Tree* TryGetSubTree(const std::string& name);
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
  void ShowAsset(const std::string& assetName, Tree* assetTree, int indents);
  bool ShowBasicEntry(const std::string& name, bool selected, int indents);
  Tree* ShowExpandableEntry(
    const std::string& name, Tree* parentTree, bool isAsset, int indents);
  void ShowStatus(Rsl::Asset::Status status);

  float GetLineOffset(int indents);
  float GetEntryNameOffset(int indents);
  float GetEntrySymbolOffset(int indents);
  void AddIndentLine(float startScreenPosY, float endScreenPosY, int indents);
  void AddEntryLine(float endOffset, int indents);
};

} // namespace Editor

#endif