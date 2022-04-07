#ifndef world_Object_h
#define world_Object_h

#include <string>

#include "comp/Type.h"
#include "world/Space.h"

namespace World {

// This represents a game object in the world. Objects do not store any data.
// They are merely a way to reference the collection of data that represents an
// object.
struct Object
{
  Object(const Object& other);
  Object(Space* space);
  Object(Space* space, MemberId memberId);

  template<typename T>
  T& AddComponent() const;
  template<typename T>
  void RemComponent() const;
  template<typename T>
  T* GetComponent() const;
  template<typename T>
  bool HasComponent() const;

  void* AddComponent(Comp::TypeId typeId) const;
  void RemComponent(Comp::TypeId typeId) const;
  void* GetComponent(Comp::TypeId typeId) const;
  bool HasComponent(Comp::TypeId typeId) const;

  Object CreateChild() const;
  Object Duplicate() const;
  Member& GetMember() const;
  std::string& GetName() const;
  bool HasParent() const;
  Object Parent() const;
  bool Valid() const;

  Space* mSpace;
  MemberId mMemberId;
};

} // namespace World

#include "Object.hh"

#endif
