#include "rsl/ResourceId.h"

namespace Rsl {

ResourceId::ResourceId() {}

ResourceId::ResourceId(const ResourceId& other): mId(other.mId) {}

ResourceId::ResourceId(ResourceId&& other): mId(std::move(other.mId)) {}

ResourceId::ResourceId(const char* id): mId(id) {}

ResourceId::ResourceId(const std::string& id): mId(id) {}

ResourceId::ResourceId(
  const std::string& assetName, const std::string& resourceName)
{
  Init(assetName, resourceName);
}

ResourceId& ResourceId::operator=(const ResourceId& other)
{
  mId = other.mId;
  return *this;
}

ResourceId& ResourceId::operator=(const char* id)
{
  mId = id;
  return *this;
}

ResourceId& ResourceId::operator=(const std::string& id)
{
  mId = id;
  return *this;
}

void ResourceId::Init(
  const std::string& assetName, const std::string& resourceName)
{
  mId = assetName + ':' + resourceName;
}

std::string ResourceId::GetAssetName() const
{
  size_t assetIdEnd = mId.find(':');
  if (assetIdEnd == std::string::npos) {
    return "";
  }
  return mId.substr(0, assetIdEnd);
}

std::string ResourceId::GetAssetFile() const
{
  return GetAssetName() + nAssetExtension;
}

std::string ResourceId::GetResourceName() const
{
  size_t assetIdEnd = mId.find(':');
  if (assetIdEnd == std::string::npos) {
    return "";
  }
  size_t resourceNameStart = assetIdEnd + 1;
  size_t count = mId.size() - resourceNameStart;
  return mId.substr(resourceNameStart, count);
}

bool ResourceId::operator==(const ResId& other) const
{
  return mId == other.mId;
}

} // namespace Rsl