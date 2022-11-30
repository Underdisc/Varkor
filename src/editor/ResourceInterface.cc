#include <imgui/imgui.h>

#include "editor/ResourceInterface.h"
#include "editor/Utility.h"
#include "gfx/Cubemap.h"
#include "gfx/Font.h"
#include "gfx/Image.h"
#include "gfx/Material.h"
#include "gfx/Mesh.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"
#include "rsl/Library.h"

namespace Editor {

ResourceInterface::ResourceInterface(const ResId& id): mResId(id)
{
  Result result = Rsl::AddConfig(id.GetAssetName());
  if (!result.Success()) {
    LogError(result.mError.c_str());
    mOpen = false;
  }
}

ResourceInterface::~ResourceInterface()
{
  Rsl::RemConfig(mResId.GetAssetName());
}

void ResourceInterface::Show()
{
  ImGui::Begin("Resource", &mOpen);

  // Show the ResourceId centered at the top of the window.
  float windowSize = ImGui::GetWindowSize().x;
  float textSize = ImGui::CalcTextSize(mResId.mId.c_str()).x;
  float startPos = (windowSize - textSize) / 2.0f;
  ImGui::SetCursorPosX(startPos);
  ImGui::Text(mResId.mId.c_str());

  Vlk::Value& assetVal = Rsl::GetConfig(mResId.GetAssetName());
  Vlk::Value assetValCopy = assetVal;

  // Show a dropdown menu for changing the resource type.
  Vlk::Value& resVal = assetVal(mResId.GetResourceName());
  Vlk::Value& typeVal = resVal("Type");
  Vlk::Value& configVal = resVal("Config");
  std::string typeName = typeVal.As<std::string>("Invalid");
  ResTypeId resTypeId = Rsl::GetResTypeId(typeName);
  const ResTypeData& resTypeData = Rsl::GetResTypeData(resTypeId);
  ImGui::PushItemWidth(-Editor::CalcBufferWidth("Type"));
  if (ImGui::BeginCombo("Type", resTypeData.mName)) {
    for (int i = 0; i < (int)ResTypeId::Count; ++i) {
      const ResTypeData& iResTypeData = Rsl::GetResTypeData((ResTypeId)i);
      bool selected = resTypeId == (ResTypeId)i;
      if (ImGui::Selectable(iResTypeData.mName, selected) && !selected) {
        resTypeId = (ResTypeId)i;
        typeVal = iResTypeData.mName;
        configVal.Clear();
      }
    }
    ImGui::EndCombo();
  }
  ImGui::PopItemWidth();

  // Show the menu used to edit the resource config.
  switch (resTypeId) {
  case ResTypeId::Cubemap: Gfx::Cubemap::EditConfig(&configVal); break;
  case ResTypeId::Font: Gfx::Font::EditConfig(&configVal); break;
  case ResTypeId::Image: Gfx::Image::EditConfig(&configVal); break;
  case ResTypeId::Material: Gfx::Material::EditConfig(&configVal); break;
  case ResTypeId::Mesh: Gfx::Mesh::EditConfig(&configVal); break;
  case ResTypeId::Model: Gfx::Model::EditConfig(&configVal); break;
  case ResTypeId::Shader: Gfx::Shader::EditConfig(&configVal); break;
  }
  ImGui::End();

  // If the asset isn't initializing and the Value changed, write it.
  Rsl::Asset& asset = Rsl::GetAsset(mResId.GetAssetName());
  bool initializing = asset.GetStatus() == Rsl::Asset::Status::Initializing;
  if (!initializing && assetVal != assetValCopy) {
    VResult<std::string> result = Rsl::ResolveResPath(asset.GetFile());
    LogAbortIf(!result.Success(), result.mError.c_str());
    assetVal.Write(result.mValue.c_str());
  }
}

} // namespace Editor