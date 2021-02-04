#include <iostream>

#include "Print.h"

void PrintTableStats(const Core::Table& table)
{
  int stride = table.Stride();
  int size = table.Size();
  int capacity = table.Capacity();
  std::cout << "Stride: " << stride << std::endl
            << "Size: " << size << std::endl
            << "SizeInBytes: " << size * stride << std::endl
            << "Capacity: " << capacity << std::endl
            << "CapacityInBytes: " << capacity * stride << std::endl;
}

void PrintTableOwners(const Core::Table& table)
{
  std::cout << "Owners: ";
  int size = table.Size();
  if (size == 0)
  {
    std::cout << "[]" << std::endl;
    return;
  }
  std::cout << "[" << table.GetOwner(0);
  for (int i = 1; i < size; ++i)
  {
    std::cout << ", " << table.GetOwner(i);
  }
  std::cout << "]" << std::endl;
}

void PrintSpace(const Core::Space& space)
{
  PrintSpaceTables(space);
  PrintSpaceTableLookup(space);
  PrintSpaceMembers(space);
  PrintSpaceAddressBin(space);
}

void PrintSpaceTables(const Core::Space& space)
{
  const Ds::Vector<Core::Table>& tables = space.Tables();
  for (int i = 0; i < tables.Size(); ++i)
  {
    std::cout << i << " {" << std::endl;
    PrintTableStats(tables[i]);
    PrintTableOwners(tables[i]);
    std::cout << "}" << std::endl;
  }
}

void PrintSpaceTableLookup(const Core::Space& space)
{
  const Ds::Vector<Core::TableRef>& lookup = space.TableLookup();
  std::cout << "TableLookup: [ComponentId, Table]";
  for (int i = 0; i < lookup.Size(); ++i)
  {
    std::cout << ", [" << i << ", " << lookup[i] << "]";
  }
  std::cout << std::endl;
}

void PrintSpaceTablesOwners(const Core::Space& space)
{
  const Ds::Vector<Core::Table>& tables = space.Tables();
  for (int i = 0; i < tables.Size(); ++i)
  {
    std::cout << "Table " << i << " ";
    PrintTableOwners(tables[i]);
  }
}

void PrintSpaceMembers(const Core::Space& space)
{
  const Ds::Vector<Core::Member>& members = space.Members();
  std::cout << "Members: [Address, Count]";
  for (const Core::Member& member : members)
  {
    std::cout << ", ";
    if (!member.InUse())
    {
      std::cout << "[inv]";
      continue;
    }
    std::cout << "[" << member.mAddressIndex << ", " << member.mCount << "]";
  }
  std::cout << std::endl;
}

void PrintSpaceAddressBin(const Core::Space& space)
{
  const Ds::Vector<Core::ComponentAddress>& addressBin = space.AddressBin();
  std::cout << "AddressBin: [Table, Index]";
  for (const Core::ComponentAddress& addr : addressBin)
  {
    std::cout << ", ";
    if (!addr.InUse())
    {
      std::cout << "[inv]";
      continue;
    }
    std::cout << "[" << addr.mTable << ", " << addr.mIndex << "]";
  }
  std::cout << std::endl;
}

void PrintSpaceUnusedMemRefs(const Core::Space& space)
{
  const Ds::Vector<Core::MemRef>& unusedMemRefs = space.UnusedMemRefs();
  std::cout << "UnusedMemRefs: ";
  if (unusedMemRefs.Size() == 0)
  {
    std::cout << "None" << std::endl;
    return;
  }
  std::cout << unusedMemRefs[0];
  for (int i = 1; i < unusedMemRefs.Size(); ++i)
  {
    std::cout << ", " << unusedMemRefs[i];
  }
  std::cout << std::endl;
}
