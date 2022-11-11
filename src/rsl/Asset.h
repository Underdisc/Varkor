#ifndef rsl_Asset_h
#define rsl_Asset_h

#include <string>

#include "Result.h"
#include "ds/Vector.h"
#include "rsl/ResourceType.h"
#include "vlk/Valkor.h"

namespace Rsl {

struct Asset
{
  // A status to indicate whether an asset or resource has been loaded or not.
  enum class Status
  {
    Dormant,
    Queued,
    Initializing,
    Failed,
    Live,
  };

  // A way to track a resource within the asset's allocation.
  struct ResourceDescriptor
  {
    ResTypeId mResTypeId;
    std::string mName;
    size_t mByteIndex;
  };
  typedef ResourceDescriptor ResDesc;

private:
  std::string mName;
  Status mStatus;
  char* mResBin;
  size_t mResBinSize;
  size_t mResBinCapacity;
  Ds::Vector<ResDesc> mResDescs;

public:
  explicit Asset(const std::string& name);
  Asset(Asset&& other);
  Asset& operator=(Asset&& other);
  ~Asset();

  // Accessor functions.
  const std::string& GetName() const;
  std::string GetFile() const;
  bool HasFile() const;
  VResult<Vlk::Value> GetVlkValue();
  Status GetStatus() const;
  const Ds::Vector<ResDesc>& GetResDescs() const;
  template<typename T>
  T& GetRes(const std::string& name);
  template<typename T>
  T* TryGetRes(const std::string& name);
  template<typename T>
  bool HasRes(const std::string& name);

  // For initializing or deinitializing an asset and individual resources.
  void QueueInit();
  void Init();
  Result TryInit();
  template<typename T, typename... Args>
  T& InitRes(const std::string& name, Args&&... args);
  template<typename T, typename... Args>
  VResult<T*> TryInitRes(const std::string& name, Args&&... args);
  void Finalize();
  void InitFinalize();
  void Sleep();

private:
  Result InitRes(const Vlk::Explorer& configEx);
  Result InitModel(const std::string& name, const Vlk::Explorer& configEx);

  void Purge();
  void* GetResDescData(const ResDesc& resDesc);
  VResult<ResDesc> AllocateRes(ResTypeId resTypeId, const std::string& name);
  void GrowResBin(size_t neededCapacity);
  void MoveResources(char* fromBin, char* toBin);
  void DestructResources();
};

bool operator>(const Asset& lhs, const Asset& rhs);
bool operator<(const Asset& lhs, const Asset& rhs);
bool operator>(const std::string& lhs, const Asset& rhs);
bool operator>(const Asset& lhs, const std::string& rhs);
bool operator<(const std::string& lhs, const Asset& rhs);
bool operator<(const Asset& lhs, const std::string& rhs);

} // namespace Rsl

#include "rsl/Asset.hh"

#endif