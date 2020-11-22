#include <iostream>

#include "Error.h"

#include "ObjectSpace.h"

namespace Core {

void ComponentAddress::EndUse()
{
  mTable = nInvalidTableRef;
}

bool ComponentAddress::InUse() const
{
  return mTable != nInvalidTableRef;
}

std::ostream& operator<<(std::ostream& os, const ComponentAddress& addr)
{
  if (!addr.InUse())
  {
    os << "[inv]";
    return os;
  }
  os << "[" << addr.mTable << ", " << addr.mIndex << "]";
  return os;
}

int Object::smInvalidObjectAddress = -1;

int Object::EndAddress() const
{
  return mAddressIndex + mCount;
}

int Object::LastAddress() const
{
  return mAddressIndex + mCount - 1;
}

bool Object::Valid() const
{
  return mAddressIndex != smInvalidObjectAddress;
}

TableRef ObjectSpace::RegisterComponentType(int componentId, int size)
{
  TableRef table = mTables.Size();
  ComponentTable newTable(size);
  mTables.Push(newTable);
  mTableLookup[componentId] = table;
  return table;
}

ObjRef ObjectSpace::CreateObject()
{
  Object newObject;
  newObject.mAddressIndex = mAddressBin.Size();
  newObject.mCount = 0;
  ObjRef newObjectRef = mObjects.Size();
  mObjects.Push(newObject);
  return newObjectRef;
}

void* ObjectSpace::AddComponent(int componentId, ObjRef object)
{
  // Verify that the component table exists, the object exist, and the object
  // doesn't already have the component being added.
  VerifyComponentTable(componentId);
  VerifyObject(object);
  LogAbortIf(
    HasComponent(componentId, object),
    "This object already has this type of component.");

  // Allocate data for the new component and save a pointer to the component
  // data so it can be used as the return value.
  TableRef table = mTableLookup[componentId];
  ComponentAddress newAddress;
  newAddress.mTable = table;
  newAddress.mIndex = mTables[table].Add(object);
  void* componentData = mTables[table][newAddress.mIndex];

  // Add the component's address to the address bin, and make that address part
  // of the object.
  Object& selected = mObjects[object];
  int nextAddressIndex = selected.EndAddress();
  if (nextAddressIndex >= mAddressBin.Size())
  {
    mAddressBin.Push(newAddress);
    ++selected.mCount;
    return componentData;
  }
  const ComponentAddress& compAddr = mAddressBin[nextAddressIndex];
  if (!compAddr.InUse())
  {
    mAddressBin[nextAddressIndex] = newAddress;
    ++selected.mCount;
    return componentData;
  }
  for (int i = 0; i < selected.mCount; ++i)
  {
    int copyAddrIndex = selected.mAddressIndex + i;
    ComponentAddress copyAddr = mAddressBin[copyAddrIndex];
    mAddressBin.Push(copyAddr);
    mAddressBin[copyAddrIndex].EndUse();
  }
  mAddressBin.Push(newAddress);
  ++selected.mCount;
  selected.mAddressIndex = mAddressBin.Size() - selected.mCount;
  return componentData;
}

void ObjectSpace::RemComponent(int componentId, ObjRef object)
{
  // Verify that the component table and the object both exist.
  VerifyComponentTable(componentId);
  VerifyObject(object);

  // Verify that the object has the component while finding the index of the
  // component address.
  bool foundComponentType = false;
  TableRef table = mTableLookup[componentId];
  Object& selected = mObjects[object];
  int compAddrIndex = selected.mAddressIndex;
  int endAddr = selected.EndAddress();
  for (; compAddrIndex < endAddr; ++compAddrIndex)
  {
    if (mAddressBin[compAddrIndex].mTable == table)
    {
      foundComponentType = true;
      break;
    }
  }
  LogAbortIf(
    !foundComponentType,
    "The object did not have the component type to be removed.");

  // Remove the old component address from the object's address list.
  ComponentAddress& compAddr = mAddressBin[compAddrIndex];
  mTables[table].Rem(compAddr.mIndex);
  if (compAddrIndex == selected.LastAddress())
  {
    compAddr.EndUse();
  } else
  {
    ComponentAddress& lastCompAddr = mAddressBin[selected.LastAddress()];
    compAddr = lastCompAddr;
    lastCompAddr.EndUse();
  }
  --selected.mCount;
}

void* ObjectSpace::GetComponent(int componentId, ObjRef object) const
{
  // Make sure the component table and object exist.
  VerifyComponentTable(componentId);
  VerifyObject(object);

  // Find the requested component by going through the object's component
  // references.
  TableRef table = mTableLookup[componentId];
  const Object& selected = mObjects[object];
  for (int i = 0; i < selected.mCount; ++i)
  {
    const ComponentAddress& address = mAddressBin[selected.mAddressIndex + i];
    if (address.mTable == table)
    {
      return mTables[address.mTable][address.mIndex];
    }
  }
  return nullptr;
}

bool ObjectSpace::HasComponent(int componentId, ObjRef object) const
{
  const void* comp = GetComponent(componentId, object);
  return comp != nullptr;
}

const void* ObjectSpace::GetComponentData(int componentId) const
{
  VerifyComponentTable(componentId);
  TableRef table = mTableLookup[componentId];
  return mTables[table].Data();
}

void ObjectSpace::ShowAll() const
{
  ShowTables();
  ShowTableLookup();
  ShowObjects();
  ShowAddressBin();
}

void ObjectSpace::ShowTableLookup() const
{
  std::cout << "TableLookup: [ComponentId, Table]";
  for (int i = 0; i < mTableLookup.Size(); ++i)
  {
    std::cout << ", [" << i << ", " << mTableLookup[i] << "]";
  }
  std::cout << std::endl;
}

void ObjectSpace::ShowTable(int componentId) const
{
  VerifyComponentTable(componentId);
  TableRef table = mTableLookup[componentId];
  mTables[table].ShowStats();
  mTables[table].ShowOwners();
}

void ObjectSpace::ShowTables() const
{
  for (int i = 0; i < mTables.Size(); ++i)
  {
    std::cout << i << " {" << std::endl;
    mTables[i].ShowStats();
    mTables[i].ShowOwners();
    std::cout << "}" << std::endl;
  }
}

void ObjectSpace::ShowObjects() const
{
  std::cout << "Objects: [Address, Count]";
  for (const Object& object : mObjects)
  {
    std::cout << ", [" << object.mAddressIndex << ", " << object.mCount << "]";
  }
  std::cout << std::endl;
}

void ObjectSpace::ShowAddressBin() const
{
  std::cout << "AddressBin: [Table, Index]";
  for (int i = 0; i < mAddressBin.Size(); ++i)
  {
    std::cout << ", " << mAddressBin[i];
  }
  std::cout << std::endl;
}

bool ObjectSpace::ValidComponentTable(int componentId) const
{
  // Make sure the table lookup is large enough to treat the component id as an
  // index, the component type has been initialized, and the table lookup has a
  // valid table reference for the component id.
  return componentId < mTableLookup.Size() && componentId >= 0 &&
    mTableLookup[componentId] != nInvalidTableRef;
}

void ObjectSpace::VerifyComponentTable(int componentId) const
{
  // Make sure that a table exists for the component type.
  LogAbortIf(
    !ValidComponentTable(componentId),
    "There is no table for this component type.");
}

void ObjectSpace::VerifyObject(ObjRef object) const
{
  // Make sure the object both exists and is valid.
  LogAbortIf(
    object < 0 || object >= mObjects.Size() || !mObjects[object].Valid(),
    "The object under this reference does not exist.");
}

} // namespace Core
