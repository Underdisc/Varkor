#ifndef comp_Type_h
#define comp_Type_h

#include <string>

#include "ds/Vector.h"
#include "util/Delegate.h"

namespace Vlk {
struct Pair;
struct Explorer;
} // namespace Vlk

namespace World {
struct Object;
}

namespace Comp {

typedef int TypeId;
constexpr TypeId nInvalidTypeId = -1;

template<typename T>
struct Type
{
  static TypeId smId;
  static void Register();
  static void RegisterEditHook();
  static void RegisterGizmo();
};

struct TypeData
{
  std::string mName;
  int mSize;

  void (*mDefaultConstruct)(void* data);
  void (*mCopyConstruct)(void* from, void* to);
  void (*mMoveConstruct)(void* from, void* to);
  void (*mDestruct)(void* data);

  Util::Delegate<void> mVUpdate;
  Util::Delegate<void, Vlk::Pair&> mVSerialize;
  Util::Delegate<void, const Vlk::Explorer&> mVDeserialize;

  void (*mEditHook)(void* component);
  void (*mGizmoStart)();
  bool (*mGizmoRun)(void* component, const World::Object& object);
};

template<typename T>
const TypeData& GetTypeData();
const TypeData& GetTypeData(TypeId id);
int TypeDataCount();
TypeId GetTypeId(const std::string& typeName);

} // namespace Comp

#include "Type.hh"

#endif
