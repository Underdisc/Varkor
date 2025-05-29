#ifndef ds_HashSet_h
#define ds_HashSet_h

#include "ds/Hash.h"
#include "ds/Vector.h"

namespace Ds {

template<typename T>
struct HashSet {
private:
  struct IterBase {
  public:
    void operator++();
    void operator--();
    bool operator==(const IterBase& other) const;
    bool operator!=(const IterBase& other) const;

  protected:
    IterBase(HashSet<T>& hashSet, size_t bucket, size_t idx);
    void Verify() const;
    HashSet<T>& mHashSet;
    size_t mBucket;
    size_t mIdx;
    friend HashSet<T>;
  };

public:
  struct Iter: IterBase {
    T& operator*();
    T* operator->();
    Iter& operator++();
    Iter& operator--();
    Iter operator++(int);
    Iter operator--(int);

  private:
    Iter(HashSet<T>& hashSet, size_t bucket, size_t idx);
    friend HashSet<T>;
  };

  struct CIter: IterBase {
  public:
    CIter(Iter& iter);
    const T& operator*() const;
    const T* operator->() const;
    CIter& operator++();
    CIter& operator--();
    CIter operator++(int);
    CIter operator--(int);

  private:
    CIter(HashSet<T>& hashSet, size_t bucket, size_t idx);
    friend HashSet<T>;
  };

  Iter begin() const;
  Iter end() const;
  CIter cbegin() const;
  CIter cend() const;

  HashSet();
  template<typename U>
  Iter Insert(U&& key);
  void Clear();

  template<typename CT>
  void Remove(const CT& key);
  template<typename CT>
  Iter Find(const CT& key) const;
  template<typename CT>
  bool Contains(const CT& key) const;

  bool Empty() const;
  const Ds::Vector<Ds::Vector<T>>& Buckets() const;
  size_t StartBucket() const;

private:
  Ds::Vector<Ds::Vector<T>> mBuckets;
  size_t mSize;

  template<typename CT>
  size_t Bucket(const CT& key) const;
  void TryGrow();
  void Grow();
  void Grow(size_t newBucketCount);
};

} // namespace Ds

#include "HashSet.hh"

#endif
