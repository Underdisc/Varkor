#ifndef world_Object_h
#define world_Object_h

#include <string>

#include "world/Types.h"

namespace World {

// This represents a game object in the world. Objects do not store any data.
// They are merely a way to reference the collection of data that represents an
// object.
struct Object
{
  Object();
  Object(SpaceId space, MemberId member);

  template<typename T>
  void AddComponent() const;
  template<typename T>
  void RemComponent() const;
  template<typename T>
  T* GetComponent() const;
  template<typename T>
  bool HasComponent() const;

  std::string& GetName() const;
  bool HasParent() const;
  Object Parent() const;
  void Invalidate();
  bool Valid() const;

  SpaceId mSpace;
  MemberId mMember;
};

} // namespace World

#include "Object.hh"

#endif
