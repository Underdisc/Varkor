#include <imgui/imgui.h>

#include "Options.h"
#include "editor/CoreInterface.h"
#include "editor/Editor.h"
#include "editor/FileInterface.h"
#include "editor/FramerInterface.h"
#include "editor/LayerInterface.h"
#include "editor/LibraryInterface.h"
#include "editor/LogInterface.h"
#include "editor/TempInterface.h"
#include "editor/Utility.h"
#include "rsl/Library.h"
#include "vlk/Valkor.h"
#include "world/World.h"

namespace Editor {

void CoreInterface::Init()
{
  OpenInterface<LogInterface>();
  OpenInterface<LibraryInterface>();
  mShowImGuiDemo = false;
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

  // Find the currently selected layer.
  ImGui::BeginChild("Layers", ImVec2(0, 0), true);
  World::LayerIt activeLayerIt = World::nLayers.end();
  LayerInterface* layerInterface = FindInterface<LayerInterface>();
  if (layerInterface != nullptr) {
    activeLayerIt = layerInterface->mLayerIt;
  }

  // Display all of the existing layers.
  World::LayerIt it = World::nLayers.begin();
  World::LayerIt itE = World::nLayers.end();
  while (it != itE) {
    World::Layer& layer = *it;
    bool selected = it == activeLayerIt;
    ImGui::PushID((void*)&layer);
    if (ImGui::Selectable(layer.mName.c_str(), selected)) {
      if (!selected) {
        OpenInterface<LayerInterface>(it);
      }
      else {
        CloseInterface<LayerInterface>();
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
        CloseInterface<LayerInterface>();
      }
    }
    it = nextIt;
  }
  ImGui::EndChild();
  ImGui::End();

  if (mShowImGuiDemo) {
    ImGui::ShowDemoWindow(&mShowImGuiDemo);
  }
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
        std::string path = Rsl::PrependResDirectory(filename);
        VResult<World::LayerIt> result = World::LoadLayer(path.c_str());
        if (result.Success()) {
          OpenInterface<LayerInterface>(result.mValue);
        }
        else {
          LogError(result.mError.c_str());
        }
      },
      FileInterface::AccessType::Select);
  }

  // Allow a user to save a Space to file if one is selected.
  bool layerSelected = false;
  LayerInterface* layerInterface = FindInterface<LayerInterface>();
  if (layerInterface != nullptr) {
    layerSelected = true;
  }
  if (ImGui::MenuItem("Save Layer", nullptr, false, layerSelected)) {
    OpenInterface<FileInterface>(
      [layerInterface](const std::string& filename)
      {
        if (layerInterface == nullptr) {
          return;
        }
        std::string path = Rsl::PrependResDirectory(filename);
        Result result =
          World::SaveLayer(layerInterface->mLayerIt, path.c_str());
        LogErrorIf(!result.Success(), result.mError.c_str());
      },
      FileInterface::AccessType::Save,
      layerInterface->mLayerIt->mName + ".vlk");
  }

  bool canSave = !Rsl::IsStandalone();
  if (ImGui::MenuItem("Save Components", nullptr, false, canSave)) {
    Comp::SaveComponentsFile();
  }
  ImGui::EndMenu();
}

void CoreInterface::ViewMenu()
{
  if (ImGui::BeginMenu("View")) {
    InterfaceMenuItem<LogInterface>("Log");
    InterfaceMenuItem<FramerInterface>("Framer");
    InterfaceMenuItem<LibraryInterface>("Library");
    InterfaceMenuItem<TempInterface>("Temp");
    ImGui::MenuItem("Demo", NULL, &mShowImGuiDemo);
    ImGui::EndMenu();
  }
}

template<typename T>
void CoreInterface::InterfaceMenuItem(const char* label)
{
  T* interface = FindInterface<T>();
  bool open = interface != nullptr;
  if (ImGui::MenuItem(label, nullptr, &open)) {
    if (!open) {
      CloseInterface<T>();
    }
    else {
      OpenInterface<T>();
    }
  }
}

} // namespace Editor
