#include <filesystem>
#include <mutex>
#include <thread>

#include "Options.h"
#include "Result.h"
#include "Viewport.h"
#include "ds/Map.h"
#include "ds/Vector.h"
#include "ext/Tracy.h"
#include "rsl/Library.h"

namespace Rsl {

struct SharedConfig {
  Vlk::Value mConfig;
  int mRefCount;
};

std::string nExtraResDirectory;
Ds::RbTree<Asset> nAssets;
Ds::Map<std::string, SharedConfig> nSharedConfigs;

std::thread* nInitThread = nullptr;
bool nStopInitThread = false;
bool nInitThreadRunning = false;
std::mutex nInitQueueMutex;
Ds::Vector<std::string> nInitQueue;
std::mutex nFinalizeQueueMutex;
Ds::Vector<std::string> nFinalizeQueue;

Asset& NewAsset(const std::string& name) {
  VResult<Ds::RbTree<Asset>::Iter> result = nAssets.Emplace(name);
  if (!result.Success()) {
    std::stringstream error;
    error << "Asset \"" << name << "\" already in asset tree.";
    LogAbort(error.str().c_str());
  }
  return *result.mValue;
}

Asset& AddAsset(const std::string& name) {
  Asset& asset = NewAsset(name);
  asset.Finalize();
  return asset;
}

Asset& QueueAsset(const std::string& name) {
  Asset& asset = NewAsset(name);
  asset.QueueInit();
  return asset;
}

Asset& RequireAsset(const std::string& name) {
  Asset& asset = NewAsset(name);
  asset.InitFinalize();
  return asset;
}

void RemAsset(const std::string& name) {
  nAssets.Remove(name);
}

Asset& GetAsset(const std::string& name) {
  return nAssets.Get(name);
}

Asset* TryGetAsset(const std::string& name) {
  return nAssets.TryGet(name);
}

Asset::Status GetAssetStatus(const std::string& name) {
  Asset* asset = TryGetAsset(name);
  if (asset != nullptr) {
    return asset->GetStatus();
  }
  return Asset::Status::Dormant;
}

VResult<Vlk::Value*> AddConfig(const std::string& assetName) {
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

void RemConfig(const std::string& assetName) {
  SharedConfig& sharedConfig = nSharedConfigs.Get(assetName);
  --sharedConfig.mRefCount;
  if (sharedConfig.mRefCount == 0) {
    nSharedConfigs.Remove(assetName);
  }
}

Vlk::Value& GetConfig(const std::string& assetName) {
  return nSharedConfigs.Get(assetName).mConfig;
}

void WriteConfig(const std::string& assetName) {
  Vlk::Value& assetVal = GetConfig(assetName);
  std::string assetFile = assetName + Rsl::nAssetExtension;
  VResult<std::string> result = Rsl::ResolveResPath(assetFile);
  LogAbortIf(!result.Success(), result.mError.c_str());
  assetVal.Write(result.mValue.c_str());
}

bool IsStandalone() {
  return Options::nConfig.mProjectDirectory == "";
}

std::string ResDirectory() {
  return Options::nConfig.mProjectDirectory + "res";
}

std::string PrependResDirectory(const std::string& path) {
  return ResDirectory() + '/' + path;
}

// A resource path can be relative to the executable or relative to one of a set
// of directories. This function will return a path is guaranteed to reference
// an existing entry in one of the possible paths or an error when an entry
// doesn't exist. It's analogous to include directories.
VResult<std::string> ResolveResPath(const std::string& path) {
  std::string testPaths[4] = {
    path,
    VARKOR_WORKING_DIRECTORY + path,
    PrependResDirectory(path),
    nExtraResDirectory + path};
  for (const std::string& testPath: testPaths) {
    if (std::filesystem::exists(testPath)) {
      return VResult<std::string>(testPath);
    }
  }
  return Result("Resource path \"" + path + "\" failed resolution.");
}

void Init() {
  RegisterResourceTypes();
  RequireAsset(nDefaultAssetName);
}

void Purge() {
  if (nInitThread != nullptr) {
    nStopInitThread = true;
    nInitThread->join();
    delete nInitThread;
    nInitThread = nullptr;
    nStopInitThread = false;
  }
  nAssets.Clear();
}

void AddToInitQueue(const Asset& asset) {
  if (asset.GetStatus() != Asset::Status::Queued) {
    std::string error =
      "Asset \"" + asset.GetName() + "\" lacks Queued status.";
    LogAbort(error.c_str());
  }
  nInitQueueMutex.lock();
  nInitQueue.Push(asset.GetName());
  nInitQueueMutex.unlock();
}

bool InitThreadOpen() {
  return nInitThread != nullptr;
}

void InitializationThreadMain() {
  ProfileThread("Init");

  Viewport::StartContextSharing();
  nInitThreadRunning = true;
  while (!nStopInitThread && !nInitQueue.Empty()) {
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
  nInitThreadRunning = false;
}

void HandleFinalization() {
  while (!nFinalizeQueue.Empty()) {
    Asset& asset = GetAsset(nFinalizeQueue[0]);
    asset.Finalize();
    nFinalizeQueueMutex.lock();
    nFinalizeQueue.Remove(0);
    nFinalizeQueueMutex.unlock();
  }
}

void HandleInitialization() {
  // Create the thread when there are initializations to perform and delete it
  // when there are no initializations left.
  if (nInitThread != nullptr && !nInitThreadRunning) {
    nInitThread->join();
    delete nInitThread;
    nInitThread = nullptr;
  }
  if (!nInitQueue.Empty() && nInitThread == nullptr) {
    nInitThread = alloc std::thread(InitializationThreadMain);
  }
  HandleFinalization();
}

} // namespace Rsl
