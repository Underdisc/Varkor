#ifndef editor_LibraryInterface_h
#define editor_LibraryInterface_h

#include <filesystem>
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
  // This represents an expanded entry within the Library tree.
  struct Tree
  {
    std::string mName;
    Ds::Vector<Tree> mSubTrees;
    Tree* ToggleSubTree(const std::string& name);
    Tree* ToggleAssetSubTree(
      const std::string& entryName, const std::string& assetName);
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
  void ShowEntry(
    const std::filesystem::directory_entry& dirEntry,
    const std::string& rootPath,
    const std::string& path,
    Tree* dirTree,
    int indents);
  Result ShowAsset(const std::string& assetName, Tree* assetTree, int indents);
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