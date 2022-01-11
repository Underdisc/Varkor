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
void HandleAssetLoading();
bool InitThreadOpen();
void DeserializeAssets();
void SerializeAssets();

// AssetIds refer to an Asset within an AssetBin. Positive AssetIds refer to
// typical user Assets. Negative AssetIds and 0 refer to required Assets. If any
// required Asset fails initialization, an abort will occur.
typedef int AssetId;
constexpr AssetId nDefaultAssetId = 0;
bool IsRequiredId(AssetId id);

// The current Status of Assets must be tracked since they are loaded on a
// different thread. Live indicates that an Asset is ready to be used.
enum class Status
{
  Unneeded,
  Initializing,
  Failed,
  Live,
};

// Types can be used as Assets if they provide specific members. These types are
// asset resources. Compiler errors will highlight the necessary members if an
// incompatible type is used.
template<typename T>
struct Asset
{
public:
  std::string mName;
  T mResource;
  Asset(const std::string& name, Status status);
  Result Init();
  Result Init(const std::string paths[T::smInitPathCount]);
  void SetPath(int index, const std::string& newPath);
  const std::string& GetPath(int index) const;

  Status mStatus;

private:
  std::string mPaths[T::smInitPathCount];
};

// These functions serve as the primary interface for the AssetLibrary.
template<typename T>
AssetId Create(const std::string& name, bool includeId = false);
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
  static AssetId smIdHandout;
  static AssetId smRequiredIdHandout;

  template<typename... Args>
  static void InitDefault(Args&&... args);
  template<typename... Args>
  static AssetId Require(const std::string& name, Args&&... args);
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
// initialized on the initialization thread.
template<typename T>
struct InitBin
{
  static Ds::Vector<AssetId> smInitQueue;
  static AssetId CurrentId();
  static void Queue(AssetId id);
  static void AssessInitQueue();
  static void HandleInitQueue();
};
template<typename T>
Ds::Vector<AssetId> InitBin<T>::smInitQueue;

// FInfo means FinalizeInfo. Meshes within Gfx::Model must be finalized before
// they can be rendered. This stores the information needed for finalization.
struct ModelFInfo
{
  AssetId mId;
  int mMeshIndex;
  unsigned int mAttributes;
  size_t mVertexByteCount;

  // If mMeshIndex equals smInvalidMeshIndex, all of the meshes owned by the
  // Gfx::Model have been finalized and it's ready for rendering.
  static const int smInvalidMeshIndex;
};
void AddModelFInfo(const ModelFInfo& fInfo);

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
