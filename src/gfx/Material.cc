#include <glad/glad.h>

#include "editor/Utility.h"
#include "gfx/Cubemap.h"
#include "gfx/Image.h"
#include "gfx/Material.h"
#include "gfx/Shader.h"
#include "rsl/Library.h"

namespace Gfx {

Material::Material() {}

Material::Material(Material&& other)
{
  *this = std::move(other);
}

Material& Material::operator=(Material&& other)
{
  mShaderId = std::move(other.mShaderId);
  mUniforms = std::move(other.mUniforms);
  return *this;
}

void Material::EditConfig(Vlk::Value* configValP)
{
  // A widget for changing the shader.
  Vlk::Value& configVal = *configValP;
  Vlk::Value& ShaderIdVal = configVal("ShaderId");
  ResId shaderId = ShaderIdVal.As<ResId>(Rsl::GetDefaultResId<Shader>());
  Editor::DropResourceIdWidget(Rsl::ResTypeId::Shader, &shaderId);
  ShaderIdVal = shaderId;

  // A widget for changing the drag speed of uniform widgets.
  static float dragSpeed = 1.0f;
  ImGui::PushItemWidth(-Editor::CalcBufferWidth("Drag Speed"));
  ImGui::DragFloat(
    "Drag Speed",
    &dragSpeed,
    1.0f,
    0.001f,
    100.0f,
    "%.3f",
    ImGuiSliderFlags_Logarithmic);
  ImGui::PopItemWidth();

  // Display a table for modifying uniforms
  Vlk::Value& uniformsVal =
    configVal("Uniforms").EnsureType(Vlk::Value::Type::ValueArray);
  ImGuiTableFlags flags = ImGuiTableFlags_Resizable |
    ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV;
  if (!ImGui::BeginTable("Uniforms", 4, flags)) {
    return;
  }

  // Setup the table header.
  ImGui::TableSetupColumn("Uniform");
  ImGui::TableSetupColumn("Type");
  ImGui::TableSetupColumn("Value");
  ImGui::TableSetupColumn("");

  ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
  for (int i = 0; i < 3; ++i) {
    ImGui::TableSetColumnIndex(i);
    ImGui::TableHeader(ImGui::TableGetColumnName(i));
  }

  // A button for adding new uniforms.
  ImGui::TableSetColumnIndex(3);
  if (ImGui::Button("+", ImVec2(-1.0, 0.0f))) {
    uniformsVal.PushValue();
  }
  ImGui::SameLine();
  ImGui::PushID(2);
  ImGui::TableHeader("");
  ImGui::PopID();

  // Display the table body.
  for (size_t i = 0; i < uniformsVal.Size(); ++i) {
    ImGui::TableNextRow();

    // A text box for changing the uniform name.
    ImGui::TableNextColumn();
    Vlk::Value& uniformVal = uniformsVal[i];
    Vlk::Value& nameVal = uniformVal("Name");
    std::string nameString = nameVal.As<std::string>("uName");
    ImGui::PushID((void*)&nameVal);
    Editor::InputText("Name", &nameString);
    ImGui::PopID();
    nameVal = nameString;

    // A drop down for changing the uniform type.
    ImGui::TableNextColumn();
    Vlk::Value& typeVal = uniformVal("Type");
    const UniformTypeData& floatTypeData =
      GetUniformTypeData(UniformTypeId::Float);
    std::string typeString = typeVal.As<std::string>(floatTypeData.mName);
    UniformTypeId typeId = GetUniformTypeId(typeString);
    ImGui::PushItemWidth(-FLT_MIN);
    ImGui::PushID((void*)&typeVal);
    if (ImGui::BeginCombo("Type", typeString.c_str())) {
      for (int i = 0; i < (int)UniformTypeId::Texture2d; ++i) {
        bool selected = typeId == (UniformTypeId)i;
        const UniformTypeData& typeData = GetUniformTypeData((UniformTypeId)i);
        if (ImGui::Selectable(typeData.mName, selected) && !selected) {
          typeId = (UniformTypeId)i;
          typeVal = typeData.mName;
        }
      }
      ImGui::EndCombo();
    }
    ImGui::PopID();
    ImGui::PopItemWidth();

    // A widget for editing the uniform value that depends on the type.
    ImGui::TableNextColumn();
    Vlk::Value& valueVal = uniformVal("Value");
    ImGui::PushItemWidth(-FLT_MIN);
    ImGui::PushID((void*)&valueVal);
    if (typeId == UniformTypeId::Int) {
      int value = valueVal.As<int>(0);
      ImGui::DragInt("Value", &value, dragSpeed);
      valueVal = value;
    }
    else if (typeId == UniformTypeId::Float) {
      float value = valueVal.As<float>(0.0f);
      ImGui::DragFloat("Value", &value, dragSpeed);
      valueVal = value;
    }
    else if (typeId == UniformTypeId::Vec3) {
      Vec3 value = valueVal.As<Vec3>({0.0f, 0.0f, 0.0f});
      ImGui::DragFloat3("Value", value.mD, dragSpeed);
      valueVal = value;
    }
    else if (typeId == UniformTypeId::Vec4) {
      Vec4 value = valueVal.As<Vec4>({0.0f, 0.0f, 0.0f, 0.0f});
      ImGui::DragFloat4("Value", value.mD, dragSpeed);
      valueVal = value;
    }
    else if (typeId == UniformTypeId::Texture2dRes) {
      ResId imageId = valueVal.As<ResId>(Rsl::GetDefaultResId<Image>());
      Editor::DropResourceIdWidget(
        Rsl::ResTypeId::Image, &imageId, "", FLT_MIN);
      valueVal = imageId;
    }
    else if (typeId == UniformTypeId::TextureCubemapRes) {
      ResId cubemapId = valueVal.As<ResId>(Rsl::GetDefaultResId<Cubemap>());
      Editor::DropResourceIdWidget(
        Rsl::ResTypeId::Cubemap, &cubemapId, "", FLT_MIN);
      valueVal = cubemapId;
    }
    ImGui::PopID();
    ImGui::PopItemWidth();

    // A button for removing the uniform.
    ImGui::TableNextColumn();
    ImGui::PushID((void*)&uniformVal);
    if (ImGui::Button("-", ImVec2(-1.0f, 0.0f))) {
      uniformsVal.RemoveValue(i--);
    }
    ImGui::PopID();
  }

  ImGui::EndTable();
  ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Text("Uniforms");
}

Result Material::Init(const Vlk::Explorer& configEx)
{
  // Get the ShaderId.
  Vlk::Explorer shaderResIdEx = configEx("ShaderId");
  if (!shaderResIdEx.Valid(Vlk::Value::Type::TrueValue)) {
    return Result("Missing :ShaderId: TrueValue.");
  }
  mShaderId = shaderResIdEx.As<ResId>(Rsl::GetDefaultResId<Shader>());

  // Get the uniforms.
  Vlk::Explorer uniformsEx = configEx("Uniforms");
  if (uniformsEx.Valid()) {
    if (!uniformsEx.Valid(Vlk::Value::Type::ValueArray)) {
      return Result(":Uniforms: must be a ValueArray");
    }
    for (size_t i = 0; i < uniformsEx.Size(); ++i) {
      Result initUniformResult = InitUniform(uniformsEx[i]);
      if (!initUniformResult.Success()) {
        return initUniformResult;
      }
    }
  }
  return Result();
}

Result Material::Init(const ResId& shaderResId)
{
  mShaderId = shaderResId;
  return Result();
}

Result Material::Init(Material&& other)
{
  *this = std::move(other);
  return Result();
}

Result Material::Init(
  const ResId& shaderResId,
  const std::string& materialName,
  const aiMaterial& assimpMat,
  const std::string& directory,
  const Ds::Vector<std::string>& embeddedImageNames)
{
  mShaderId = shaderResId;

  // Load textures and create their respective uniforms.
  Rsl::Asset& initAsset = Rsl::Asset::GetInitAsset();
  aiTextureType textureTypes[3] = {
    aiTextureType_DIFFUSE, aiTextureType_SPECULAR, aiTextureType_NORMALS};
  for (int i = 0; i < 3; ++i) {
    aiTextureType aiType = textureTypes[i];
    const char* preUniformName = GetUniformName(aiType);
    unsigned int textureCount = assimpMat.GetTextureCount(aiType);
    for (unsigned int i = 0; i < textureCount; ++i) {
      std::string uniformName = preUniformName;
      if (textureCount > 1) {
        uniformName += '[' + std::to_string(i) + ']';
      }

      // Get the filename where the image is stored.
      aiString filename;
      aiReturn aiResult = assimpMat.Get(AI_MATKEY_TEXTURE(aiType, i), filename);
      if (aiResult != AI_SUCCESS) {
        std::string error =
          "Failed to get filename for \"" + uniformName + "\".";
        return Result(error);
      }

      // Get an initialized image resource name.
      std::string imageResName;
      if (filename.C_Str()[0] == '*') {
        std::string indexString = filename.C_Str();
        indexString.erase(indexString.begin());
        int embeddedIndex = std::stoi(indexString);
        imageResName = embeddedImageNames[embeddedIndex];
      }
      else {
        imageResName = materialName + '(' + uniformName + ')';
        std::string file = directory + filename.C_Str();
        Result result = initAsset.TryInitRes<Image>(imageResName, file);
        if (!result.Success()) {
          result.mError =
            "Image \"" + imageResName + "\" init failed.\n" + result.mError;
          return result;
        }
      }

      // Add the texture uniform.
      ResId imageId(initAsset.GetName(), imageResName);
      mUniforms.Add<ResId>(UniformTypeId::Texture2dRes, uniformName, imageId);
    }
  }

  // Set the diffuse color uniform.
  aiColor3D assimpDiffuseColor;
  aiReturn aiResult =
    assimpMat.Get(AI_MATKEY_COLOR_DIFFUSE, assimpDiffuseColor);
  if (aiResult == AI_SUCCESS) {
    Vec3 diffuseColor;
    diffuseColor[0] = assimpDiffuseColor.r;
    diffuseColor[1] = assimpDiffuseColor.g;
    diffuseColor[2] = assimpDiffuseColor.b;
    mUniforms.Add<Vec3>("uDiffuseColor", diffuseColor);
  }

  return Result();
}

Result Material::InitUniform(const Vlk::Explorer& uniformEx)
{
  Vlk::Explorer nameEx = uniformEx("Name");
  if (!nameEx.Valid(Vlk::Value::Type::TrueValue)) {
    return Result(uniformEx.Path() + " missing :Name: TrueValue");
  }
  std::string uniformName = nameEx.As<std::string>();

  Vlk::Explorer typeEx = uniformEx("Type");
  if (!typeEx.Valid(Vlk::Value::Type::TrueValue)) {
    return Result(uniformEx.Path() + " missing :Type: TrueValue.");
  }
  std::string typeName = typeEx.As<std::string>();
  UniformTypeId typeId = GetUniformTypeId(typeName);
  if (typeId == UniformTypeId::Invalid) {
    return Result(uniformEx.Path() + " type \"" + typeName + "\" is invalid.");
  }

  Vlk::Explorer valueEx = uniformEx("Value");
  if (!valueEx.Valid()) {
    return Result(uniformEx.Path() + " missing :Value:.");
  }
  switch (typeId) {
  case UniformTypeId::Int:
    mUniforms.Add<int>(uniformName, valueEx.As<int>(0));
    break;
  case UniformTypeId::Float:
    mUniforms.Add<float>(uniformName, valueEx.As<float>(0.0f));
    break;
  case UniformTypeId::Vec3:
    mUniforms.Add<Vec3>(uniformName, valueEx.As<Vec3>({0.0f, 0.0f, 0.0f}));
    break;
  case UniformTypeId::Vec4:
    mUniforms.Add<Vec4>(
      uniformName, valueEx.As<Vec4>({0.0f, 0.0f, 0.0f, 0.0f}));
    break;
  case UniformTypeId::Texture2dRes:
    mUniforms.Add<ResId>(
      UniformTypeId::Texture2dRes,
      uniformName,
      valueEx.As<ResId>(Rsl::GetDefaultResId<Image>()));
    break;
  case UniformTypeId::TextureCubemapRes:
    mUniforms.Add<ResId>(
      UniformTypeId::TextureCubemapRes,
      uniformName,
      valueEx.As<ResId>(Rsl::GetDefaultResId<Cubemap>()));
    break;
  default: break;
  }
  return Result();
}

const char* Material::GetUniformName(aiTextureType aiType)
{
  switch (aiType) {
  case aiTextureType_DIFFUSE: return "uDiffuse";
  case aiTextureType_SPECULAR: return "uSpecular";
  case aiTextureType_NORMALS: return "uNormals";
  default: break;
  }
  return "uOther";
}

}; // namespace Gfx