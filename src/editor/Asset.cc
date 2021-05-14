#include <glad/glad.h>
#include <imgui/imgui.h>
#include <sstream>

#include "AssetLibrary.h"
#include "editor/Util.h"

#include "Asset.h"

namespace Editor {

// Shader Asset Selection //////////////////////////////////////////////////////
bool nShowShaderSelectWindow = false;
AssetSelectCallback nShaderSelectCallback = nullptr;
void* nShaderSelectData = nullptr;

void StartShaderSelection(AssetSelectCallback callback, void* data)
{
  nShowShaderSelectWindow = true;
  nShaderSelectCallback = callback;
  nShaderSelectData = data;
}

void ShaderSelectWindow()
{
  ImGui::Begin("Select Shader", &nShowShaderSelectWindow);
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
  ImGui::End();
}

// Model Asset Selection ///////////////////////////////////////////////////////
bool nShowModelSelectWindow = false;
AssetSelectCallback nModelSelectCallback = nullptr;
void* nModelSelectData = nullptr;

void StartModelSelection(AssetSelectCallback callback, void* data)
{
  nShowModelSelectWindow = true;
  nModelSelectCallback = callback;
  nModelSelectData = data;
}

void ModelSelectWindow()
{
  ImGui::Begin("Select Model", &nShowModelSelectWindow);
  auto it = AssetLibrary::nModels.Begin();
  auto itE = AssetLibrary::nModels.End();
  while (it != itE)
  {
    if (ImGui::Selectable(it->mValue.mName.c_str()))
    {
      nModelSelectCallback(it->Key(), nModelSelectData);
      nShowModelSelectWindow = false;
      nModelSelectCallback = nullptr;
      nModelSelectData = nullptr;
    }
    ++it;
  }
  ImGui::End();
}

// Asset Windows ///////////////////////////////////////////////////////////////
// Shader //////////////////////////////////////////////////////////////////////
AssetLibrary::AssetId nSelectedShader = AssetLibrary::nInvalidAssetId;
void ChangeShaderCallback(const std::string& path, void* data)
{
  // Update the appropriate shader file for the selected shader.
  AssetLibrary::ShaderAsset* shaderAsset =
    AssetLibrary::nShaders.Find(nSelectedShader);
  LogAbortIf(shaderAsset == nullptr, "The selected shader asset must exist");
  int shaderType = (int)(long long int)data;
  if (shaderType == GL_VERTEX_SHADER)
  {
    shaderAsset->mVertexFile = path;
  } else if (shaderType == GL_FRAGMENT_SHADER)
  {
    shaderAsset->mFragmentFile = path;
  }
}

void EditShaderWindow()
{
  // Make sure that the selected shader exists.
  AssetLibrary::ShaderAsset* shaderAsset =
    AssetLibrary::nShaders.Find(nSelectedShader);
  LogAbortIf(shaderAsset == nullptr, "The selected shader asset must exist");

  // Begin the window and display the name text box and the refresh button.
  bool showWindow = true;
  ImGui::Begin("Edit Shader", &showWindow);
  ImGui::PushItemWidth(-1);
  InputText("Name", &shaderAsset->mName);
  if (ImGui::Button("Refresh", ImVec2(-1.0f, 0.0f)))
  {
    Gfx::Shader::InitResult result = shaderAsset->mShader.Init(
      shaderAsset->mVertexFile.c_str(), shaderAsset->mFragmentFile.c_str());
    if (!result.mSuccess)
    {
      OpenPopup("Shader Compilation Error", result.mError.c_str());
    }
  }

  // Display buttons for changing the vertex and fragment files.
  std::stringstream buttonLabel;
  buttonLabel << "Vertex: ";
  if (shaderAsset->mVertexFile.empty())
  {
    buttonLabel << "None Selected";
  } else
  {
    buttonLabel << shaderAsset->mVertexFile;
  }
  if (ImGui::Button(buttonLabel.str().c_str(), ImVec2(-1.0f, 0.0f)))
  {
    StartFileSelection(ChangeShaderCallback, (void*)GL_VERTEX_SHADER);
  }
  buttonLabel.str("");
  buttonLabel << "Fragment: ";
  if (shaderAsset->mFragmentFile.empty())
  {
    buttonLabel << "None Selected";
  } else
  {
    buttonLabel << shaderAsset->mFragmentFile;
  }
  if (ImGui::Button(buttonLabel.str().c_str(), ImVec2(-1.0f, 0.0f)))
  {
    StartFileSelection(ChangeShaderCallback, (void*)GL_FRAGMENT_SHADER);
  }
  ImGui::End();

  // Deselect the shader asset if the edit shader window was closed.
  if (!showWindow)
  {
    nSelectedShader = AssetLibrary::nInvalidAssetId;
  }
}

void ShowShaders()
{
  if (ImGui::Button("Create Shader", ImVec2(-1, 0)))
  {
    AssetLibrary::CreateEmptyShader();
  }

  // Display all of the shaders within the asset library.
  ImGui::BeginChild("Shaders", ImVec2(-1.0f, 0.0f), true);
  auto it = AssetLibrary::nShaders.Begin();
  auto itE = AssetLibrary::nShaders.End();
  while (it != itE)
  {
    AssetLibrary::ShaderAsset& shaderAsset = it->mValue;
    std::stringstream label;
    label << it->Key() << ": " << shaderAsset.mName;
    if (ImGui::Button(label.str().c_str(), ImVec2(-65.0f, 0.0f)))
    {
      nSelectedShader = it->Key();
    }
    ImGui::SameLine();
    if (shaderAsset.mShader.Live())
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

// Model ///////////////////////////////////////////////////////////////////////
AssetLibrary::AssetId nSelectedModel = AssetLibrary::nInvalidAssetId;
void ChangeModelAssetCallback(const std::string& path, void* data)
{
  AssetLibrary::ModelAsset* modelAsset =
    AssetLibrary::nModels.Find(nSelectedModel);
  LogAbortIf(modelAsset == nullptr, "The selected model asset must exist.");
  Gfx::Model::InitResult result = modelAsset->mModel.Init(path);
  if (!result.mSuccess)
  {
    OpenPopup("Model Import Error", result.mError.c_str());
  }
  modelAsset->mPath = path;
}

void EditModelWindow()
{
  AssetLibrary::ModelAsset* modelAsset =
    AssetLibrary::nModels.Find(nSelectedModel);
  LogAbortIf(modelAsset == nullptr, "The selected model asset must exist.");

  bool showWindow = true;
  ImGui::Begin("Edit Model", &showWindow);
  ImGui::PushItemWidth(-1);
  InputText("Name", &modelAsset->mName);
  std::stringstream buttonLabel;
  buttonLabel << "Path: ";
  if (modelAsset->mPath.empty())
  {
    buttonLabel << "None";
  } else
  {
    buttonLabel << modelAsset->mPath;
  }
  if (ImGui::Button(buttonLabel.str().c_str(), ImVec2(-1.0f, 0.0f)))
  {
    StartFileSelection(ChangeModelAssetCallback);
  }
  ImGui::End();

  if (!showWindow)
  {
    nSelectedModel = AssetLibrary::nInvalidAssetId;
  }
}

void ShowModels()
{
  if (ImGui::Button("Create Model", ImVec2(-1, 0)))
  {
    AssetLibrary::CreateEmptyModel();
  }

  // Display all of the asset library's models.
  ImGui::BeginChild("Models", ImVec2(-1.0f, 0.0f), true);
  auto it = AssetLibrary::nModels.Begin();
  auto itE = AssetLibrary::nModels.End();
  while (it != itE)
  {
    AssetLibrary::ModelAsset& modelAsset = it->mValue;
    std::stringstream label;
    label << it->Key() << ": " << modelAsset.mName;
    if (ImGui::Button(label.str().c_str(), ImVec2(-1.0f, 0.0f)))
    {
      nSelectedModel = it->Key();
    }
    ++it;
  }
  ImGui::EndChild();
}

bool nShowAssetWindow = false;
void ShowAssetWindows()
{
  if (nShowAssetWindow)
  {
    ImGui::Begin("Asset", &nShowAssetWindow);
    if (ImGui::CollapsingHeader("Shaders"))
    {
      ShowShaders();
    }
    if (ImGui::CollapsingHeader("Models"))
    {
      ShowModels();
    }
    ImGui::End();

    if (nSelectedShader != AssetLibrary::nInvalidAssetId)
    {
      EditShaderWindow();
    }
    if (nSelectedModel != AssetLibrary::nInvalidAssetId)
    {
      EditModelWindow();
    }
  }

  if (nShowShaderSelectWindow)
  {
    ShaderSelectWindow();
  }
  if (nShowModelSelectWindow)
  {
    ModelSelectWindow();
  }
}

} // namespace Editor
