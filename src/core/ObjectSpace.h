#ifndef core_ObjectSpace_h
#define core_ObjectSpace_h

#include "ComponentTable.h"
#include "Types.h"
#include "ds/Vector.h"

namespace Core {

struct ComponentAddress
{
  TableRef mTable;
  int mIndex;

  void EndUse();
  bool InUse() const;
};

struct Object
{
  int mAddressIndex;
  ObjSizeT mCount;

  static int smInvalidObjectAddress;

  int EndAddress() const;
  int LastAddress() const;
  bool Valid() const;
};

struct ObjectSpace
{
  TableRef RegisterComponentType(int componentId, int size);
  ObjRef CreateObject();
  void* AddComponent(int componentId, ObjRef object);
  void RemComponent(int componentId, ObjRef object);
  void* GetComponent(int componentId, ObjRef object) const;
  bool HasComponent(int compnentId, ObjRef object) const;
  const void* GetComponentData(int componentId) const;

  // Any component type that is registered within an ObjectSpace should be
  // tightly packed. Not doing so won't necessarily result in errors, but
  // problems will likely appear if the padding of a component's data varies
  // among different compilers.
  template<typename T>
  void RegisterComponentType();

  // Component types that have not been registered with an ObjectSpace will be
  // if they are added to an object as a component.
  template<typename T>
  T& AddComponent(ObjRef object);
  template<typename T>
  void RemComponent(ObjRef object);
  template<typename T>
  T& GetComponent(ObjRef object);
  template<typename T>
  bool HasComponent(ObjRef object);
  template<typename T>
  const T* GetComponentData() const;

  void ShowAll() const;
  void ShowTableLookup() const;
  void ShowTable(int componentId) const;
  void ShowTables() const;
  void ShowObjects() const;
  void ShowAddressBin() const;

  template<typename T>
  void ShowTable() const;

private:
  DS::Vector<TableRef> mTableLookup;
  DS::Vector<ComponentTable> mTables;

  DS::Vector<Object> mObjects;
  DS::Vector<ComponentAddress> mAddressBin;

  bool ValidComponentTable(int componentId) const;
  void VerifyComponentTable(int componentId) const;
  void VerifyObject(ObjRef object) const;
};

} // namespace Core

#include "ObjectSpace.hh"

#endif
