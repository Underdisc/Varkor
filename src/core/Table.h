#ifndef core_Table_h
#define core_Table_h

#include "core/Types.h"
#include "ds/Vector.h"

namespace Core {

class Table
{
public:
  Table();
  Table(int stride);
  ~Table();

  // Add and remove components to and from the table.
  int Add(MemRef member);
  void Rem(int index);

  // Access component data and the owner of that component data.
  void* GetData(int index) const;
  MemRef GetOwner(int index) const;

  // Access private members that allow the component table to function.
  const void* Data() const;
  int Stride() const;
  int Size() const;
  int Capacity() const;

  // Visit all of the components in a table by creating a table visitor.
  template<typename T>
  struct Visitor
  {
    Visitor(const Table* table);
    MemRef CurrentOwner();
    T& CurrentComponent();
    void Next();
    bool End();
    void ReachValidComponent();
    const Table* mTable;
    int mCurrentIndex;
  };
  template<typename T>
  Visitor<T> CreateVisitor() const;

  // For determining the size of the memory allocation used for the table.
  static constexpr int smStartCapacity = 10;
  static constexpr float smGrowthFactor = 2.0f;

private:
  char* mData;
  int mStride;
  int mSize;
  int mCapacity;
  Ds::Vector<MemRef> mOwners;

  void Grow();
  void VerifyIndex(int index) const;
};

} // namespace Core

#include "Table.hh"

#endif
