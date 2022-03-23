#include <filesystem>
#include <sstream>

#include "Options.h"

namespace Gfx {
struct Model;
}

namespace AssetLibrary {

template<typename T>
Asset<T>::Asset(const std::string& name, Status status):
  mName(name), mStatus(status)
{}

template<typename T>
Result Asset<T>::Init()
{
  // These path checks and conversions are necessary because a path may be
  // relative to the working directory (consider files within vres/) or a
  // project's resource directory.
  std::string paths[T::smInitPathCount];
  for (int i = 0; i < T::smInitPathCount; ++i)
  {
    if (std::filesystem::exists(mPaths[i]))
    {
      paths[i] = mPaths[i];
      continue;
    }
    std::string projectAssetPath = Options::PrependResDirectory(mPaths[i]);
    if (!std::filesystem::exists(projectAssetPath))
    {
      std::stringstream error;
      error << "Failed to open \"" << mPaths[i] << "\".";
      return Result(error.str());
    }
    paths[i] = projectAssetPath;
  }
  return mResource.Init(paths);
}

template<typename T>
template<typename Path>
void Asset<T>::SetPaths(int currentPathIndex, const Path& path)
{
  mPaths[currentPathIndex] = path;
}

template<typename T>
template<typename Path, typename... RemainingPaths>
void Asset<T>::SetPaths(
  int currentPathIndex,
  const Path& path,
  const RemainingPaths&... remainingPaths)
{
  mPaths[currentPathIndex] = path;
  ++currentPathIndex;
  SetPaths(currentPathIndex, remainingPaths...);
}

template<typename T>
template<typename... Paths>
void Asset<T>::SetPaths(const Paths&... paths)
{
  SetPaths(0, paths...);
}

template<typename T>
void Asset<T>::SetPath(int index, const std::string& newPath)
{
  mPaths[index] = newPath;
  mResource.Purge();
  mStatus = Status::Unneeded;
}

template<typename T>
const std::string& Asset<T>::GetPath(int index) const
{
  return mPaths[index];
}

template<typename T>
AssetId Create(const std::string& name, bool includeId)
{
  AssetId id = AssetBin<T>::NextId();
  std::stringstream ss;
  ss << name;
  if (includeId)
  {
    ss << id;
  }
  AssetBin<T>::smAssets.Emplace(id, ss.str(), Status::Unneeded);
  return id;
}

template<typename T, typename... Paths>
AssetId Create(const std::string& name, const Paths&... paths)
{
  AssetId id = AssetBin<T>::NextId();
  Asset<T>& asset = AssetBin<T>::smAssets.Emplace(id, name, Status::Unneeded);
  asset.SetPaths(paths...);
  return id;
}

template<typename T>
void Remove(AssetId id)
{
  AssetBin<T>::smAssets.Remove(id);
}

template<>
template<typename... Args>
void AssetBin<Gfx::Model>::InitDefault(Args&&... args);
template<typename T>
template<typename... Args>
void AssetBin<T>::InitDefault(Args&&... args)
{
  Asset<T>& defaultAsset =
    AssetBin<T>::smAssets.Emplace(nDefaultAssetId, "Default", Status::Live);
  defaultAsset.SetPaths(args...);
  InitBin<T>::smInitQueue.Push(nDefaultAssetId);
  Result result = defaultAsset.Init();
  InitBin<T>::smInitQueue.Pop();
  LogAbortIf(!result.Success(), result.mError.c_str());
}

template<>
template<typename... Args>
AssetId AssetBin<Gfx::Model>::Require(const std::string& name, Args&&... args);
template<typename T>
template<typename... Args>
AssetId AssetBin<T>::Require(const std::string& name, Args&&... args)
{
  AssetId id = NextRequiredId();
  Asset<T>& newAsset = smAssets.Emplace(id, name, Status::Live);
  newAsset.SetPaths(args...);
  InitBin<T>::smInitQueue.Push(id);
  Result result = newAsset.Init();
  InitBin<T>::smInitQueue.Pop();
  LogAbortIf(!result.Success(), result.mError.c_str());
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
  if (asset == nullptr || asset->mStatus == Status::Failed)
  {
    return &AssetBin<T>::smAssets.Find(nDefaultAssetId)->mResource;
  }
  if (asset->mStatus == Status::Initializing)
  {
    return nullptr;
  }
  if (asset->mStatus == Status::Unneeded)
  {
    asset->mStatus = Status::Initializing;
    InitBin<T>::Queue(id);
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
extern size_t nRemainingInits;
void InitThreadMain();

template<typename T>
AssetId InitBin<T>::CurrentId()
{
  nInitQueueMutex.lock();
  AssetId frontId = smInitQueue[0];
  nInitQueueMutex.unlock();
  return frontId;
}

template<typename T>
void InitBin<T>::Queue(AssetId id)
{
  nInitQueueMutex.lock();
  smInitQueue.Push(id);
  ++nRemainingInits;
  nInitQueueMutex.unlock();
}

template<typename T>
void InitBin<T>::AssessInitQueue()
{
  if (nInitThread == nullptr)
  {
    if (smInitQueue.Size() > 0)
    {
      nInitThread = new std::thread(InitThreadMain);
    }
    return;
  }

  if (nRemainingInits == 0)
  {
    nInitThread->join();
    delete nInitThread;
    nInitThread = nullptr;
  }
}

template<>
void InitBin<Gfx::Model>::HandleInitQueue();
template<typename T>
void InitBin<T>::HandleInitQueue()
{
  while (smInitQueue.Size() > 0)
  {
    if (nStopInitThread)
    {
      break;
    }

    AssLib::Asset<T>& asset = AssLib::AssetBin<T>::smAssets.Get(CurrentId());
    Result result = asset.Init();
    if (result.Success())
    {
      asset.mStatus = Status::Live;
    } else
    {
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
