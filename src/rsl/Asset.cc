#include <filesystem>

#include "gfx/Cubemap.h"
#include "gfx/Font.h"
#include "gfx/Image.h"
#include "gfx/Material.h"
#include "gfx/Mesh.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"
#include "rsl/Asset.h"
#include "rsl/Library.h"

#include "Error.h"

namespace Rsl {

Asset* Asset::smInitAsset = nullptr;

Asset::Asset(const std::string& name):
  mName(name),
  mStatus(Status::Dormant),
  mResBin(nullptr),
  mResBinSize(0),
  mResBinCapacity(0)
{}

Asset::Asset(Asset&& other)
{
  *this = std::move(other);
}

Asset& Asset::operator=(Asset&& rhs)
{
  Purge();
  mName = std::move(rhs.mName);
  mStatus = rhs.mStatus;
  mResDescs = std::move(rhs.mResDescs);
  mResBin = rhs.mResBin;
  mResBinSize = rhs.mResBinSize;
  mResBinCapacity = rhs.mResBinCapacity;

  rhs.mResBin = nullptr;
  rhs.mResBinSize = 0;
  rhs.mResBinCapacity = 0;
  return *this;
}

Asset::~Asset()
{
  Purge();
}

const std::string& Asset::GetName() const
{
  return mName;
}

std::string Asset::GetFile() const
{
  return mName + nAssetExtension;
}

bool Asset::HasFile() const
{
  std::string file = GetFile();
  return std::filesystem::exists(file);
}

VResult<Vlk::Value> Asset::GetVlkValue() const
{
  // Get the path to the asset file.
  VResult<std::string> resolutionResult = ResolveResPath(GetFile());
  if (!resolutionResult.Success()) {
    return Result(
      "Asset \"" + mName + "\" missing file.\n" + resolutionResult.mError);
  }
  const std::string& resolvedFile = resolutionResult.mValue;

  // Read the value within the asset file.
  Vlk::Value val;
  Result result = val.Read(resolvedFile.c_str());
  if (!result.Success()) {
    return Result("Asset \"" + mName + "\" failed read.\n" + result.mError);
  }
  return VResult<Vlk::Value>(std::move(val));
}

VResult<ResTypeId> Asset::GetResTypeId(const Vlk::Explorer& resEx) const
{
  Vlk::Explorer resourceTypeEx = resEx("Type");
  if (!resourceTypeEx.Valid(Vlk::Value::Type::TrueValue)) {
    return Result(
      "Resource \"" + resEx.Path() + "\" missing :Type: TrueValue.");
  }
  std::string resTypeIdStr = resourceTypeEx.As<std::string>("Invalid");
  ResTypeId resTypeId = Rsl::GetResTypeId(resTypeIdStr);
  if (resTypeId == ResTypeId::Invalid) {
    return Result(
      "Resource \"" + resourceTypeEx.Path() + "\" has invalid type \"" +
      resTypeIdStr + "\".");
  }
  return resTypeId;
}

VResult<Ds::Vector<Asset::DefResInfo>> Asset::GetAllDefResInfo() const
{
  VResult<Vlk::Value> result = GetVlkValue();
  if (!result.Success()) {
    return result;
  }
  Vlk::Explorer assetEx(result.mValue);
  if (!assetEx.Valid(Vlk::Value::Type::PairArray)) {
    return Result("Asset \"" + mName + "\" root Value is not a PairArray.");
  }

  Ds::Vector<DefResInfo> allDefResInfo;
  for (int i = 0; i < assetEx.Size(); ++i) {
    Vlk::Explorer resEx = assetEx(i);
    VResult<ResTypeId> resTypeIdResult = GetResTypeId(resEx);
    if (!resTypeIdResult.Success()) {
      return resTypeIdResult;
    }
    DefResInfo newDefResInfo;
    newDefResInfo.mName = resEx.Key();
    newDefResInfo.mTypeId = resTypeIdResult.mValue;
    allDefResInfo.Emplace(std::move(newDefResInfo));
  }
  return allDefResInfo;
}

Asset::Status Asset::GetStatus() const
{
  return mStatus;
}

const Ds::Vector<Asset::ResDesc>& Asset::GetResDescs() const
{
  return mResDescs;
}

void Asset::QueueInit()
{
  if (mStatus != Status::Dormant && mStatus != Status::Failed) {
    std::string error = "Asset \"" + mName + "\" already ";
    switch (mStatus) {
    case Status::Queued: error += "queued."; break;
    case Status::Initializing: error += "initializing."; break;
    case Status::Live: error += "live."; break;
    }
    LogAbort(error.c_str());
  }
  mStatus = Status::Queued;
  AddToInitQueue(*this);
}

void Asset::Init()
{
  Result result = TryInit();
  if (!result.Success()) {
    std::stringstream error;
    error << "Asset \"" << mName << "\" failed initialization.\n"
          << result.mError;
    LogAbort(error.str().c_str());
  }
}

Result Asset::TryInit()
{
  smInitAsset = this;
  mStatus = Status::Initializing;

  VResult<Vlk::Value> result = GetVlkValue();
  if (!result.Success()) {
    mStatus = Status::Failed;
    return result;
  }
  Vlk::Explorer rootEx(result.mValue);

  // Initialize all of the resources in the value.
  for (int i = 0; i < rootEx.Size(); ++i) {
    Result result = TryInitRes(rootEx(i));
    if (!result.Success()) {
      Purge();
      mStatus = Status::Failed;
      return result;
    }
  }

  smInitAsset = nullptr;
  return Result();
}

void Asset::Finalize()
{
  for (ResDesc& resDesc : mResDescs) {
    if (resDesc.mResTypeId == ResTypeId::Mesh) {
      Gfx::Mesh& mesh = *(Gfx::Mesh*)GetResDescData(resDesc);
      mesh.Finalize();
    }
  }
  mStatus = Status::Live;
}

void Asset::InitFinalize()
{
  Init();
  Finalize();
}

void Asset::Sleep()
{
  Purge();
  mStatus = Status::Dormant;
}

Asset& Asset::GetInitAsset()
{
  LogAbortIf(smInitAsset == nullptr, "An asset is not being initialized.");
  return *smInitAsset;
}

Result Asset::TryInitRes(const Vlk::Explorer& resEx)
{
  // Get the resource's type.
  VResult<ResTypeId> resTypeIdResult = GetResTypeId(resEx);
  if (!resTypeIdResult.Success()) {
    return resTypeIdResult;
  }
  ResTypeId resTypeId = resTypeIdResult.mValue;

  // Get the config that will be used to initialize the resource.
  Vlk::Explorer configEx = resEx("Config");
  if (!configEx.Valid()) {
    std::string error = "Resource \"" + resEx.Path() + "\" missing :Config:.";
    return Result(error);
  }

  // Initialize the resource.
  const std::string& name = resEx.Key();
  Result result;
  // clang-format off
  switch (resTypeId) {
  case ResTypeId::Cubemap:
    result = TryInitRes<Gfx::Cubemap>(name, configEx); break;
  case ResTypeId::Font:
    result = TryInitRes<Gfx::Font>(name, configEx); break;
  case ResTypeId::Image:
    result = TryInitRes<Gfx::Image>(name, configEx); break;
  case ResTypeId::Material:
    result = TryInitRes<Gfx::Material>(name, configEx); break;
  case ResTypeId::Mesh:
    result = TryInitRes<Gfx::Mesh>(name, configEx); break;
  case ResTypeId::Model:
    result = TryInitRes<Gfx::Model>(name, configEx); break;
  case ResTypeId::Shader:
    result = TryInitRes<Gfx::Shader>(name, configEx); break;
  }
  // clang-format on
  glFinish();
  if (!result.Success()) {
    const ResTypeData& resTypeData = Rsl::GetResTypeData(resTypeId);
    std::string error = "Resource \"" + resEx.Key() +
      "\" failed initialization.\n" + result.mError;
    return Result(error);
  }
  return result;
}

void Asset::Purge()
{
  DestructResources();
  if (mResBin != nullptr) {
    delete[] mResBin;
    mResBin = nullptr;
    mResBinSize = 0;
    mResBinCapacity = 0;
  }
  mResDescs.Clear();
}

void* Asset::GetResDescData(const ResDesc& resDesc)
{
  return (void*)&mResBin[resDesc.mByteIndex];
}

VResult<Asset::ResDesc> Asset::AllocateRes(
  ResTypeId resTypeId, const std::string& name)
{
  // Make sure the supplied resource name is not already in use.
  for (const ResDesc& resDesc : mResDescs) {
    if (resTypeId == resDesc.mResTypeId && name == resDesc.mName) {
      const ResTypeData& resTypeData = GetResTypeData(resTypeId);
      std::string typeName = resTypeData.mName;
      return Result(typeName + " resource name \"" + name + "\" already used.");
    }
  }

  // Create space for the new resource.
  const ResTypeData& resTypeData = GetResTypeData(resTypeId);
  size_t oldSize = mResBinSize;
  size_t newSize = mResBinSize + resTypeData.mSize;
  if (mResBinCapacity < newSize) {
    GrowResBin(newSize);
  }
  mResBinSize = newSize;

  ResDesc newResDesc;
  newResDesc.mResTypeId = resTypeId;
  newResDesc.mName = name;
  if (mResDescs.Empty()) {
    newResDesc.mByteIndex = 0;
  }
  else {
    newResDesc.mByteIndex = oldSize;
  }
  mResDescs.Push(std::move(newResDesc));
  return mResDescs.Top();
}

void Asset::GrowResBin(size_t newCapacity)
{
  char* newResBin = alloc char[newCapacity];
  if (mResBin != nullptr) {
    MoveResources(mResBin, newResBin);
    DestructResources();
    delete[] mResBin;
  }
  mResBin = newResBin;
  mResBinCapacity = newCapacity;
}

void Asset::MoveResources(char* fromBin, char* toBin)
{
  for (const ResDesc& resDesc : mResDescs) {
    void* fromRes = (void*)&fromBin[resDesc.mByteIndex];
    void* toRes = (void*)&toBin[resDesc.mByteIndex];
    const ResTypeData& resTypeData = GetResTypeData(resDesc.mResTypeId);
    resTypeData.mMoveConstruct(fromRes, toRes);
  }
}

void Asset::DestructResources()
{
  for (const ResDesc& resDesc : mResDescs) {
    void* atRes = (void*)&mResBin[resDesc.mByteIndex];
    const ResTypeData& resTypeData = GetResTypeData(resDesc.mResTypeId);
    resTypeData.mDestruct(atRes);
  }
}

bool operator>(const Asset& lhs, const Asset& rhs)
{
  return lhs.GetName() > rhs.GetName();
}

bool operator<(const Asset& lhs, const Asset& rhs)
{
  return lhs.GetName() < rhs.GetName();
}

bool operator>(const std::string& lhs, const Asset& rhs)
{
  return lhs > rhs.GetName();
}

bool operator>(const Asset& lhs, const std::string& rhs)
{
  return lhs.GetName() > rhs;
}

bool operator<(const std::string& lhs, const Asset& rhs)
{
  return lhs < rhs.GetName();
}

bool operator<(const Asset& lhs, const std::string& rhs)
{
  return lhs.GetName() < rhs;
}

} // namespace Rsl