#include <filesystem>
#include <imgui/imgui.h>

#include "editor/AssetInterfaces.h"
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

ValueResult<const aiScene*> Model::Import(
  const std::string& file, Assimp::Importer* importer, bool flipUvs)
{
  // Resolve the file to an absolute path.
  ValueResult<std::string> resolutionResult = Rsl::ResolveResPath(file);
  if (!resolutionResult.Success()) {
    return Result(resolutionResult.mError);
  }
  const std::string& resolvedFile = resolutionResult.mValue;
  // The importer will crash if the path does not reference a regular file.
  if (!std::filesystem::is_regular_file(resolvedFile)) {
    return Result("\"" + file + "\" is not a regular file.");
  }

  // Import the model.
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
  return ValueResult<const aiScene*>(scene);
}

void Model::EditConfig(Vlk::Value* configValP)
{
  Mesh::EditConfig(configValP);
  Vlk::Value& configVal = *configValP;
  Vlk::Value& shaderIdVal = configVal("ShaderId");
  ResId shaderId = shaderIdVal.As<ResId>(Rsl::GetDefaultResId<Shader>());
  Editor::DropResourceWidget<Shader>(&shaderId);
  shaderIdVal = shaderId;
}

ValueResult<Model> Model::Init(Rsl::Asset& asset, const Vlk::Explorer& configEx)
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
  ValueResult<const aiScene*> result = Import(file, &importer, flipUvs);
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

  // Create all of the materials.
  Model newModel;
  for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
    const aiMaterial& assimpMat = *scene->mMaterials[i];
    std::string materialName = assimpMat.GetName().C_Str();
    if (materialName.empty()) {
      materialName = "Material[" + std::to_string(i) + "]";
    }
    ValueResult<Material> result =
      Material::Init(asset, shaderId, materialName, assimpMat, directory);
    asset.InitRes<Material>(materialName, std::move(result.mValue));
    if (!result.Success()) {
      return Result(
        "Material \"" + materialName + "\" init failed.\n" + result.mError);
    }
    newModel.mMaterialIds.Emplace(asset.GetName(), materialName);
  }

  // Create all of the meshes.
  for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
    const aiMesh& assimpMesh = *scene->mMeshes[i];
    if (assimpMesh.mPrimitiveTypes & (unsigned int)aiPrimitiveType_TRIANGLE) {
      std::string meshName = assimpMesh.mName.C_Str();
      Result result = asset.TryInitRes<Gfx::Mesh>(meshName, assimpMesh);
      if (!result.Success()) {
        return Result(
          "Mesh \"" + meshName + "\" init failed.\n" + result.mError);
      }
      MeshDescriptor newMeshDesc;
      newMeshDesc.mMeshId.Init(asset.GetName(), meshName);
      newMeshDesc.mMaterialIndex = (size_t)assimpMesh.mMaterialIndex;
      newModel.mMeshDescs.Push(std::move(newMeshDesc));
    }
  }

  // Create all of the renderables.
  Mat4 parentTransform;
  Math::Identity(&parentTransform);
  newModel.CreateRenderables(*scene->mRootNode, parentTransform);
  return ValueResult<Model>(std::move(newModel));
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