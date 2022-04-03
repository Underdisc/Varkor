#include <filesystem>
#include <sstream>

#include "Options.h"

namespace AssetLibrary {

template<typename T>
Asset<T>::Asset(const std::string& name): mName(name), mStatus(Status::Unneeded)
{}

template<typename T>
Result Asset<T>::Init()
{
  // These path checks and conversions are necessary because a path may be
  // relative to the working directory (consider files within vres/) or a
  // project's resource directory.
  Ds::Vector<std::string> paths;
  for (size_t i = 0; i < mPaths.Size(); ++i) {
    if (std::filesystem::exists(mPaths[i])) {
      paths.Push(mPaths[i]);
      continue;
    }
    std::string projectAssetPath = Options::PrependResDirectory(mPaths[i]);
    if (!std::filesystem::exists(projectAssetPath)) {
      std::stringstream error;
      error << "Failed to open \"" << mPaths[i] << "\".";
      return Result(error.str());
    }
    paths.Push(projectAssetPath);
  }
  return mResource.Init(paths);
}

template<typename T>
template<typename... Args>
Result Asset<T>::Init(Args&&... args)
{
  return mResource.Init(args...);
}

template<typename T>
void Asset<T>::Finalize()
{
  mResource.Finalize();
}

template<typename T>
AssetId CreateEmpty(const std::string& name, bool includeId)
{
  AssetId id = AssetBin<T>::NextId();
  std::stringstream ss;
  ss << name;
  if (includeId) {
    ss << id;
  }
  AssetBin<T>::smAssets.Emplace(id, ss.str());
  return id;
}

template<typename T, typename... Args>
AssetId CreateInit(const std::string& name, Args&&... args)
{
  AssetId id = AssetBin<T>::NextId();
  Asset<T>& asset = AssetBin<T>::smAssets.Emplace(id, name);
  Result result = asset.Init(args...);
  LogAbortIf(!result.Success(), result.mError.c_str());
  asset.Finalize();
  asset.mStatus = Status::Live;
  return id;
}

template<typename T>
void Remove(AssetId id)
{
  AssetBin<T>::smAssets.Remove(id);
}

template<typename T>
template<typename... Args>
void AssetBin<T>::Default(Args&&... args)
{
  Asset<T>& defaultAsset = smAssets.Emplace(nDefaultAssetId, "Default");
  Result result = defaultAsset.Init(args...);
  LogAbortIf(!result.Success(), result.mError.c_str());
  defaultAsset.Finalize();
  defaultAsset.mStatus = Status::Live;
}

template<typename T>
template<typename... Args>
AssetId AssetBin<T>::Require(const std::string& name, Args&&... args)
{
  AssetId id = NextRequiredId();
  Asset<T>& newAsset = smAssets.Emplace(id, name);
  Result result = newAsset.Init(args...);
  LogAbortIf(!result.Success(), result.mError.c_str());
  newAsset.Finalize();
  newAsset.mStatus = Status::Live;
  return id;
}

template<typename T>
AssetId AssetBin<T>::NextId()
{
  return smIdHandout++;
}

template<typename T>
AssetId AssetBin<T>::NextRequiredId()
{
  return smRequiredIdHandout--;
}

template<typename T>
T* TryGetLive(AssetId id)
{
  Asset<T>* asset = TryGetAsset<T>(id);
  if (asset == nullptr || asset->mStatus == Status::Failed) {
    return &GetAsset<T>(nDefaultAssetId).mResource;
  }
  if (asset->mStatus == Status::Loading) {
    return nullptr;
  }
  if (asset->mStatus == Status::Unneeded) {
    asset->mStatus = Status::Loading;
    LoadBin<T>::Queue(id);
    return nullptr;
  }
  return &asset->mResource;
}

template<typename T>
Asset<T>& GetAsset(AssetId id)
{
  return AssetBin<T>::smAssets.Get(id);
}

template<typename T>
Asset<T>* TryGetAsset(AssetId id)
{
  return AssetBin<T>::smAssets.Find(id);
}

extern std::thread* nInitThread;
extern bool nStopInitThread;
extern std::mutex nInitQueueMutex;
extern std::mutex nFinalizeQueueMutex;
extern size_t nRemainingInits;
void InitThreadMain();

template<typename T>
void LoadBin<T>::Queue(AssetId id)
{
  nInitQueueMutex.lock();
  smInitQueue.Push(id);
  ++nRemainingInits;
  nInitQueueMutex.unlock();
}

// Finalization happens on the main thread.
template<typename T>
void LoadBin<T>::HandleFinalization()
{
  while (smFinalizeQueue.Size() > 0) {
    AssLib::Asset<T>& asset = GetAsset<T>(smFinalizeQueue[0]);
    asset.Finalize();
    asset.mStatus = Status::Live;
    nFinalizeQueueMutex.lock();
    smFinalizeQueue.Remove(0);
    nFinalizeQueueMutex.unlock();
  }
}

// Initialization happens on the init thread.
template<typename T>
void LoadBin<T>::HandleInitialization()
{
  while (smInitQueue.Size() > 0) {
    if (nStopInitThread) {
      break;
    }

    Asset<T>& asset = GetAsset<T>(smInitQueue[0]);
    Result result = asset.Init();
    if (result.Success()) {
      nFinalizeQueueMutex.lock();
      smFinalizeQueue.Push(smInitQueue[0]);
      nFinalizeQueueMutex.unlock();
    }
    else {
      LogError(result.mError.c_str());
      asset.mStatus = Status::Failed;
    }
    nInitQueueMutex.lock();
    smInitQueue.Remove(0);
    --nRemainingInits;
    nInitQueueMutex.unlock();
  }
}

} // namespace AssetLibrary
