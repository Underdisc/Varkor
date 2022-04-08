#ifndef AssetLibrary_h
#define AssetLibrary_h

#include <mutex>
#include <string>
#include <thread>

#include "Result.h"
#include "ds/Map.h"
#include "ds/Vector.h"

namespace AssetLibrary {

void Init();
void Purge();
void HandleLoading();
bool InitThreadOpen();
void DeserializeAssets();
void SerializeAssets();

// AssetIds refer to an Asset within an AssetBin. Positive AssetIds refer to
// typical user Assets. Negative AssetIds and 0 refer to required Assets.
typedef int AssetId;
constexpr AssetId nDefaultAssetId = 0;
bool IsRequiredId(AssetId id);

// Assets have a status to indicate whether they are ready for use or not.
enum class Status
{
  Unneeded,
  Loading,
  Failed,
  Live,
};

// Any type can be an Asset, but the type needs to have an Init function that
// takes a vector of paths, a Finalize function, and a Purge function. The
// functions must be defined, but can have empty definitions.
template<typename T>
struct Asset
{
  std::string mName;
  T mResource;
  Status mStatus;
  Ds::Vector<std::string> mPaths;

  Asset(const std::string& name);
  Result Init();
  void Finalize();
  Result FullInit();
  template<typename... Args>
  Result FullInit(Args&&... args);
};

// These functions are the primary interface for the AssetLibrary.
template<typename T>
AssetId CreateEmpty(const std::string& name, bool includeId = false);
template<typename T, typename... Args>
AssetId CreateInit(const std::string& name, Args&&... args);
template<typename T>
void Remove(AssetId id);
template<typename T>
T* TryGetLive(AssetId id);
template<typename T>
Asset<T>& GetAsset(AssetId id);
template<typename T>
Asset<T>* TryGetAsset(AssetId id);

// Every Asset type has a bin that stores all Assets of that type.
template<typename T>
struct AssetBin
{
  static Ds::Map<AssetId, Asset<T>> smAssets;
  template<typename... Args>
  static void Default(Args&&... args);
  template<typename... Args>
  static AssetId Require(const std::string& name, Args&&... args);

  static AssetId smIdHandout;
  static AssetId smRequiredIdHandout;
  static AssetId NextId();
  static AssetId NextRequiredId();
};
template<typename T>
Ds::Map<AssetId, Asset<T>> AssetBin<T>::smAssets;
template<typename T>
AssetId AssetBin<T>::smIdHandout = 1;
template<typename T>
AssetId AssetBin<T>::smRequiredIdHandout = -1;

// Every Asset type has a bin for queueing the ids of Assets that need to be
// initialized on the init thread and finalized.
template<typename T>
struct LoadBin
{
  static Ds::Vector<AssetId> smInitQueue;
  static Ds::Vector<AssetId> smFinalizeQueue;
  static void Queue(AssetId id);
  static void HandleFinalization();
  static void HandleInitialization();
};
template<typename T>
Ds::Vector<AssetId> LoadBin<T>::smInitQueue;
template<typename T>
Ds::Vector<AssetId> LoadBin<T>::smFinalizeQueue;

// All required AssetIds
// Shader
extern AssetId nColorShaderId;
extern AssetId nDebugDrawShaderId;
extern AssetId nDefaultTextShaderId;
extern AssetId nFramebufferShaderId;
extern AssetId nMemberIdShaderId;
// Model
extern AssetId nArrowModelId;
extern AssetId nCubeModelId;
extern AssetId nScaleModelId;
extern AssetId nSphereModelId;
extern AssetId nTorusModelId;

} // namespace AssetLibrary

namespace AssLib = AssetLibrary;
typedef AssetLibrary::AssetId AssetId;

#include "AssetLibrary.hh"

#endif
