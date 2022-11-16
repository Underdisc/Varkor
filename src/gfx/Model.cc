#include <filesystem>
#include <imgui/imgui.h>

#include "editor/Utility.h"
#include "gfx/Image.h"
#include "gfx/Material.h"
#include "gfx/Mesh.h"
#include "gfx/Model.h"
#include "math/Matrix4.h"
#include "rsl/Library.h"
#include "util/Memory.h"

namespace Gfx {

Model::Model() {}

Model::Model(Model&& other)
{
  *this = std::move(other);
}

Model& Model::operator=(Model&& other)
{
  mRenderableDescs = std::move(other.mRenderableDescs);
  mMeshDescs = std::move(other.mMeshDescs);
  mMaterialIds = std::move(other.mMaterialIds);
  return *this;
}

VResult<const aiScene*> Model::Import(
  const std::string& file, Assimp::Importer* importer, bool flipUvs)
{
  // Resolve the file to an absolute path.
  VResult<std::string> resolutionResult = Rsl::ResolveResPath(file);
  if (!resolutionResult.Success()) {
    return Result(resolutionResult.mError);
  }
  const std::string& resolvedFile = resolutionResult.mValue;
  // The importer will crash if the path does not reference a regular file.
  if (!std::filesystem::is_regular_file(resolvedFile)) {
    return Result("\"" + file + "\" is not a regular file.");
  }

  // Import the model.
  importer->SetPropertyInteger(AI_CONFIG_FBX_CONVERT_TO_M, 0);
  unsigned int flags =
    aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_SortByPType;
  if (flipUvs) {
    flags |= aiProcess_FlipUVs;
  }
  const aiScene* scene = importer->ReadFile(resolvedFile, flags);
  bool sceneCreated = scene != nullptr && scene->mRootNode != nullptr;
  if (!sceneCreated || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
    return Result(
      "Failed to import \"" + file + "\".\n" + importer->GetErrorString());
  }
  return VResult<const aiScene*>(scene);
}

void Model::EditConfig(Vlk::Value* configValP)
{
  Mesh::EditConfig(configValP);
  Vlk::Value& configVal = *configValP;
  Vlk::Value& shaderIdVal = configVal("ShaderId");
  ResId shaderId = shaderIdVal.As<ResId>(Rsl::GetDefaultResId<Shader>());
  Editor::DropResourceIdWidget(Rsl::ResTypeId::Shader, &shaderId);
  shaderIdVal = shaderId;
}

Result Model::Init(const Vlk::Explorer& configEx)
{
  // Get the file to import.
  Vlk::Explorer fileEx = configEx("File");
  if (!fileEx.Valid(Vlk::Value::Type::TrueValue)) {
    return Result("Missing :File: TrueValue.");
  }
  std::string file = fileEx.As<std::string>();

  // Import the model.
  bool flipUvs = configEx("FlipUvs").As<bool>(false);
  Assimp::Importer importer;
  VResult<const aiScene*> result = Import(file, &importer, flipUvs);
  if (!result.Success()) {
    return result;
  }
  const aiScene* scene = result.mValue;

  // Get the information needed to create all of the materials.
  Vlk::Explorer shaderIdEx = configEx("ShaderId");
  if (!shaderIdEx.Valid(Vlk::Value::Type::TrueValue)) {
    return Result("Missing :ShaderId: TrueValue.");
  }
  ResId shaderId = shaderIdEx.As<ResId>();
  std::string directory = file.substr(0, file.find_last_of('/') + 1);

  // Create all of the image resources that are embedded in the model.
  Rsl::Asset& initAsset = Rsl::Asset::GetInitAsset();
  Ds::Vector<std::string> embeddedImageNames;
  for (int i = 0; i < (int)scene->mNumTextures; ++i) {
    aiTexture* texture = scene->mTextures[i];
    if (texture->mHeight != 0) {
      return Result("Uncompressed embedded textures not implemented.");
    }

    std::string imageName = "Embedded[" + std::to_string(i) + "]";
    imageName = imageName + "(" + texture->mFilename.C_Str() + ")";
    Result result = initAsset.TryInitRes<Image>(
      imageName, (void*)texture->pcData, texture->mWidth);
    if (!result.Success()) {
      result.mError =
        "Image \"" + imageName + "\" init failed.\n" + result.mError;
      return result;
    }
    embeddedImageNames.Emplace(std::move(imageName));
  }

  // Create all of the materials.
  for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
    const aiMaterial& assimpMat = *scene->mMaterials[i];
    std::string materialName = "Material[" + std::to_string(i) + "](" +
      assimpMat.GetName().C_Str() + ")";
    Result result = initAsset.TryInitRes<Material>(
      materialName,
      shaderId,
      materialName,
      assimpMat,
      directory,
      embeddedImageNames);
    if (!result.Success()) {
      return Result(
        "Material \"" + materialName + "\" init failed.\n" + result.mError);
    }
    mMaterialIds.Emplace(initAsset.GetName(), materialName);
  }

  // Create all of the meshes.
  for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
    const aiMesh& assimpMesh = *scene->mMeshes[i];
    if (assimpMesh.mPrimitiveTypes & (unsigned int)aiPrimitiveType_TRIANGLE) {
      std::string meshName =
        "Mesh[" + std::to_string(i) + "](" + assimpMesh.mName.C_Str() + ")";
      Result result = initAsset.TryInitRes<Gfx::Mesh>(meshName, assimpMesh);
      if (!result.Success()) {
        return Result(
          "Mesh \"" + meshName + "\" init failed.\n" + result.mError);
      }
      MeshDescriptor newMeshDesc;
      newMeshDesc.mMeshId.Init(initAsset.GetName(), meshName);
      newMeshDesc.mMaterialIndex = (size_t)assimpMesh.mMaterialIndex;
      mMeshDescs.Push(std::move(newMeshDesc));
    }
  }

  // Create all of the renderables.
  Mat4 parentTransform;
  Math::Identity(&parentTransform);
  CreateRenderables(*scene->mRootNode, parentTransform);
  return Result();
}

size_t Model::RenderableCount() const
{
  return mRenderableDescs.Size();
}

Renderable Model::GetRenderable(size_t renderableDescIndex) const
{
  const RenderableDescriptor& renderableDesc =
    mRenderableDescs[renderableDescIndex];
  const MeshDescriptor& meshDesc = mMeshDescs[renderableDesc.mMeshIndex];

  Renderable renderable;
  renderable.mTransform = renderableDesc.mTransformation;
  renderable.mMeshId = meshDesc.mMeshId;
  renderable.mMaterialId = mMaterialIds[meshDesc.mMaterialIndex];
  return renderable;
}

void Model::CreateRenderables(
  const aiNode& assimpNode, const Mat4& parentTransform)
{
  Mat4 transformation;
  float* fromTransformation = (float*)&assimpNode.mTransformation.a1;
  float* toTransformation = transformation[0];
  constexpr size_t count = sizeof(Mat4) / sizeof(float);
  Util::CopyAssignRange<float>(fromTransformation, toTransformation, count);
  transformation = parentTransform * transformation;
  for (unsigned int i = 0; i < assimpNode.mNumMeshes; ++i) {
    RenderableDescriptor newRenderableDesc;
    newRenderableDesc.mTransformation = transformation;
    newRenderableDesc.mMeshIndex = assimpNode.mMeshes[i];
    mRenderableDescs.Push(newRenderableDesc);
  }
  for (unsigned int i = 0; i < assimpNode.mNumChildren; ++i) {
    CreateRenderables(*assimpNode.mChildren[i], transformation);
  }
}

} // namespace Gfx