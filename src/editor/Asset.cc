#include <glad/glad.h>
#include <imgui/imgui.h>
#include <iostream>
#include <sstream>

#include "AssetLibrary.h"
#include "editor/Util.h"

#include "Asset.h"

namespace Editor {

bool nShowShaderSelectWindow = false;
ShaderSelectCallback nShaderSelectCallback = nullptr;
void* nShaderSelectData = nullptr;

void StartShaderSelection(ShaderSelectCallback callback, void* data)
{
  nShowShaderSelectWindow = true;
  nShaderSelectCallback = callback;
  nShaderSelectData = data;
}

void ShaderSelectWindow()
{
  ImGui::Begin("Select Shader", &nShowShaderSelectWindow);
  ImGui::BeginChild("Shaders", ImVec2(0, 0), true);
  auto it = AssetLibrary::nShaders.Begin();
  auto itE = AssetLibrary::nShaders.End();
  while (it != itE)
  {
    if (ImGui::Selectable(it->mValue.mName.c_str()))
    {
      nShaderSelectCallback(it->Key(), nShaderSelectData);
      nShowShaderSelectWindow = false;
      nShaderSelectCallback = nullptr;
      nShaderSelectData = nullptr;
    }
    ++it;
  }
  ImGui::EndChild();
  ImGui::End();
}

bool nShowAssetWindow = false;
AssetLibrary::ShaderId nSelectedShader = AssetLibrary::nInvalidShaderId;
int nEditShaderFileType = 0;

void ChangeShaderCallback(const std::string& path, void* data)
{
  // Update the appropriate shader file for the selected shader.
  AssetLibrary::Shader* shader = AssetLibrary::nShaders.Find(nSelectedShader);
  LogAbortIf(
    shader == nullptr,
    "This cannot be called if the selected shader doesn't exist.");
  if (nEditShaderFileType == GL_VERTEX_SHADER)
  {
    shader->mVertexFile = path;
  } else if (nEditShaderFileType == GL_FRAGMENT_SHADER)
  {
    shader->mFragmentFile = path;
  }
}

void EditShaderWindow()
{
  // Make sure that the selected shader exists.
  AssetLibrary::Shader* shader = AssetLibrary::nShaders.Find(nSelectedShader);
  LogAbortIf(
    shader == nullptr,
    "This cannot be called if the selected shader doesn't exist.");

  // Begin the window and display the name text box and the refresh button.
  bool showWindow = true;
  ImGui::Begin("Edit Shader", &showWindow);
  ImGui::PushItemWidth(-1);
  InputText("Name", &shader->mName);
  if (ImGui::Button("Refresh", ImVec2(-1.0f, 0.0f)))
  {
    Gfx::Shader::InitResult result = shader->mShader.Init(
      shader->mVertexFile.c_str(), shader->mFragmentFile.c_str());
    if (!result.mSuccess)
    {
      OpenPopup("Shader Compilation Error", result.mError.c_str());
    }
  }

  // Display buttons for changing the vertex and fragment files.
  std::stringstream buttonLabel;
  buttonLabel << "Vertex: ";
  if (shader->mVertexFile.empty())
  {
    buttonLabel << "None Selected";
  } else
  {
    buttonLabel << shader->mVertexFile;
  }
  if (ImGui::Button(buttonLabel.str().c_str(), ImVec2(-1.0f, 0.0f)))
  {
    nEditShaderFileType = GL_VERTEX_SHADER;
    StartFileSelection(ChangeShaderCallback);
  }
  buttonLabel.str("");
  buttonLabel << "Fragment: ";
  if (shader->mFragmentFile.empty())
  {
    buttonLabel << "None Selected";
  } else
  {
    buttonLabel << shader->mFragmentFile;
  }
  if (ImGui::Button(buttonLabel.str().c_str(), ImVec2(-1.0f, 0.0f)))
  {
    nEditShaderFileType = GL_FRAGMENT_SHADER;
    StartFileSelection(ChangeShaderCallback);
  }
  ImGui::End();

  // Deselect the shader if the edit shader window was closed.
  if (!showWindow)
  {
    nSelectedShader = AssetLibrary::nInvalidShaderId;
  }
}

void ShowShaders()
{
  if (ImGui::Button("Create Empty Shader", ImVec2(-1, 0)))
  {
    AssetLibrary::CreateEmptyShader();
  }

  // Display all of the shaders within the asset library.
  ImGui::BeginChild("Shaders", ImVec2(-1.0f, 0.0f), true);
  auto it = AssetLibrary::nShaders.Begin();
  auto itE = AssetLibrary::nShaders.End();
  while (it != itE)
  {
    AssetLibrary::Shader& shader = it->mValue;
    std::stringstream label;
    label << it->Key() << ": " << shader.mName;
    if (ImGui::Button(label.str().c_str(), ImVec2(-65.0f, 0.0f)))
    {
      nSelectedShader = it->Key();
    }
    ImGui::SameLine();
    if (shader.mShader.Live())
    {
      ImVec4 green(0.0f, 1.0f, 0.0f, 1.0f);
      ImGui::TextColored(green, "Live");
    } else
    {
      ImVec4 red(1.0f, 0.0f, 0.0f, 1.0f);
      ImGui::TextColored(red, "Not Live");
    }
    ++it;
  }
  ImGui::EndChild();
}

void ShowAssetWindows()
{
  if (nShowAssetWindow)
  {
    ImGui::Begin("Asset", &nShowAssetWindow);
    if (ImGui::CollapsingHeader("Shaders"))
    {
      ShowShaders();
    }
    ImGui::End();

    if (nSelectedShader != AssetLibrary::nInvalidShaderId)
    {
      EditShaderWindow();
    }
  }

  if (nShowShaderSelectWindow)
  {
    ShaderSelectWindow();
  }
}

} // namespace Editor
