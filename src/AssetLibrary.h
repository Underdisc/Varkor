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

std::string PrependResDirectory(const std::string& path);
ValueResult<std::string> ResolveResourcePath(const std::string& path);

// AssetIds refer to an Asset within an AssetBin. Positive AssetIds refer to
// assets that will be serialized. Negative AssetIds and 0 refer to assets that
// won't be serialized.
typedef int AssetId;
constexpr AssetId nDefaultAssetId = 0;
constexpr AssetId nInvalidAssetId = -1;
bool SerializableId(AssetId id);

// Assets have a status to indicate whether they are ready for use or not.
enum class Status
{
  Unneeded,
  Loading,
  Failed,
  Live,
};

// Any type can be an Asset, but the type needs to define these things in order
// to make full use of the AssetLibrary's functionality.
/*
struct InitInfo {
  void Prep(...) {...}
  void Serialize(Vlk::Value&) {...}
  void Deserialize(const Vlk::Explorer&) {...}
};
Result Init(const InitInfo&) {...}
void Finalize() {...}
void Purge() {...}
*/
template<typename T>
struct Asset
{
  std::string mName;
  T mResource;
  Status mStatus;
  typename T::InitInfo mInitInfo;

  Asset(const std::string& name);
  template<typename... Args>
  void Prep(Args&&... args);
  Result Init();
  void Finalize();
  Result FullInit();
  template<typename... Args>
  Result FullInit(Args&&... args);
};

// These functions are the primary interface for the AssetLibrary.
template<typename T, typename... Args>
AssetId Require(const std::string& name, Args&&... args);
template<typename T, typename... Args>
AssetId Create(const std::string& name, Args&&... args);
template<typename T>
AssetId CreateSerializable(const std::string& name);
template<typename T>
void Remove(AssetId id);
template<typename T>
T* TryGetLive(AssetId id, AssetId defaultId = nDefaultAssetId);
template<typename T>
Asset<T>& GetAsset(AssetId id);
template<typename T>
Asset<T>* TryGetAsset(AssetId id);
template<typename T>
void TryUpdateInitInfo(AssetId id, const typename T::InitInfo& info);

// Every Asset type has a bin that stores all Assets of that type.
template<typename T>
struct AssetBin
{
  static Ds::Map<AssetId, Asset<T>> smAssets;
  template<typename... Args>
  static void Default(Args&&... args);
  template<typename... Args>
  static AssetId Require(const std::string& name, Args&&... args);
  template<typename... Args>
  static AssetId Create(const std::string& name, Args&&... args);
  static AssetId CreateSerializable(const std::string& name);

  static AssetId smSerializableIdHandout;
  static AssetId NextSerializableId();
  static AssetId smNonserializableIdHandout;
  static AssetId NextNonserializableId();
};
template<typename T>
Ds::Map<AssetId, Asset<T>> AssetBin<T>::smAssets;
template<typename T>
AssetId AssetBin<T>::smSerializableIdHandout = 1;
template<typename T>
AssetId AssetBin<T>::smNonserializableIdHandout = -2;

// Every Asset type has a bin for queueing the ids of Assets that need to be
// initialized on the init thread and finalized on the main thread.
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
extern AssetId nDefaultSpriteShaderId;
extern AssetId nDefaultSkyboxShaderId;
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
