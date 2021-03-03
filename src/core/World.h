#ifndef core_World_h
#define core_World_h

#include "Camera.h"
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
  void AddComponent() const;
  template<typename T>
  void RemComponent() const;
  template<typename T>
  T* GetComponent() const;
  template<typename T>
  bool HasComponent() const;

  std::string& GetName() const;
  void Invalidate();
  bool Valid() const;

  SpaceRef mSpace;
  MemRef mMember;
};

extern void (*CentralUpdate)();
extern void (*SpaceUpdate)(const Space& space, SpaceRef spaceRef);

void Init();
SpaceRef CreateSpace();
Space& GetSpace(SpaceRef ref);
void Update();
void Render(const Camera& camera);

template<typename T>
void UpdateComponentType(const Space& space, SpaceRef spaceRef);

} // namespace World
} // namespace Core

#include "World.hh"

#endif
