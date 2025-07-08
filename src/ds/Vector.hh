#include <sstream>
#include <utility>

#include "Error.h"
#include "debug/MemLeak.h"
#include "util/Memory.h"

namespace Ds {

template<typename T>
const size_t Vector<T>::smStartCapacity = 10;
template<typename T>
const float Vector<T>::smGrowthFactor = 2.0f;

template<typename T>
Vector<T>::Vector(): mData(nullptr), mSize(0), mCapacity(0) {}

template<typename T>
Vector<T>::Vector(const Vector<T>& other):
  mSize(other.mSize), mCapacity(other.mSize) {
  if (other.mData == nullptr) {
    mData = nullptr;
    return;
  }
  mData = CreateAllocation(other.mSize);
  Util::CopyConstructRange<T>(other.mData, mData, other.mSize);
}

template<typename T>
Vector<T>::Vector(Vector<T>&& other):
  mData(other.mData), mSize(other.mSize), mCapacity(other.mCapacity) {
  other.mData = nullptr;
  other.mSize = 0;
  other.mCapacity = 0;
}

template<typename T>
Vector<T>::Vector(const std::initializer_list<T>& list): Vector() {
  *this = list;
}

template<typename T>
Vector<T>::~Vector() {
  Clear();
  if (mData != nullptr) {
    DeleteAllocation(mData);
  }
}

template<typename T>
void Vector<T>::Push(const T& value) {
  if (mSize >= mCapacity) {
    Grow();
  }
  new (mData + mSize) T(value);
  ++mSize;
}

template<typename T>
void Vector<T>::Push(T&& value) {
  if (mSize >= mCapacity) {
    Grow();
  }
  new (mData + mSize) T(std::forward<T>(value));
  ++mSize;
}

template<typename T>
void Vector<T>::Push(const T& value, size_t count) {
  size_t newSize = mSize + count;
  size_t newCapacity = mCapacity;
  if (newCapacity == 0) {
    newCapacity = smStartCapacity;
  }
  while (newSize > newCapacity) {
    newCapacity = (size_t)((float)newCapacity * smGrowthFactor);
  }
  if (newCapacity > mCapacity) {
    Grow(newCapacity);
  }
  Util::Fill<T>(mData + mSize, value, count);
  mSize = newSize;
}

template<typename T>
template<typename... Args>
void Vector<T>::Emplace(Args&&... args) {
  if (mSize >= mCapacity) {
    Grow();
  }
  new (mData + mSize) T(std::forward<Args>(args)...);
  ++mSize;
}

template<typename T>
void Vector<T>::Insert(size_t index, const T& value) {
  if (index == mSize) {
    Push(value);
    return;
  }
  CreateGap(index);
  mData[index] = value;
  ++mSize;
}

template<typename T>
void Vector<T>::Insert(size_t index, T&& value) {
  if (index == mSize) {
    Push(std::move(value));
    return;
  }
  CreateGap(index);
  mData[index] = std::move(value);
  ++mSize;
}

template<typename T>
void Vector<T>::Swap(size_t indexA, size_t indexB) {
  T temp = std::move(mData[indexA]);
  mData[indexA] = std::move(mData[indexB]);
  mData[indexB] = std::move(temp);
}

template<typename T>
void Vector<T>::Pop() {
  if (mSize != 0) {
    mData[mSize - 1].~T();
    --mSize;
  }
}

template<typename T>
void Vector<T>::Sort() {
  auto greaterThan = [](const T& a, const T& b) -> bool {
    return a > b;
  };
  Quicksort(0, mSize - 1, greaterThan);
}

template<typename T>
void Vector<T>::Sort(bool (*greaterThan)(const T&, const T&)) {
  Quicksort(0, (int)mSize - 1, greaterThan);
}

template<typename T>
void Vector<T>::Quicksort(
  int start, int end, bool (*greaterThan)(const T&, const T&)) {
  if (end <= start) {
    return;
  }
  int pivot = Partition(start, end, greaterThan);
  Quicksort(start, pivot - 1, greaterThan);
  Quicksort(pivot + 1, end, greaterThan);
}

template<typename T>
int Vector<T>::Partition(
  int start, int end, bool (*greaterThan)(const T&, const T&)) {
  // A Hoare partition that uses the center element as the pivot.
  Swap(start, start + (end - start) / 2);
  const T& pivot = mData[start];
  int i = start + 1;
  int j = end;
  while (true) {
    while (greaterThan(mData[j], pivot)) {
      --j;
    }
    while (i <= end && greaterThan(pivot, mData[i])) {
      ++i;
    }
    if (j < i) {
      Swap(start, j);
      return j;
    }
    Swap(i, j);
    --j;
    ++i;
  }
}

template<typename T>
void Vector<T>::Clear() {
  Util::DestructRange<T>(mData, mSize);
  mSize = 0;
}

template<typename T>
void Vector<T>::Remove(size_t index) {
  VerifyIndex(index);
  for (size_t i = index + 1; i < mSize; ++i) {
    mData[i - 1] = std::move(mData[i]);
  }
  mData[mSize - 1].~T();
  --mSize;
}

template<typename T>
void Vector<T>::LazyRemove(size_t index) {
  VerifyIndex(index);
  if (mSize == 1) {
    Pop();
    return;
  }
  --mSize;
  mData[index] = std::move(mData[mSize]);
  mData[mSize].~T();
}

template<typename T>
template<typename... Args>
void Vector<T>::Resize(size_t newSize, Args&&... args) {
  for (size_t i = newSize; i < mSize; ++i) {
    mData[i].~T();
  }
  if (newSize > mCapacity) {
    Grow(newSize);
  }
  for (size_t i = mSize; i < newSize; ++i) {
    new (mData + i) T(args...);
  }
  mSize = newSize;
}

template<typename T>
void Vector<T>::Reserve(size_t newCapacity) {
  if (mCapacity >= newCapacity) {
    return;
  }
  Grow(newCapacity);
}

template<typename T>
void Vector<T>::Shrink() {
  if (mSize == mCapacity) {
    return;
  }

  T* oldData = mData;
  mData = CreateAllocation(mSize);
  Util::MoveConstructRange<T>(oldData, mData, mSize);
  Util::DestructRange<T>(oldData, mSize);
  DeleteAllocation(oldData);
  mCapacity = mSize;
}

template<typename T>
template<typename CT>
VResult<size_t> Vector<T>::Find(const CT& value) const {
  for (size_t i = 0; i < mSize; ++i) {
    if (mData[i] == value) {
      return i;
    }
  }
  return Result("Value not found");
}

template<typename T>
template<typename CT>
bool Vector<T>::Contains(const CT& value) const {
  return Find(value).Success();
}

template<typename T>
size_t Vector<T>::Size() const {
  return mSize;
}

template<typename T>
bool Vector<T>::Empty() const {
  return mSize == 0;
}

template<typename T>
size_t Vector<T>::Capacity() const {
  return mCapacity;
}

template<typename T>
const T* Vector<T>::CData() const {
  return mData;
}

template<typename T>
T* Vector<T>::Data() {
  return mData;
}

template<typename T>
T& Vector<T>::Top() const {
  LogAbortIf(mSize == 0, "The Vector is empty.");
  return mData[mSize - 1];
}

template<typename T>
const T& Vector<T>::operator[](size_t index) const {
  VerifyIndex(index);
  return mData[index];
}

template<typename T>
T& Vector<T>::operator[](size_t index) {
  VerifyIndex(index);
  return mData[index];
}

template<typename T>
Vector<T>& Vector<T>::operator=(const Vector<T>& other) {
  // Handle cases where this vector's size is larger than or equal to the
  // other's.
  if (mSize == other.mSize) {
    Util::CopyAssignRange<T>(other.mData, mData, mSize);
    return *this;
  }
  if (mSize > other.mSize) {
    Util::CopyAssignRange<T>(other.mData, mData, other.mSize);
    Util::DestructRange<T>(mData + other.mSize, mSize - other.mSize);
    mSize = other.mSize;
    return *this;
  }

  // Handle cases where this vector's size is smaller than the other's.
  if (mCapacity >= other.mSize) {
    Util::CopyAssignRange<T>(other.mData, mData, mSize);
    T* from = other.mData + mSize;
    T* to = mData + mSize;
    size_t count = other.mSize - mSize;
    Util::CopyConstructRange(from, to, count);
    mSize = other.mSize;
    return *this;
  }

  // The other vector's size is larger and this vector needs a larger capacity
  // to contain all of the elements from it.
  if (mData != nullptr) {
    Util::DestructRange<T>(mData, mSize);
    DeleteAllocation(mData);
  }
  T* newData = CreateAllocation(other.mSize);
  Util::CopyConstructRange<T>(other.mData, newData, other.mSize);
  mCapacity = other.mSize;
  mSize = other.mSize;
  mData = newData;
  return *this;
}

template<typename T>
Vector<T>& Vector<T>::operator=(Vector<T>&& other) {
  Clear();
  if (mData != nullptr) {
    DeleteAllocation(mData);
  }
  mData = other.mData;
  mSize = other.mSize;
  mCapacity = other.mCapacity;

  other.mData = nullptr;
  other.mSize = 0;
  other.mCapacity = 0;
  return *this;
}

template<typename T>
Vector<T>& Vector<T>::operator=(const std::initializer_list<T>& list) {
  Clear();
  for (const T& element: list) {
    Push(element);
  }
  return *this;
}

template<typename T>
T* Vector<T>::begin() {
  return mData;
}

template<typename T>
T* Vector<T>::end() {
  return mData + mSize;
}

template<typename T>
const T* Vector<T>::begin() const {
  return mData;
}

template<typename T>
const T* Vector<T>::end() const {
  return mData + mSize;
}

template<typename T>
void Vector<T>::VerifyIndex(size_t index) const {
  if (index < 0 || index >= mSize) {
    std::stringstream error;
    error << index << " is not a valid index.";
    LogAbort(error.str().c_str());
  }
}

template<typename T>
void Vector<T>::CreateGap(size_t index) {
  VerifyIndex(index);
  if (mSize >= mCapacity) {
    Grow();
  }
  new (mData + mSize) T(std::move(*(mData + mSize - 1)));
  for (size_t i = mSize - 1; i > index; --i) {
    mData[i] = std::move(mData[i - 1]);
  }
}

template<typename T>
void Vector<T>::Grow() {
  if (mData == nullptr) {
    mCapacity = smStartCapacity;
    mData = CreateAllocation(mCapacity);
  }
  else {
    size_t newCapacity = (size_t)((float)mCapacity * smGrowthFactor);
    Grow(newCapacity);
  }
}

template<typename T>
void Vector<T>::Grow(size_t newCapacity) {
  LogAbortIf(
    newCapacity <= mCapacity,
    "The new capacity must be greater than the current capacity.");

  T* newData = CreateAllocation(newCapacity);
  Util::MoveConstructRange(mData, newData, mSize);
  Util::DestructRange(mData, mSize);
  DeleteAllocation(mData);
  mData = newData;
  mCapacity = newCapacity;
}

template<typename T>
T* Vector<T>::CreateAllocation(size_t capacity) {
  size_t byteCount = sizeof(T) * capacity;
  char* byteAllocation = alloc char[byteCount];
  return (T*)byteAllocation;
}

template<typename T>
void Vector<T>::DeleteAllocation(T* allocation) {
  char* byteAllocation = (char*)allocation;
  delete[] byteAllocation;
}

} // namespace Ds
