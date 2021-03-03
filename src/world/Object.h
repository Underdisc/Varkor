#ifndef world_Object_h
#define world_Object_h

#include "world/Space.h"

namespace World {

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

} // namespace World

#include "Object.hh"

#endif
