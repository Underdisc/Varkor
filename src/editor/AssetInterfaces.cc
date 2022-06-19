#include <imgui/imgui.h>

#include "editor/AssetInterfaces.h"
#include "editor/FileInterface.h"
#include "gfx/Cubemap.h"
#include "gfx/Font.h."
#include "gfx/Image.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"

namespace Editor {

template<>
void AssetInterface<Gfx::Cubemap>::EditInitInfo(AssetId id)
{
  // Options for the different image files.
  Gfx::Cubemap::InitInfo& info = AssLib::GetAsset<Gfx::Cubemap>(id).mInitInfo;
  for (int i = 0; i < 6; ++i) {
    if (ImGui::Button(info.mFiles[i].c_str(), ImVec2(-50, 0))) {
      OpenInterface<FileInterface>(
        [=](const std::string& path) mutable
        {
          info.mFiles[i] = path;
          AssLib::TryUpdateInitInfo<Gfx::Cubemap>(id, info);
        },
        FileInterface::AccessType::Select);
    }
    ImGui::SameLine();
    ImGui::Text(Gfx::Cubemap::InitInfo::smFileDescriptors[i]);
  }

  // Drop down box for the different filter types.
  int currentFilterIndex = info.mFilter - GL_NEAREST;
  int newFilterIndex = currentFilterIndex;
  constexpr auto filterTypes = Gfx::Cubemap::InitInfo::smFilterTypes;
  const int filterCount = sizeof(filterTypes) / sizeof(const char*);
  ImGui::PushItemWidth(-80);
  ImGui::Combo("Filter Type", &newFilterIndex, filterTypes, 2);
  if (newFilterIndex != currentFilterIndex) {
    info.mFilter = newFilterIndex + GL_NEAREST;
    AssLib::TryUpdateInitInfo<Gfx::Cubemap>(id, info);
  }
  ImGui::PopItemWidth();
}

template<>
void AssetInterface<Gfx::Shader>::EditInitInfo(AssetId id)
{
  Gfx::Shader::InitInfo& info = AssLib::GetAsset<Gfx::Shader>(id).mInitInfo;
  int currentScheme = (int)info.mScheme;
  int newScheme = currentScheme;
  constexpr auto schemeStrings = Gfx::Shader::InitInfo::smSchemeStrings;
  const int schemeCount = sizeof(schemeStrings) / sizeof(const char*);
  ImGui::PushItemWidth(-50);
  ImGui::Combo("Scheme", &newScheme, schemeStrings, 2);
  if (newScheme != currentScheme) {
    info.mScheme = (Gfx::Shader::InitInfo::Scheme)newScheme;
    AssLib::TryUpdateInitInfo<Gfx::Shader>(id, info);
  }
  ImGui::PopItemWidth();

  switch (info.mScheme) {
  case Gfx::Shader::InitInfo::Scheme::Single:
    ImGui::Text("File");
    ImGui::SameLine();
    if (ImGui::Button(info.mFile.c_str(), ImVec2(-1, 0))) {
      OpenInterface<FileInterface>(
        [=](const std::string& path) mutable
        {
          info.mFile = path;
          AssLib::TryUpdateInitInfo<Gfx::Shader>(id, info);
        },
        FileInterface::AccessType::Select);
    }
    break;
  case Gfx::Shader::InitInfo::Scheme::Split:
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
    break;
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