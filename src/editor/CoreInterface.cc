#include <imgui/imgui.h>

#include "AssetLibrary.h"
#include "editor/AssetInterfaces.h"
#include "editor/CoreInterface.h"
#include "editor/Editor.h"
#include "editor/FileInterface.h"
#include "editor/FramerInterface.h"
#include "editor/OverviewInterface.h"
#include "editor/Utility.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"
#include "vlk/Explorer.h"
#include "vlk/Pair.h"
#include "world/World.h"

namespace Editor {

CoreInterface::CoreInterface()
{
  OpenInterface<AssetInterface<Gfx::Shader>>();
  OpenInterface<AssetInterface<Gfx::Model>>();
}

void CoreInterface::Show()
{
  ImGui::Begin("Core", nullptr, ImGuiWindowFlags_MenuBar);
  ImGui::BeginMenuBar();
  FileMenu();
  ViewMenu();
  ImGui::EndMenuBar();

  ImGui::Checkbox("Editor Mode", &nEditorMode);

  // Display a button for space creation and display all of the existing spaces.
  if (ImGui::Button("Create Space", ImVec2(-1, 0)))
  {
    World::CreateSpace();
  }
  ImGui::BeginChild("Spaces", ImVec2(0, 0), true);
  World::SpaceId activeSpaceId = World::nInvalidSpaceId;
  OverviewInterface* overview = FindInterface<OverviewInterface>();
  if (overview != nullptr)
  {
    activeSpaceId = overview->mSpaceId;
  }
  for (World::SpaceVisitor visitor; !visitor.End(); visitor.Next())
  {
    World::Space& space = visitor.CurrentSpace();
    bool selected = activeSpaceId == visitor.CurrentSpaceId();
    ImGui::PushID(visitor.CurrentSpaceId());
    if (ImGui::Selectable(space.mName.c_str(), selected))
    {
      if (!selected)
      {
        OpenInterface<OverviewInterface>(visitor.CurrentSpaceId());
      } else
      {
        CloseInterface<OverviewInterface>();
      }
    }
    if (selected)
    {
      ImGui::PushItemWidth(-1);
      InputText("Name", &space.mName);
      ImGui::PopItemWidth();
    }
    ImGui::PopID();
  }
  ImGui::EndChild();
  ImGui::End();
}

void CoreInterface::FileMenu()
{
  if (!ImGui::BeginMenu("File"))
  {
    return;
  }

  // Allow a user to load a Space from file.
  if (ImGui::MenuItem("Load Space"))
  {
    OpenInterface<FileInterface>(
      [this](const std::string& filename)
      {
        Vlk::Pair rootVlk;
        Util::Result result = rootVlk.Read(filename.c_str());
        if (!result.Success())
        {
          OpenInterface<PopupInterface>("Load Space Failed", result.mError);
          LogError(result.mError.c_str());
          return;
        }
        World::SpaceId newSpaceId = World::CreateSpace();
        World::Space& newSpace = World::GetSpace(newSpaceId);
        newSpace.Deserialize(rootVlk);
        OpenInterface<OverviewInterface>(newSpaceId);
      },
      FileInterface::AccessType::Select);
  }

  // Allow a user to save a Space to file if one is selected.
  bool spaceSelected = false;
  OverviewInterface* overview = FindInterface<OverviewInterface>();
  if (overview != nullptr)
  {
    spaceSelected = overview->mSpaceId != World::nInvalidSpaceId;
  }
  if (ImGui::MenuItem("Save Selected Space", nullptr, false, spaceSelected))
  {
    const World::Space& space = World::GetSpace(overview->mSpaceId);
    OpenInterface<FileInterface>(
      [this, &space](const std::string& filename)
      {
        Vlk::Pair rootVlk(space.mName);
        space.Serialize(rootVlk);
        Util::Result result = rootVlk.Write(filename.c_str());
        if (!result.Success())
        {
          OpenInterface<PopupInterface>("Save Space Failed", result.mError);
          LogError(result.mError.c_str());
        }
      },
      FileInterface::AccessType::Save,
      space.mName + ".vlk");
  }

  if (ImGui::MenuItem("Save Assets File"))
  {
    AssetLibrary::SaveAssets();
  }
  if (ImGui::MenuItem("Save Components File"))
  {
    Comp::SaveComponentsFile();
  }
  ImGui::EndMenu();
}

void CoreInterface::ViewMenu()
{
  if (!ImGui::BeginMenu("View"))
  {
    return;
  }

  if (ImGui::MenuItem("Framer"))
  {
    OpenInterface<FramerInterface>();
  }
  if (ImGui::BeginMenu("Assets"))
  {
    if (ImGui::MenuItem("Shaders"))
    {
      OpenInterface<AssetInterface<Gfx::Shader>>();
    }
    if (ImGui::MenuItem("Models"))
    {
      OpenInterface<AssetInterface<Gfx::Model>>();
    }
    ImGui::EndMenu();
  }
  ImGui::EndMenu();
}

} // namespace Editor