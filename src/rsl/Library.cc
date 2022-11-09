#include <filesystem>
#include <mutex>
#include <thread>

#include "Options.h"
#include "Result.h"
#include "Viewport.h"
#include "ds/Vector.h"
#include "rsl/Library.h"

namespace Rsl {

Ds::RbTree<Asset> nAssets;

std::thread* nInitThread = nullptr;
bool nStopInitThread = false;
std::mutex nInitQueueMutex;
Ds::Vector<std::string> nInitQueue;
std::mutex nFinalizeQueueMutex;
Ds::Vector<std::string> nFinalizeQueue;

Asset& CreateAsset(const std::string& name)
{
  return nAssets.Emplace(name);
}

void RemoveAsset(const std::string& name)
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

std::string PrependResDirectory(const std::string& path)
{
  return Options::nProjectDirectory + "res/" + path;
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
  std::string error = "Project path \"" + path + "\" failed resolution.";
  return VResult<std::string>(error, "");
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
  std::string error = "Resource path \"" + path + "\" failed resolution.";
  return VResult<std::string>(error, "");
}

void CollectAssets(const std::string& pathString)
{
  VResult<std::string> resolutionResult = ResolveProjPath(pathString);
  LogAbortIf(!resolutionResult.Success(), resolutionResult.mError.c_str());
  const std::string& resolvedPathString = resolutionResult.mValue;

  // Find all assets at the resolved path recursively.
  std::filesystem::path resolvedPath(resolvedPathString);
  std::filesystem::directory_iterator resolvedPathIt(resolvedPath);
  for (const std::filesystem::directory_entry& dirEntry : resolvedPathIt) {
    std::filesystem::path entryPath = dirEntry.path();
    std::string entryPathString =
      pathString + '/' + entryPath.filename().string();
    bool isFile = dirEntry.is_regular_file();
    if (isFile && entryPath.extension().string() == nAssetExtension) {
      size_t extensionStart = entryPathString.find_last_of('.');
      // Assets within res/ do not begin with res/ to remove redundancy.
      if (entryPathString.substr(0, 3) == "res") {
        CreateAsset(entryPathString.substr(4, extensionStart - 4));
      }
      else {
        CreateAsset(entryPathString.substr(0, extensionStart));
      }
    }
    else if (dirEntry.is_directory()) {
      CollectAssets(entryPathString);
    }
  }
}

void Init()
{
  RegisterResourceTypes();
  CollectAssets("vres");
  CollectAssets("res");
  Asset& defaultAsset = GetAsset(nDefaultAssetName);
  defaultAsset.InitFinalize();
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
  Viewport::InitContextSharing();
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
