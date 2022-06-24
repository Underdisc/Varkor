#include <imgui/imgui.h>

#include "AssetLibrary.h"
#include "editor/AssetInterfaces.h"
#include "editor/CoreInterface.h"
#include "editor/Editor.h"
#include "editor/ErrorInterface.h"
#include "editor/FileInterface.h"
#include "editor/FramerInterface.h"
#include "editor/OverviewInterface.h"
#include "editor/Utility.h"
#include "gfx/Cubemap.h"
#include "gfx/Font.h"
#include "gfx/Image.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"
#include "vlk/Valkor.h"
#include "world/World.h"

namespace Editor {

void CoreInterface::Init()
{
  OpenInterface<ErrorInterface>();
  OpenInterface<AssetInterface<Gfx::Cubemap>>();
  OpenInterface<AssetInterface<Gfx::Font>>();
  OpenInterface<AssetInterface<Gfx::Image>>();
  OpenInterface<AssetInterface<Gfx::Model>>();
  OpenInterface<AssetInterface<Gfx::Shader>>();
}

void CoreInterface::Show()
{
  ImGui::Begin("Core", nullptr, ImGuiWindowFlags_MenuBar);
  ImGui::BeginMenuBar();
  FileMenu();
  ViewMenu();
  ImGui::EndMenuBar();

  ImGui::Checkbox("Editor Mode", &nEditorMode);
  ImGui::Checkbox("Pause World", &World::nPause);

  // Display a button for layer creation.
  if (ImGui::Button("Create Layer", ImVec2(-1, 0))) {
    World::CreateTopLayer();
  }

  // Display all of the existing layers.
  ImGui::BeginChild("Layers", ImVec2(0, 0), true);
  World::LayerIt activeLayerIt = World::nLayers.end();
  OverviewInterface* overview = FindInterface<OverviewInterface>();
  if (overview != nullptr) {
    activeLayerIt = overview->mLayerIt;
  }

  World::LayerIt it = World::nLayers.begin();
  World::LayerIt itE = World::nLayers.end();
  while (it != itE) {
    World::Layer& layer = *it;
    bool selected = it == activeLayerIt;
    ImGui::PushID((void*)&layer);
    if (ImGui::Selectable(layer.mName.c_str(), selected)) {
      if (!selected) {
        OpenInterface<OverviewInterface>(it);
      }
      else {
        CloseInterface<OverviewInterface>();
      }
    }
    ImGui::PopID();

    bool deleteLayer = false;
    if (ImGui::BeginPopupContextItem()) {
      if (ImGui::Selectable("Delete")) {
        deleteLayer = true;
      }
      ImGui::EndPopup();
    }

    if (selected) {
      ImGui::PushItemWidth(-1);
      InputText("Name", &layer.mName);
      ImGui::PopItemWidth();
    }

    World::LayerIt nextIt = it;
    ++nextIt;
    if (deleteLayer) {
      World::DeleteLayer(it);
      if (selected) {
        CloseInterface<OverviewInterface>();
      }
    }
    it = nextIt;
  }
  ImGui::EndChild();
  ImGui::End();
}

void CoreInterface::FileMenu()
{
  if (!ImGui::BeginMenu("File")) {
    return;
  }

  // Allow a user to load a Layer from file.
  if (ImGui::MenuItem("Load Layer")) {
    OpenInterface<FileInterface>(
      [this](const std::string& filename)
      {
        std::string path = AssLib::PrependResDirectory(filename);
        ValueResult<World::LayerIt> result = World::LoadLayer(path.c_str());
        if (result.Success()) {
          OpenInterface<OverviewInterface>(result.mValue);
        }
        else {
          LogError(result.mError.c_str());
        }
      },
      FileInterface::AccessType::Select);
  }

  // Allow a user to save a Space to file if one is selected.
  bool layerSelected = false;
  OverviewInterface* overview = FindInterface<OverviewInterface>();
  if (overview != nullptr) {
    layerSelected = true;
  }
  if (ImGui::MenuItem("Save Layer", nullptr, false, layerSelected)) {
    OpenInterface<FileInterface>(
      [overview](const std::string& filename)
      {
        if (overview == nullptr) {
          return;
        }
        std::string path = AssLib::PrependResDirectory(filename);
        Result result = World::SaveLayer(overview->mLayerIt, path.c_str());
        LogErrorIf(!result.Success(), result.mError.c_str());
      },
      FileInterface::AccessType::Save,
      overview->mLayerIt->mName + ".vlk");
  }
  if (ImGui::MenuItem("Save Assets")) {
    AssetLibrary::SerializeAssets();
  }
  if (ImGui::MenuItem("Save Components")) {
    Comp::SaveComponentsFile();
  }
  ImGui::EndMenu();
}

void CoreInterface::ViewMenu()
{
  if (!ImGui::BeginMenu("View")) {
    return;
  }

  if (ImGui::MenuItem("Error")) {
    OpenInterface<ErrorInterface>();
  }
  if (ImGui::MenuItem("Framer")) {
    OpenInterface<FramerInterface>();
  }
  if (ImGui::BeginMenu("Assets")) {
    if (ImGui::MenuItem("Cubemaps")) {
      OpenInterface<AssetInterface<Gfx::Cubemap>>();
    }
    if (ImGui::MenuItem("Fonts")) {
      OpenInterface<AssetInterface<Gfx::Font>>();
    }
    if (ImGui::MenuItem("Images")) {
      OpenInterface<AssetInterface<Gfx::Image>>();
    }
    if (ImGui::MenuItem("Models")) {
      OpenInterface<AssetInterface<Gfx::Model>>();
    }
    if (ImGui::MenuItem("Shaders")) {
      OpenInterface<AssetInterface<Gfx::Shader>>();
    }
    ImGui::EndMenu();
  }
  ImGui::EndMenu();
}

} // namespace Editor
