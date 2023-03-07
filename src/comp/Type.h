#ifndef comp_Type_h
#define comp_Type_h

#include <string>

#include "ds/Vector.h"
#include "gfx/Renderable.h"
#include "util/Delegate.h"

namespace Vlk {
struct Value;
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
  static void Register(const char* name);
  template<typename... Dependencies>
  static void AddDependencies();
};

struct TypeData
{
  std::string mName;
  size_t mSize;
  Ds::Vector<TypeId> mDependencies;
  Ds::Vector<TypeId> mDependants;
  void (*mDefaultConstruct)(void* data);
  void (*mCopyConstruct)(void* from, void* to);
  void (*mMoveConstruct)(void* from, void* to);
  void (*mDestruct)(void* data);
  Util::Delegate<void> mVStaticInit;
  Util::Delegate<void, const World::Object&> mVInit;
  Util::Delegate<void, const World::Object&> mVUpdate;
  Util::Delegate<void, Vlk::Value&> mVSerialize;
  Util::Delegate<void, const Vlk::Explorer&> mVDeserialize;
  Util::Delegate<void, const World::Object&> mVRenderable;
  Util::Delegate<void, const World::Object&> mVEdit;
  Util::Delegate<void, const World::Object&> mVGizmoEdit;

  template<typename Dependant, typename Dependency, typename... Rest>
  void AddDependencies();
  template<typename Dependant>
  void AddDependencies();
};

template<typename T>
const TypeData& GetTypeData();
const TypeData& GetTypeData(TypeId id);
size_t TypeDataCount();
TypeId GetTypeId(const std::string& typeName);

void AssessComponentsFile();
void SaveComponentsFile();

} // namespace Comp

#include "Type.hh"

#endif
