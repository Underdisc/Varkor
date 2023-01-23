#ifndef rsl_Library_h
#define rsl_Library_h

#include <string>

#include "Result.h"
#include "ds/RbTree.h"
#include "rsl/Asset.h"
#include "rsl/ResourceId.h"

namespace Rsl {

constexpr const char* nInvalidAssetName = "vres/invalid";
constexpr const char* nDefaultAssetName = "vres/defaults";

Asset& AddAsset(const std::string& name);
Asset& QueueAsset(const std::string& name);
Asset& RequireAsset(const std::string& name);
void RemAsset(const std::string& name);
Asset& GetAsset(const std::string& name);
Asset* TryGetAsset(const std::string& name);
Asset::Status GetAssetStatus(const std::string& name);

VResult<Vlk::Value*> AddConfig(const std::string& assetName);
void RemConfig(const std::string& assetName);
Vlk::Value& GetConfig(const std::string& assetName);
void WriteConfig(const std::string& assetName);

template<typename T>
ResId GetDefaultResId();
template<typename T>
T& GetDefaultRes();
template<typename T>
T& GetRes(const ResId& resId);
template<typename T>
T* TryGetRes(const ResId& resId);
template<typename T>
bool HasRes(const ResId& resId);

bool IsStandalone();
std::string PrependResDirectory(const std::string& path);
VResult<std::string> ResolveProjPath(const std::string& path);
VResult<std::string> ResolveResPath(const std::string& path);

extern Ds::RbTree<Asset> nAssets;
void Init();
void Purge();
void AddToInitQueue(const Asset& asset);
void HandleInitialization();
bool InitThreadOpen();

} // namespace Rsl

#include "rsl/Library.hh"

#endif
