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
  int Add(MemRef member);
  void Rem(int index);

  void* operator[](int index) const;
  const void* Data() const;
  int Size() const;
  MemRef GetOwner(int index) const;

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

  void ShowStats() const;
  void ShowOwners() const;

  const static int smStartCapacity;
  const static float smGrowthFactor;

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
