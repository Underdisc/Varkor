#include <filesystem>
#include <mutex>
#include <thread>

#include "Options.h"
#include "Result.h"
#include "Viewport.h"
#include "ds/Map.h"
#include "ds/Vector.h"
#include "rsl/Library.h"

namespace Rsl {

struct SharedConfig
{
  Vlk::Value mConfig;
  int mRefCount;
};

Ds::RbTree<Asset> nAssets;
Ds::Map<std::string, SharedConfig> nSharedConfigs;

std::thread* nInitThread = nullptr;
bool nStopInitThread = false;
std::mutex nInitQueueMutex;
Ds::Vector<std::string> nInitQueue;
std::mutex nFinalizeQueueMutex;
Ds::Vector<std::string> nFinalizeQueue;

Asset& AddAsset(const std::string& name)
{
  return nAssets.Emplace(name);
}

Asset& QueueAsset(const std::string& name)
{
  Asset& asset = AddAsset(name);
  asset.QueueInit();
  return asset;
}

Asset& RequireAsset(const std::string& name)
{
  Asset& asset = AddAsset(name);
  asset.InitFinalize();
  return asset;
}

void RemAsset(const std::string& name)
{
  nAssets.Remove(name);
}

Asset& GetAsset(const std::string& name)
{
  return nAssets.Get(name);
}

Asset* TryGetAsset(const std::string& name)
{
  return nAssets.TryGet(name);
}

Asset::Status GetAssetStatus(const std::string& name)
{
  Asset* asset = TryGetAsset(name);
  if (asset != nullptr) {
    return asset->GetStatus();
  }
  return Asset::Status::Dormant;
}

VResult<Vlk::Value*> AddConfig(const std::string& assetName)
{
  // Try to get the existing config.
  SharedConfig* sharedConfig = nSharedConfigs.TryGet(assetName);
  if (sharedConfig != nullptr) {
    ++sharedConfig->mRefCount;
    return &sharedConfig->mConfig;
  }

  // Resolve the asset file.
  std::string file = assetName + nAssetExtension;
  VResult<std::string> resolutionResult = ResolveResPath(file);
  if (!resolutionResult.Success()) {
    return Result(
      "Asset \"" + assetName + "\" add config failed.\n" +
      resolutionResult.mError);
  }
  const std::string& resolvedFile = resolutionResult.mValue;

  // Load the new config.
  SharedConfig newSharedConfig;
  Result readResult = newSharedConfig.mConfig.Read(resolvedFile.c_str());
  if (!readResult.Success()) {
    return Result(
      "Asset \"" + assetName + "\" add config failed.\n" + readResult.mError);
  }
  newSharedConfig.mRefCount = 1;
  return &nSharedConfigs.Emplace(assetName, std::move(newSharedConfig)).mConfig;
}

void RemConfig(const std::string& assetName)
{
  SharedConfig& sharedConfig = nSharedConfigs.Get(assetName);
  --sharedConfig.mRefCount;
  if (sharedConfig.mRefCount == 0) {
    nSharedConfigs.Remove(assetName);
  }
}

Vlk::Value& GetConfig(const std::string& assetName)
{
  return nSharedConfigs.Get(assetName).mConfig;
}

void WriteConfig(const std::string& assetName)
{
  Vlk::Value& assetVal = GetConfig(assetName);
  std::string assetFile = assetName + Rsl::nAssetExtension;
  VResult<std::string> result = Rsl::ResolveResPath(assetFile);
  LogAbortIf(!result.Success(), result.mError.c_str());
  assetVal.Write(result.mValue.c_str());
}

bool IsStandalone()
{
  return Options::nProjectDirectory == "";
}

std::string ResDirectory()
{
  return Options::nProjectDirectory + "res";
}

std::string PrependResDirectory(const std::string& path)
{
  return ResDirectory() + '/' + path;
}

// A path can be next to the executable or within a project directory. This
// function will return a path that is guaranteed to reference a directory entry
// in one of these two locations or an error when an entry doesn't exist.
VResult<std::string> ResolveProjPath(const std::string& path)
{
  if (std::filesystem::exists(path)) {
    return VResult<std::string>(path);
  }
  std::string projPath = Options::nProjectDirectory + path;
  if (std::filesystem::exists(projPath)) {
    return VResult<std::string>(projPath);
  }
  return Result("Project path \"" + path + "\" failed resolution.");
}

// The same as ResolveProjPath, but for a project's res/ directory.
VResult<std::string> ResolveResPath(const std::string& path)
{
  if (std::filesystem::exists(path)) {
    return VResult<std::string>(path);
  }
  std::string inResPath = PrependResDirectory(path);
  if (std::filesystem::exists(inResPath)) {
    return VResult<std::string>(inResPath);
  }
  return Result("Resource path \"" + path + "\" failed resolution.");
}

void Init()
{
  RegisterResourceTypes();
  RequireAsset(nDefaultAssetName);
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

void AddToInitQueue(const Asset& asset)
{
  if (asset.GetStatus() != Asset::Status::Queued) {
    std::string error =
      "Asset \"" + asset.GetName() + "\" lacks Queued status.";
    LogAbort(error.c_str());
  }
  nInitQueueMutex.lock();
  nInitQueue.Push(asset.GetName());
  nInitQueueMutex.unlock();
}

bool InitThreadOpen()
{
  return nInitThread != nullptr;
}

void InitializationThreadMain()
{
  Viewport::StartContextSharing();
  while (!nInitQueue.Empty()) {
    if (nStopInitThread) {
      break;
    }
    Asset& asset = GetAsset(nInitQueue[0]);
    Result result = asset.TryInit();
    if (!result.Success()) {
      std::string error = "Asset \"" + asset.GetName() +
        "\" initialization failed.\n" + result.mError;
      LogError(error.c_str());
    }
    else {
      nFinalizeQueueMutex.lock();
      nFinalizeQueue.Push(nInitQueue[0]);
      nFinalizeQueueMutex.unlock();
    }
    nInitQueueMutex.lock();
    nInitQueue.Remove(0);
    nInitQueueMutex.unlock();
  }
  Viewport::EndContextSharing();
}

void HandleFinalization()
{
  while (!nFinalizeQueue.Empty()) {
    Asset& asset = GetAsset(nFinalizeQueue[0]);
    asset.Finalize();
    nFinalizeQueueMutex.lock();
    nFinalizeQueue.Remove(0);
    nFinalizeQueueMutex.unlock();
  }
}

void HandleInitialization()
{
  // Create the thread when there are initializations to perform and delete it
  // when there are no initializations left.
  if (!nInitQueue.Empty() && nInitThread == nullptr) {
    nInitThread = alloc std::thread(InitializationThreadMain);
  }
  else if (nInitQueue.Empty() && nInitThread != nullptr) {
    nInitThread->join();
    delete nInitThread;
    nInitThread = nullptr;
  }
  HandleFinalization();
}

} // namespace Rsl
