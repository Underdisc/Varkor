#ifndef AssetLibrary_h
#define AssetLibrary_h

#include <string>

#include "Result.h"
#include "ds/Map.h"

namespace AssetLibrary {

void Init();
void LoadAssets();
void SaveAssets();

// Types can be used as Assets if they provide specific members. These types are
// asset resources. Compiler errors will highlight the necessary members if an
// incompatible type is used.
template<typename T>
struct Asset
{
public:
  std::string mName;
  T mResource;
  Asset(const std::string& name);
  Result Init();
  Result Init(const std::string paths[T::smInitPathCount]);
  void SetPath(int index, const std::string& newPath);
  const std::string& GetPath(int index) const;

private:
  std::string mPaths[T::smInitPathCount];
};

// AssetIds refer to an Asset within an AssetBin. Positive AssetIds refer to
// typical user Assets. Negative AssetIds and 0 refer to required Assets. If any
// required Asset fails initialization, an abort will occur.
typedef int AssetId;
constexpr AssetId nDefaultAssetId = 0;
bool IsRequiredId(AssetId id);

// These functions serve as the primary interface for the AssetLibrary.
template<typename T>
AssetId Create(const std::string& name, bool includeId = false);
template<typename T>
void Remove(AssetId id);
template<typename T, typename... Args>
AssetId Require(const std::string& name, Args&&... args);
template<typename T>
T& Get(AssetId id, AssetId defaultId = nDefaultAssetId);
template<typename T>
Asset<T>& GetAsset(AssetId id, AssetId defaultId = nDefaultAssetId);
template<typename T>
T* TryGet(AssetId id);
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
  static AssetId NextId();
  static AssetId NextRequiredId();
};
template<typename T>
Ds::Map<AssetId, Asset<T>> AssetBin<T>::smAssets;
template<typename T>
AssetId AssetBin<T>::smIdHandout = 1;
template<typename T>
AssetId AssetBin<T>::smRequiredIdHandout = -1;

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
