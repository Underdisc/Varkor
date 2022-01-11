#include <fstream>

#include "AssetLibrary.h"
#include "Viewport.h"
#include "gfx/Font.h"
#include "gfx/Image.h"
#include "gfx/Mesh.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"
#include "vlk/Valkor.h"

namespace AssetLibrary {

const int ModelFInfo::smInvalidMeshIndex = -1;

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

std::thread* nInitThread = nullptr;
bool nStopInitThread = false;
size_t nRemainingInits;
std::mutex nInitQueueMutex;
std::mutex nModelFInfoMutex;
Ds::Vector<ModelFInfo> nAllModelFInfo;

void InitRequiredAssets()
{
  // Initialize all defaults.
  AssetBin<Gfx::Font>::InitDefault("vres/font/novaMono/font.ttf");
  AssetBin<Gfx::Image>::InitDefault("vres/image/questionmarkSquare.png");
  AssetBin<Gfx::Model>::InitDefault("vres/model/questionmarkCube.obj");
  AssetBin<Gfx::Shader>::InitDefault(
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
    Asset<T>& asset =
      AssetBin<T>::smAssets.Emplace(id, assetEx.Key(), Status::Unneeded);
    if (id >= AssetBin<T>::smIdHandout)
    {
      AssetBin<T>::smIdHandout = id + 1;
    }
    Vlk::Explorer pathsEx = assetEx("Paths");
    for (int i = 0; i < T::smInitPathCount; ++i)
    {
      asset.SetPath(i, pathsEx[i].As<std::string>(""));
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
  if (nInitThread != nullptr)
  {
    nStopInitThread = true;
    nInitThread->join();
    delete nInitThread;
    nInitThread = nullptr;
    nStopInitThread = false;
  }
}

const char* nAssetFile = "assets.vlk";
void DeserializeAssets()
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

void SerializeAssets()
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

void HandleAssetLoading()
{
  InitBin<Gfx::Font>::AssessInitQueue();
  InitBin<Gfx::Image>::AssessInitQueue();
  InitBin<Gfx::Model>::AssessInitQueue();
  InitBin<Gfx::Shader>::AssessInitQueue();

  if (nAllModelFInfo.Size() == 0)
  {
    return;
  }
  nModelFInfoMutex.lock();
  for (const ModelFInfo& fInfo : nAllModelFInfo)
  {
    Asset<Gfx::Model>& asset = GetAsset<Gfx::Model>(fInfo.mId);
    if (fInfo.mMeshIndex == ModelFInfo::smInvalidMeshIndex)
    {
      asset.mStatus = Status::Live;
    } else
    {
      asset.mResource.FinalizeMesh(fInfo);
    }
  }
  nAllModelFInfo.Clear();
  nModelFInfoMutex.unlock();
}

void AddModelFInfo(const ModelFInfo& fInfo)
{
  nModelFInfoMutex.lock();
  nAllModelFInfo.Push(fInfo);
  nModelFInfoMutex.unlock();
}

void InitThreadMain()
{
  Viewport::InitContextSharing();
  while (nRemainingInits > 0 && !nStopInitThread)
  {
    InitBin<Gfx::Shader>::HandleInitQueue();
    InitBin<Gfx::Font>::HandleInitQueue();
    InitBin<Gfx::Image>::HandleInitQueue();
    InitBin<Gfx::Model>::HandleInitQueue();
  }
}

template<>
template<typename... Args>
void AssetBin<Gfx::Model>::InitDefault(Args&&... args)
{
  Asset<Gfx::Model>& defaultAsset = AssetBin<Gfx::Model>::smAssets.Emplace(
    nDefaultAssetId, "Default", Status::Initializing);
  InitBin<Gfx::Model>::smInitQueue.Push(nDefaultAssetId);
  Result result = defaultAsset.mResource.Init(args...);
  InitBin<Gfx::Model>::smInitQueue.Pop();
  LogAbortIf(!result.Success(), result.mError.c_str());
}

template<>
template<typename... Args>
AssetId AssetBin<Gfx::Model>::Require(const std::string& name, Args&&... args)
{
  AssetId id = AssetBin<Gfx::Model>::NextRequiredId();
  Asset<Gfx::Model>& newAsset =
    AssetBin<Gfx::Model>::smAssets.Emplace(id, name, Status::Initializing);
  InitBin<Gfx::Model>::smInitQueue.Push(id);
  Result result = newAsset.mResource.Init(args...);
  InitBin<Gfx::Model>::smInitQueue.Pop();
  LogAbortIf(!result.Success(), result.mError.c_str());
  return id;
}

template<>
void InitBin<Gfx::Model>::HandleInitQueue()
{
  while (smInitQueue.Size() > 0)
  {
    if (nStopInitThread)
    {
      break;
    }

    AssLib::Asset<Gfx::Model>& asset =
      AssLib::AssetBin<Gfx::Model>::smAssets.Get(CurrentId());
    Result result = asset.Init();
    if (!result.Success())
    {
      LogError(result.mError.c_str());
      asset.mStatus = AssLib::Status::Failed;
    }

    nInitQueueMutex.lock();
    smInitQueue.Remove(0);
    --nRemainingInits;
    nInitQueueMutex.unlock();
  }
}

} // namespace AssetLibrary
