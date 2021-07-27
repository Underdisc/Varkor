#ifndef AssetLibrary_h
#define AssetLibrary_h

#include <string>

#include "ds/Map.h"
#include "util/Utility.h"

namespace AssetLibrary {

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
  Asset(const std::string& name, bool required);
  Util::Result Init();
  bool Required() const;
  void SetPath(int index, const std::string& newPath);
  const std::string& GetPath(int index) const;

private:
  bool mRequired;
  std::string mPaths[T::smInitPathCount];
};

typedef int AssetId;
constexpr AssetId nInvalidAssetId = -1;

// These are the functions that should be used when interacting with the
// AssetLibrary.
template<typename T>
AssetId Create(const std::string& name, bool includeId = false);
template<typename T>
void Remove(AssetId id);
template<typename T, typename... Args>
AssetId Require(Args&&... args);
template<typename T>
T& Get(AssetId id);
template<typename T>
T* TryGet(AssetId id);
template<typename T>
Asset<T>& GetAsset(AssetId id);
template<typename T>
Asset<T>* TryGetAsset(AssetId id);

// Every Asset type has a bin that stores all Assets of that type.
template<typename T>
struct AssetBin
{
  static AssetId smIdHandout;
  static Ds::Map<AssetId, Asset<T>> smAssets;
  static AssetId NextId()
  {
    return smIdHandout++;
  }
};
template<typename T>
AssetId AssetBin<T>::smIdHandout = 0;
template<typename T>
Ds::Map<AssetId, Asset<T>> AssetBin<T>::smAssets;

} // namespace AssetLibrary

namespace AssLib = AssetLibrary;
typedef AssetLibrary::AssetId AssetId;

#include "AssetLibrary.hh"

#endif
