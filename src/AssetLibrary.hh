#include <sstream>

#include "Options.h"

namespace AssetLibrary {

template<typename T>
Asset<T>::Asset(const std::string& name): mName(name), mStatus(Status::Unneeded)
{}

template<typename T>
Result Asset<T>::Init()
{
  return mResource.Init(mInitInfo);
}

template<typename T>
void Asset<T>::Finalize()
{
  mResource.Finalize();
}

template<typename T>
Result Asset<T>::FullInit()
{
  Result result = Init();
  if (!result.Success()) {
    LogError(result.mError.c_str());
    mStatus = Status::Failed;
  }
  else {
    Finalize();
    mStatus = Status::Live;
  }
  return result;
}

template<typename T>
template<typename... Args>
Result Asset<T>::FullInit(Args&&... args)
{
  Result result = mResource.Init(args...);
  if (!result.Success()) {
    LogError(result.mError.c_str());
    mStatus = Status::Failed;
  }
  else {
    Finalize();
    mStatus = Status::Live;
  }
  return result;
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
  Result result = asset.FullInit(args...);
  LogAbortIf(!result.Success(), "Asset failed initialization.");
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
  Result result = defaultAsset.FullInit(args...);
  LogAbortIf(!result.Success(), "Default Asset failed initialization.");
}

template<typename T>
template<typename... Args>
AssetId AssetBin<T>::Require(const std::string& name, Args&&... args)
{
  AssetId id = NextRequiredId();
  Asset<T>& newAsset = smAssets.Emplace(id, name);
  Result result = newAsset.FullInit(args...);
  LogAbortIf(!result.Success(), "Required Asset failed initialization.");
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
T* TryGetLive(AssetId id, AssetId defaultId)
{
  Asset<T>* asset = TryGetAsset<T>(id);
  if (asset == nullptr || asset->mStatus == Status::Failed) {
    return &GetAsset<T>(defaultId).mResource;
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

template<typename T>
void TryUpdateInitInfo(AssetId id, const typename T::InitInfo& info)
{
  Asset<T>* asset = TryGetAsset<T>(id);
  if (asset == nullptr) {
    return;
  }
  asset->mInitInfo = info;
  asset->mResource.Purge();
  asset->mStatus = AssLib::Status::Unneeded;
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
