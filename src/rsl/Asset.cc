#include <filesystem>

#include "ext/Tracy.h"
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
std::string Asset::smInitResName = "";

Asset::Asset(const std::string& name):
  mName(name),
  mStatus(Status::Dormant),
  mResBin(nullptr),
  mResBinSize(0),
  mResBinCapacity(0) {}

Asset::Asset(Asset&& other) {
  *this = std::move(other);
}

Asset& Asset::operator=(Asset&& rhs) {
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

Asset::~Asset() {
  Purge();
}

const std::string& Asset::GetName() const {
  return mName;
}

std::string Asset::GetFile() const {
  return mName + nAssetExtension;
}

bool Asset::HasFile() const {
  std::string file = GetFile();
  return std::filesystem::exists(file);
}

Asset::Status Asset::GetStatus() const {
  return mStatus;
}

const Ds::Vector<Asset::ResDesc>& Asset::GetResDescs() const {
  return mResDescs;
}

void Asset::QueueInit() {
  if (mStatus != Status::Dormant && mStatus != Status::Failed) {
    std::string error = "Asset \"" + mName + "\" already ";
    switch (mStatus) {
    case Status::Queued: error += "queued."; break;
    case Status::Initializing: error += "initializing."; break;
    case Status::Live: error += "live."; break;
    default: break;
    }
    LogAbort(error.c_str());
  }
  mStatus = Status::Queued;
  AddToInitQueue(*this);
}

void Asset::Init() {
  Result result = TryInit();
  if (!result.Success()) {
    std::stringstream error;
    error << "Asset \"" << mName << "\" failed initialization.\n"
          << result.mError;
    LogAbort(error.str().c_str());
  }
}

Result Asset::TryInit() {
  ZoneScoped;
  ZoneText(mName.c_str(), (size_t)mName.size());

  smInitAsset = this;
  mStatus = Status::Initializing;

  VResult<Vlk::Value*> addConfigResult = AddConfig(mName);
  if (!addConfigResult.Success()) {
    mStatus = Status::Failed;
    return std::move(addConfigResult);
  }
  Vlk::Explorer rootEx(*addConfigResult.mValue);
  if (!rootEx.Valid(Vlk::Value::Type::ValueArray)) {
    return Result("Root Value is not a ValueArray.");
  }

  // Initialize all of the resources in the value.
  Result initResResult;
  for (int i = 0; i < rootEx.Size(); ++i) {
    initResResult = TryInitRes(rootEx[i]);
    if (!initResResult.Success()) {
      Purge();
      mStatus = Status::Failed;
      break;
    }
  }

  RemConfig(mName);
  smInitAsset = nullptr;
  return initResResult;
}

void Asset::Finalize() {
  ZoneScoped;

  for (ResDesc& resDesc: mResDescs) {
    if (resDesc.mResTypeId == ResTypeId::Mesh) {
      Gfx::Mesh& mesh = *(Gfx::Mesh*)GetResDescData(resDesc);
      mesh.Finalize();
    }
  }
  mStatus = Status::Live;
}

void Asset::InitFinalize() {
  Init();
  Finalize();
}

Vlk::Value* Asset::TryGetResVal(
  Vlk::Value& assetVal, const std::string& resName) {
  for (int i = 0; i < assetVal.Size(); ++i) {
    Vlk::Value& resVal = assetVal[i];
    const Vlk::Value* nameVal = resVal.TryGetConstPair("Name");
    if (nameVal == nullptr) {
      continue;
    }
    if (nameVal->GetType() != Vlk::Value::Type::TrueValue) {
      continue;
    }
    if (resName == nameVal->As<std::string>()) {
      return &resVal;
    }
  }
  return nullptr;
}

VResult<Ds::Vector<Asset::DefResInfo>> Asset::GetAllDefResInfo(
  const Vlk::Explorer& assetEx) {
  if (!assetEx.Valid(Vlk::Value::Type::ValueArray)) {
    return Result("Root Value is not a ValueArray.");
  }

  Ds::Vector<DefResInfo> allDefResInfo;
  for (int i = 0; i < assetEx.Size(); ++i) {
    Vlk::Explorer resEx = assetEx[i];
    DefResInfo newDefResInfo;

    Vlk::Explorer nameEx = resEx("Name");
    if (!nameEx.Valid(Vlk::Value::Type::TrueValue)) {
      return Result(
        "Resource at \"" + resEx.Path() + "\" missing :Name: TrueValue.");
    }
    newDefResInfo.mName = nameEx.As<std::string>();

    Vlk::Explorer typeEx = resEx("Type");
    if (!typeEx.Valid(Vlk::Value::Type::TrueValue)) {
      return Result(
        "Resource \"" + newDefResInfo.mName + "\" at \"" + resEx.Path() +
        "\" missing :Type: TrueValue.");
    }
    newDefResInfo.mTypeId = GetResTypeId(typeEx.As<std::string>());
    allDefResInfo.Emplace(std::move(newDefResInfo));
  }
  return allDefResInfo;
}

Asset& Asset::GetInitAsset() {
  LogAbortIf(smInitAsset == nullptr, "An asset is not being initialized.");
  return *smInitAsset;
}

const std::string& Asset::GetInitResName() {
  return smInitResName;
}

Result Asset::TryInitRes(const Vlk::Explorer& resEx) {
  ZoneScoped;

  // Get the resource's name.
  Vlk::Explorer nameEx = resEx("Name");
  if (!nameEx.Valid(Vlk::Value::Type::TrueValue)) {
    return Result(
      "Resource at \"" + resEx.Path() + "\" missing :Name: TrueValue.");
  }
  std::string name = nameEx.As<std::string>();
  ZoneText(name.c_str(), (size_t)name.size());

  // Get the resource's type.
  Vlk::Explorer typeEx = resEx("Type");
  if (!typeEx.Valid(Vlk::Value::Type::TrueValue)) {
    return Result(
      "Resource \"" + name + "\" at \"" + resEx.Path() +
      "\" missing :Type: TrueValue.");
  }
  std::string typeStr = typeEx.As<std::string>();
  ResTypeId resTypeId = GetResTypeId(typeStr);
  if (resTypeId == ResTypeId::Invalid) {
    return Result(
      "Resource \"" + name + "\" at \"" + resEx.Path() +
      "\" has an invalid type \"" + typeStr + "\".");
  }

  // Get the config that will be used to initialize the resource.
  Vlk::Explorer configEx = resEx("Config");
  if (!configEx.Valid()) {
    return Result(
      "Resource \"" + name + "\" at \"" + resEx.Path() +
      "\" missing :Config: PairArray.");
  }

  // Initialize the resource.
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
  default: break;
  }
  // clang-format on
  glFinish();
  if (!result.Success()) {
    const ResTypeData& resTypeData = Rsl::GetResTypeData(resTypeId);
    return Result(
      "Resource \"" + name + "\" at \"" + resEx.Path() +
      "\" failed initialization.\n" + result.mError);
  }
  return result;
}

