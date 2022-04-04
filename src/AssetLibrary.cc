#include <fstream>

#include "AssetLibrary.h"
#include "Options.h"
#include "Viewport.h"
#include "gfx/Font.h"
#include "gfx/Image.h"
#include "gfx/Mesh.h"
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
  AssetBin<Gfx::Font>::Default("vres/font/novaMono/font.ttf");
  AssetBin<Gfx::Image>::Default("vres/image/questionmarkSquare.png");
  AssetBin<Gfx::Model>::Default("vres/model/questionmarkCube.obj");
  AssetBin<Gfx::Shader>::Default(
    "vres/shader/default.vs", "vres/shader/default.fs");

  // All other required assets.
  nColorShaderId = AssetBin<Gfx::Shader>::Require(
    "Color", "vres/shader/default.vs", "vres/shader/color.fs");
  nDebugDrawShaderId = AssetBin<Gfx::Shader>::Require(
    "DebugDraw", "vres/shader/debugLine.vs", "vres/shader/color.fs");
  nDefaultTextShaderId = AssetBin<Gfx::Shader>::Require(
    "DefaultText", "vres/shader/text.vs", "vres/shader/text.fs");
  nFramebufferShaderId = AssetBin<Gfx::Shader>::Require(
    "Framebuffer", "vres/shader/fullscreen.vs", "vres/shader/fullscreen.fs");
  nMemberIdShaderId = AssetBin<Gfx::Shader>::Require(
    "MemberId", "vres/shader/default.vs", "vres/shader/memberId.fs");
  nArrowModelId =
    AssetBin<Gfx::Model>::Require("Arrow", "vres/model/arrow.obj");
  nCubeModelId = AssetBin<Gfx::Model>::Require("Cube", "vres/model/cube.obj");
  nScaleModelId =
    AssetBin<Gfx::Model>::Require("Scale", "vres/model/scale.obj");
  nSphereModelId =
    AssetBin<Gfx::Model>::Require("Sphere", "vres/model/sphere.obj");
  nTorusModelId =
    AssetBin<Gfx::Model>::Require("Torus", "vres/model/torus.obj");
}

template<typename T>
void SerializeAssets(Vlk::Value& rootVal)
{
  std::string resourceTypeName = Util::GetShortTypename<T>();
  Vlk::Value& assetsVal = rootVal(resourceTypeName);
  for (const auto& idAssetPair : AssetBin<T>::smAssets) {
    // We ignore required assets and assets without any paths.
    if (IsRequiredId(idAssetPair.Key())) {
      continue;
    }
    const Asset<T>& asset = idAssetPair.mValue;
    if (asset.mPaths.Size() == 0) {
      continue;
    }

    Vlk::Value& assetVal = assetsVal(asset.mName);
    assetVal("Id") = idAssetPair.Key();
    Vlk::Value& pathsVal = assetVal("Paths")[{asset.mPaths.Size()}];
    for (size_t i = 0; i < asset.mPaths.Size(); ++i) {
      pathsVal[i] = asset.mPaths[i];
    }
  }
}

template<typename T>
void DeserializeAssets(const Vlk::Explorer& rootEx)
{
  std::string resourceTypename = Util::GetShortTypename<T>();
  Vlk::Explorer assetArrayEx = rootEx(resourceTypename);
  if (!assetArrayEx.Valid()) {
    return;
  }
  for (int i = 0; i < assetArrayEx.Size(); ++i) {
    // Ensure we have a valid Id so the new asset can be added to the bin.
    Vlk::Explorer assetEx = assetArrayEx(i);
    AssetId id = assetEx("Id").As<int>(AssLib::nDefaultAssetId);
    if (IsRequiredId(id)) {
      std::stringstream error;
      error << assetEx.Path() << " asset omitted because it has a required Id.";
      LogError(error.str().c_str());
      continue;
    }
    if (AssetBin<T>::smAssets.Contains(id)) {
      std::stringstream error;
      error << assetEx.Path()
            << " asset omitted because its Id is already in use by another "
               "asset of the same type.";
      LogError(error.str().c_str());
      continue;
    }

    // Add the asset to the bin.
    Asset<T>& asset = AssetBin<T>::smAssets.Emplace(id, assetEx.Key());
    if (id >= AssetBin<T>::smIdHandout) {
      AssetBin<T>::smIdHandout = id + 1;
    }
    Vlk::Explorer pathsEx = assetEx("Paths");
    for (size_t i = 0; i < pathsEx.Size(); ++i) {
      asset.mPaths.Push(pathsEx[i].As<std::string>(""));
    }
  }
}

void Init()
{
  InitRequiredAssets();
  DeserializeAssets();
}

bool InitThreadOpen()
{
  return nInitThread != nullptr;
}

void Purge()
{
  if (nInitThread != nullptr) {
    nStopInitThread = true;
    nInitThread->join();
    delete nInitThread;
    nInitThread = nullptr;
    nStopInitThread = false;
  }
}

const char* nAssetsFilename = "assets.vlk";
void DeserializeAssets()
{
  Vlk::Value rootVal;
  std::string assetsFile = Options::PrependResDirectory(nAssetsFilename);
  Result result = rootVal.Read(assetsFile.c_str());
  if (!result.Success()) {
    LogError(result.mError.c_str());
    return;
  }
  Vlk::Explorer rootEx(rootVal);
  DeserializeAssets<Gfx::Font>(rootEx);
  DeserializeAssets<Gfx::Image>(rootEx);
  DeserializeAssets<Gfx::Model>(rootEx);
  DeserializeAssets<Gfx::Shader>(rootEx);
}

void SerializeAssets()
{
  Vlk::Value rootVal;
  SerializeAssets<Gfx::Font>(rootVal);
  SerializeAssets<Gfx::Image>(rootVal);
  SerializeAssets<Gfx::Model>(rootVal);
  SerializeAssets<Gfx::Shader>(rootVal);
  std::string assetsFile = Options::PrependResDirectory(nAssetsFilename);
  Result result = rootVal.Write(assetsFile.c_str());
  LogErrorIf(!result.Success(), result.mError.c_str());
}

bool IsRequiredId(AssetId id)
{
  return id < 1;
}

// Threaded Asset Loading //////////////////////////////////////////////////////
std::thread* nInitThread = nullptr;
bool nStopInitThread = false;
size_t nRemainingInits;
std::mutex nInitQueueMutex;
std::mutex nFinalizeQueueMutex;

void HandleLoading()
{
  // Create the thread when there are initializations to perform and delete it
  // when there are no initializations left.
  if (nRemainingInits > 0 && nInitThread == nullptr) {
    nInitThread = new std::thread(InitThreadMain);
  }
  else if (nRemainingInits == 0 && nInitThread != nullptr) {
    nInitThread->join();
    delete nInitThread;
    nInitThread = nullptr;
  }

  // Handle the finalization of any assets that have completed initialization.
  LoadBin<Gfx::Font>::HandleFinalization();
  LoadBin<Gfx::Image>::HandleFinalization();
  LoadBin<Gfx::Model>::HandleFinalization();
  LoadBin<Gfx::Shader>::HandleFinalization();
}

void InitThreadMain()
{
  Viewport::InitContextSharing();
  while (nRemainingInits > 0 && !nStopInitThread) {
    LoadBin<Gfx::Shader>::HandleInitialization();
    LoadBin<Gfx::Font>::HandleInitialization();
    LoadBin<Gfx::Image>::HandleInitialization();
    LoadBin<Gfx::Model>::HandleInitialization();
  }
}

} // namespace AssetLibrary
