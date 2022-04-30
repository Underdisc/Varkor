#include <imgui/imgui.h>

#include "editor/AssetInterfaces.h"
#include "editor/FileInterface.h"
#include "gfx/Font.h."
#include "gfx/Image.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"

namespace Editor {

template<>
void AssetInterface<Gfx::Shader>::EditInitInfo(AssetId id)
{
  Gfx::Shader::InitInfo& info = AssLib::GetAsset<Gfx::Shader>(id).mInitInfo;
  ImGui::Text("Vertex Shader");
  ImGui::SameLine();
  if (ImGui::Button(info.mVertexFile.c_str(), ImVec2(-1, 0))) {
    OpenInterface<FileInterface>(
      [=](const std::string& path) mutable
      {
        info.mVertexFile = path;
        AssLib::TryUpdateInitInfo<Gfx::Shader>(id, info);
      },
      FileInterface::AccessType::Select);
  }

  ImGui::Text("Fragment Shader");
  ImGui::SameLine();
  if (ImGui::Button(info.mFragmentFile.c_str(), ImVec2(-1, 0))) {
    OpenInterface<FileInterface>(
      [=](const std::string& path) mutable
      {
        info.mFragmentFile = path;
        AssLib::TryUpdateInitInfo<Gfx::Shader>(id, info);
      },
      FileInterface::AccessType::Select);
  }
}

template<>
void AssetInterface<Gfx::Model>::EditInitInfo(AssetId id)
{
  Gfx::Model::InitInfo& info = AssLib::GetAsset<Gfx::Model>(id).mInitInfo;
  ImGui::Text("File");
  ImGui::SameLine();
  if (ImGui::Button(info.mFile.c_str(), ImVec2(-1, 0))) {
    OpenInterface<FileInterface>(
      [=](const std::string& path) mutable
      {
        info.mFile = path;
        AssLib::TryUpdateInitInfo<Gfx::Model>(id, info);
      },
      FileInterface::AccessType::Select);
  }
}

template<>
void AssetInterface<Gfx::Image>::EditInitInfo(AssetId id)
{
  Gfx::Image::InitInfo info = AssLib::GetAsset<Gfx::Image>(id).mInitInfo;
  ImGui::Text("File");
  ImGui::SameLine();
  if (ImGui::Button(info.mFile.c_str(), ImVec2(-1, 0))) {
    OpenInterface<FileInterface>(
      [=](const std::string& path) mutable
      {
        info.mFile = path;
        AssLib::TryUpdateInitInfo<Gfx::Image>(id, info);
      },
      FileInterface::AccessType::Select);
  }
}

template<>
void AssetInterface<Gfx::Font>::EditInitInfo(AssetId id)
{
  Gfx::Font::InitInfo& info = AssLib::GetAsset<Gfx::Font>(id).mInitInfo;
  ImGui::Text("File");
  ImGui::SameLine();
  if (ImGui::Button(info.mFile.c_str(), ImVec2(-1, 0))) {
    OpenInterface<FileInterface>(
      [=](const std::string& path) mutable
      {
        info.mFile = path;
        AssLib::TryUpdateInitInfo<Gfx::Font>(id, info);
      },
      FileInterface::AccessType::Select);
  }
}

} // namespace Editor