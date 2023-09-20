#ifndef rsl_ResourceId_h
#define rsl_ResourceId_h

#include <string>

namespace Rsl {

constexpr const char* nAssetExtension = ".a";
constexpr const char* nResIdDelimeter = ":";

// A way to identify a specific resource within the resource library.
struct ResourceId
{
  // Stores the asset and resource name like so "AssetName:ResourceName"
  std::string mId;

  ResourceId();
  ResourceId(const ResourceId& other);
  ResourceId(ResourceId&& other);
  ResourceId(const char* id);
  ResourceId(const std::string& id);
  ResourceId(const std::string& assetName, const std::string& resourceName);
  ResourceId& operator=(const ResourceId& other);
  ResourceId& operator=(const char* id);
  ResourceId& operator=(const std::string& id);
  void Init(const std::string& assetName, const std::string& resourceName);

  std::string GetAssetName() const;
  std::string GetAssetFile() const;
  std::string GetResourceName() const;
  void SetResourceName(const std::string& name);
  bool operator==(const ResourceId& other) const;
  bool operator!=(const ResourceId& other) const;
};
typedef ResourceId ResId;

} // namespace Rsl

typedef Rsl::ResId ResId;

#endif