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

void EditShader(AssetId assetId)
{
  AssetLibrary::ShaderAsset* shaderAsset = AssetLibrary::nShaders.Find(assetId);
  LogAbortIf(shaderAsset == nullptr, "Shader asset does not exist.");

  // Display the name text box and the refresh button.
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
}

void ShowShaders()
{
  if (ImGui::Button("Create Shader", ImVec2(-1, 0)))
  {
    AssetLibrary::CreateEmptyShader();
  }

  // Display all of the shaders within the asset library.
  ImGui::BeginChild("ShaderList", ImVec2(-1, -1), true);
  auto it = AssetLibrary::nShaders.Begin();
  auto itE = AssetLibrary::nShaders.End();
  AssetId nextShader = nSelectedShader;
  while (it != itE)
  {
    AssetLibrary::ShaderAsset& shaderAsset = it->mValue;
    ImGui::PushItemWidth(-1);
    if (shaderAsset.mShader.Live())
    {
      ImVec4 green(0.0f, 1.0f, 0.0f, 1.0f);
      ImGui::TextColored(green, "+");
    } else
    {
      ImVec4 red(1.0f, 0.0f, 0.0f, 1.0f);
      ImGui::TextColored(red, "-");
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();

    bool selected = it->Key() == nSelectedShader;
    ImGui::PushID(it->Key());
    if (ImGui::Selectable(shaderAsset.mName.c_str(), selected))
    {
      if (nSelectedShader == it->Key())
      {
        nextShader = AssetLibrary::nInvalidAssetId;
      } else
      {
        nextShader = it->Key();
      }
    }
    ImGui::PopID();
    if (selected)
    {
      EditShader(nSelectedShader);
    }
    ++it;
  }
  nSelectedShader = nextShader;
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

void EditModel(AssetId assetId)
{
  AssetLibrary::ModelAsset* modelAsset = AssetLibrary::nModels.Find(assetId);
  LogAbortIf(modelAsset == nullptr, "Model asset does not exist.");

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
}

void ShowModels()
{
  if (ImGui::Button("Create Model", ImVec2(-1, 0)))
  {
    AssetLibrary::CreateEmptyModel();
  }

  // Display all of the asset library's models.
  ImGui::BeginChild("ModelList", ImVec2(-1, -1), true);
  auto it = AssetLibrary::nModels.Begin();
  auto itE = AssetLibrary::nModels.End();
  AssetId nextModel = nSelectedModel;
  while (it != itE)
  {
    AssetLibrary::ModelAsset& modelAsset = it->mValue;
    bool selected = it->Key() == nSelectedModel;
    ImGui::PushID(it->Key());
    if (ImGui::Selectable(modelAsset.mName.c_str(), selected))
    {
      if (nSelectedModel == it->Key())
      {
        nextModel = AssetLibrary::nInvalidAssetId;
      } else
      {
        nextModel = it->Key();
      }
    }
    ImGui::PopID();
    if (selected)
    {
      EditModel(nSelectedModel);
    }
    ++it;
  }
  nSelectedModel = nextModel;
  ImGui::EndChild();
}

bool nShowAssetWindow = false;
void ShowAssetWindows()
{
  if (nShowAssetWindow)
  {
    ImGui::Begin("Asset", &nShowAssetWindow);
    ImVec2 childSize(ImGui::GetWindowContentRegionWidth() * 0.5f, -1);
    ImGui::BeginChild("Shader", childSize, true);
    ShowShaders();
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("Model", childSize, true);
    ShowModels();
    ImGui::EndChild();
    ImGui::End();
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
