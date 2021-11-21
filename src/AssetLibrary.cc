#include <fstream>

#include "AssetLibrary.h"
#include "gfx/Font.h"
#include "gfx/Image.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"
#include "vlk/Valkor.h"

namespace AssetLibrary {

AssetId nColorShaderId;
AssetId nDebugDrawShaderId;
AssetId nDefaultTextShaderId;
AssetId nFramebufferShaderId;
AssetId nMemberIdShaderId;
AssetId nArrowModelId;
AssetId nCubeModelId;
AssetId nScaleModelId;
AssetId nSphereModelId;
AssetId nTorusModelId;

void InitRequiredAssets()
{
  // Initialize all defaults.
  AssetBin<Gfx::Font>::InitDefault("vres/font/novaMono/font.ttf");
  AssetBin<Gfx::Image>::InitDefault("vres/image/questionmarkSquare.png");
  AssetBin<Gfx::Model>::InitDefault("vres/model/questionmarkCube.obj");
  AssetBin<Gfx::Shader>::InitDefault(
    "vres/shader/default.vs", "vres/shader/default.fs");

  // All other required assets.
  nColorShaderId = Require<Gfx::Shader>(
    "Color", "vres/shader/default.vs", "vres/shader/color.fs");
  nDebugDrawShaderId = Require<Gfx::Shader>(
    "DebugDraw", "vres/shader/debugLine.vs", "vres/shader/color.fs");
  nDefaultTextShaderId = Require<Gfx::Shader>(
    "DefaultText", "vres/shader/text.vs", "vres/shader/text.fs");
  nFramebufferShaderId = Require<Gfx::Shader>(
    "Framebuffer", "vres/shader/fullscreen.vs", "vres/shader/fullscreen.fs");
  nMemberIdShaderId = Require<Gfx::Shader>(
    "MemberId", "vres/shader/default.vs", "vres/shader/memberId.fs");
  nArrowModelId = Require<Gfx::Model>("Arrow", "vres/model/arrow.fbx");
  nCubeModelId = Require<Gfx::Model>("Cube", "vres/model/cube.obj");
  nScaleModelId = Require<Gfx::Model>("Scale", "vres/model/scale.fbx");
  nSphereModelId = Require<Gfx::Model>("Sphere", "vres/model/sphere.fbx");
  nTorusModelId = Require<Gfx::Model>("Torus", "vres/model/torus.fbx");
}

template<typename T>
void SerializeAssets(Vlk::Value& rootVal)
{
  std::string resourceTypeName = Util::GetShortTypename<T>();
  Vlk::Value& assetsVal = rootVal(resourceTypeName);
  for (const auto& idAssetPair : AssetBin<T>::smAssets)
  {
    if (IsRequiredId(idAssetPair.Key()))
    {
      continue;
    }
    Vlk::Value& assetVal = assetsVal(idAssetPair.mValue.mName);
    assetVal("Id") = idAssetPair.Key();
    Vlk::Value& pathsVal = assetVal("Paths")[{T::smInitPathCount}];
    for (int i = 0; i < T::smInitPathCount; ++i)
    {
      pathsVal[i] = idAssetPair.mValue.GetPath(i);
    }
  }
}

template<typename T>
void DeserializeAssets(const Vlk::Explorer& rootEx)
{
  std::string resourceTypename = Util::GetShortTypename<T>();
  Vlk::Explorer assetArrayEx = rootEx(resourceTypename);
  if (!assetArrayEx.Valid())
  {
    return;
  }
  for (int i = 0; i < assetArrayEx.Size(); ++i)
  {
    // Ensure we have a valid Id so the new asset can be added to the bin.
    Vlk::Explorer assetEx = assetArrayEx(i);
    AssetId id = assetEx("Id").As<int>(AssLib::nDefaultAssetId);
    if (IsRequiredId(id))
    {
      std::stringstream error;
      error << assetEx.Path() << " asset omitted because it has a required Id.";
      LogError(error.str().c_str());
      continue;
    }
    if (AssetBin<T>::smAssets.Contains(id))
    {
      std::stringstream error;
      error << assetEx.Path()
            << " asset omitted because its Id is already in use by another "
               "asset of the same type.";
      LogError(error.str().c_str());
      continue;
    }

    // Add the asset to the bin.
    Asset<T>& asset = AssetBin<T>::smAssets.Emplace(id, assetEx.Key());
    if (id >= AssetBin<T>::smIdHandout)
    {
      AssetBin<T>::smIdHandout = id + 1;
    }
    Vlk::Explorer pathsEx = assetEx("Paths");
    for (int i = 0; i < T::smInitPathCount; ++i)
    {
      asset.SetPath(i, pathsEx[i].As<std::string>(""));
    }

    // todo: This will only be here temporarily. Assets should initialized in a
    // different thread upon request when they are needed.
    asset.Init();
  }
}

void Init()
{
  InitRequiredAssets();
  LoadAssets();
}

const char* nAssetFile = "assets.vlk";
void LoadAssets()
{
  Vlk::Value rootVal;
  Result result = rootVal.Read(nAssetFile);
  if (!result.Success())
  {
    LogError(result.mError.c_str());
    return;
  }
  Vlk::Explorer rootEx(rootVal);
  DeserializeAssets<Gfx::Font>(rootEx);
  DeserializeAssets<Gfx::Image>(rootEx);
  DeserializeAssets<Gfx::Model>(rootEx);
  DeserializeAssets<Gfx::Shader>(rootEx);
}

void SaveAssets()
{
  Vlk::Value rootVal;
  SerializeAssets<Gfx::Font>(rootVal);
  SerializeAssets<Gfx::Image>(rootVal);
  SerializeAssets<Gfx::Model>(rootVal);
  SerializeAssets<Gfx::Shader>(rootVal);
  Result result = rootVal.Write(nAssetFile);
  LogErrorIf(!result.Success(), result.mError.c_str());
}

bool IsRequiredId(AssetId id)
{
  return id < 1;
}

} // namespace AssetLibrary
