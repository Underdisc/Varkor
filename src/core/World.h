#ifndef core_World_h
#define core_World_h

#include "core/Space.h"
#include "ds/Vector.h"

namespace Core {
namespace World {

// This is used to iterate through all of the spaces that are currently in the
// world.
struct SpaceVisitor
{
  SpaceVisitor();
  Space& CurrentSpace();
  SpaceRef CurrentSpaceRef();
  void Next();
  bool End();

private:
  SpaceRef mCurrentSpace;
};

// This represents a game object in the world. Objects do not store any data.
// They are merely a way to reference the collection of data that represents an
// object.
struct Object
{
  Object();
  Object(SpaceRef space, MemRef member);

  template<typename T>
  void AddComponent();
  template<typename T>
  void RemComponent();
  template<typename T>
  T* GetComponent();
  template<typename T>
  bool HasComponent();

  std::string& GetName();
  void Invalidate();
  bool Valid();

  SpaceRef mSpace;
  MemRef mMember;
};

SpaceRef CreateSpace();
Space& GetSpace(SpaceRef ref);

} // namespace World
} // namespace Core

#include "World.hh"

#endif