void Asset::Purge() {
  DestructResources();
  if (mResBin != nullptr) {
    delete[] mResBin;
    mResBin = nullptr;
    mResBinSize = 0;
    mResBinCapacity = 0;
  }
  mResDescs.Clear();
}

void* Asset::GetResDescData(const ResDesc& resDesc) {
  return (void*)&mResBin[resDesc.mByteIndex];
}

VResult<Asset::ResDesc> Asset::AllocateRes(
  ResTypeId resTypeId, const std::string& name) {
  // Make sure the supplied resource name is not already in use.
  for (const ResDesc& resDesc: mResDescs) {
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

void Asset::GrowResBin(size_t newCapacity) {
  char* newResBin = alloc char[newCapacity];
  if (mResBin != nullptr) {
    MoveResources(mResBin, newResBin);
    DestructResources();
    delete[] mResBin;
  }
  mResBin = newResBin;
  mResBinCapacity = newCapacity;
}

void Asset::MoveResources(char* fromBin, char* toBin) {
  for (const ResDesc& resDesc: mResDescs) {
    void* fromRes = (void*)&fromBin[resDesc.mByteIndex];
    void* toRes = (void*)&toBin[resDesc.mByteIndex];
    const ResTypeData& resTypeData = GetResTypeData(resDesc.mResTypeId);
    resTypeData.mMoveConstruct(fromRes, toRes);
  }
}

void Asset::DestructResources() {
  for (const ResDesc& resDesc: mResDescs) {
    void* atRes = (void*)&mResBin[resDesc.mByteIndex];
    const ResTypeData& resTypeData = GetResTypeData(resDesc.mResTypeId);
    resTypeData.mDestruct(atRes);
  }
}

bool operator>(const Asset& lhs, const Asset& rhs) {
  return lhs.GetName() > rhs.GetName();
}

bool operator<(const Asset& lhs, const Asset& rhs) {
  return lhs.GetName() < rhs.GetName();
}

bool operator>(const std::string& lhs, const Asset& rhs) {
  return lhs > rhs.GetName();
}

bool operator>(const Asset& lhs, const std::string& rhs) {
  return lhs.GetName() > rhs;
}

bool operator<(const std::string& lhs, const Asset& rhs) {
  return lhs < rhs.GetName();
}

bool operator<(const Asset& lhs, const std::string& rhs) {
  return lhs.GetName() < rhs;
}

} // namespace Rsl
