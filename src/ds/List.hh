#include <utility>

#include "Error.h"
#include "debug/MemLeak.h"

namespace Ds {

template<typename T>
List<T>::Node::Node(const T& value):
  mValue(value), mNext(nullptr), mPrev(nullptr) {}

template<typename T>
List<T>::Node::Node(T&& value):
  mValue(std::forward<T>(value)), mNext(nullptr), mPrev(nullptr) {}

template<typename T>
template<typename... Args>
List<T>::Node::Node(Args&&... args):
  mValue(std::forward<Args>(args)...), mNext(nullptr), mPrev(nullptr) {}

template<typename T>
void List<T>::IterBase::operator++() {
  this->VerifyCurrent();
  this->mCurrent = mCurrent->mNext;
}

template<typename T>
bool List<T>::IterBase::operator==(const IterBase& other) const {
  return this->mCurrent == other.mCurrent;
}

template<typename T>
bool List<T>::IterBase::operator!=(const IterBase& other) const {
  return this->mCurrent != other.mCurrent;
}

template<typename T>
List<T>::IterBase::IterBase(Node* current): mCurrent(current) {}

template<typename T>
void List<T>::IterBase::VerifyCurrent() const {
  LogAbortIf(mCurrent == nullptr, "The Iter does not point to a Node.");
}

template<typename T>
List<T>::Iter::Iter(Node* current): IterBase(current) {}

template<typename T>
T& List<T>::Iter::operator*() const {
  this->VerifyCurrent();
  return this->mCurrent->mValue;
}

template<typename T>
T* List<T>::Iter::operator->() const {
  this->VerifyCurrent();
  return &this->mCurrent->mValue;
}

template<typename T>
List<T>::CIter::CIter(Node* current): IterBase(current) {}

template<typename T>
const T& List<T>::CIter::operator*() const {
  this->VerifyCurrent();
  return this->mCurrent->mValue;
}

template<typename T>
const T* List<T>::CIter::operator->() const {
  this->VerifyCurrent();
  return &this->mCurrent->mValue;
}

template<typename T>
typename List<T>::Iter List<T>::begin() const {
  Iter begin(mHead);
  return begin;
}

template<typename T>
typename List<T>::Iter List<T>::end() const {
  Iter end(nullptr);
  return end;
}

template<typename T>
typename List<T>::CIter List<T>::cbegin() const {
  CIter cbegin(mHead);
  return cbegin;
}

template<typename T>
typename List<T>::CIter List<T>::cend() const {
  CIter cend(nullptr);
  return cend;
}

template<typename T>
List<T>::List(): mHead(nullptr), mTail(nullptr), mSize(0) {}

template<typename T>
List<T>::~List() {
  Clear();
}

template<typename T>
void List<T>::PushBack(const T& value) {
  Node* newNode = alloc Node(value);
  InsertNode(end(), newNode);
}

template<typename T>
void List<T>::PushFront(const T& value) {
  Node* newNode = alloc Node(value);
  InsertNode(begin(), newNode);
}

template<typename T>
void List<T>::PushBack(T&& value) {
  Node* newNode = alloc Node(std::forward<T>(value));
  InsertNode(end(), newNode);
}

template<typename T>
void List<T>::PushFront(T&& value) {
  Node* newNode = alloc Node(std::forward<T>(value));
  InsertNode(begin(), newNode);
}

template<typename T>
template<typename... Args>
void List<T>::EmplaceBack(Args&&... args) {
  Node* newNode = alloc Node(std::forward<Args>(args)...);
  InsertNode(end(), newNode);
}

template<typename T>
template<typename... Args>
void List<T>::EmplaceFront(Args&&... args) {
  Node* newNode = alloc Node(std::forward<Args>(args)...);
  InsertNode(begin(), newNode);
}

template<typename T>
typename List<T>::Iter List<T>::Insert(Iter it, const T& value) {
  Node* newNode = alloc Node(value);
  InsertNode(it, newNode);
  return Iter(newNode);
}

template<typename T>
typename List<T>::Iter List<T>::Insert(Iter it, T&& value) {
  Node* newNode = alloc Node(std::forward<T>(value));
  InsertNode(it, newNode);
  return Iter(newNode);
}

template<typename T>
template<typename... Args>
typename List<T>::Iter List<T>::Emplace(Iter it, Args&&... args) {
  Node* newNode = alloc Node(std::forward<Args>(args)...);
  InsertNode(it, newNode);
  return Iter(newNode);
}

template<typename T>
void List<T>::PopBack() {
  LogAbortIf(mTail == nullptr, "The list is empty");
  --mSize;
  if (mTail->mPrev == nullptr) {
    delete mTail;
    mTail = nullptr;
    mHead = nullptr;
    return;
  }
  Node* prev = mTail->mPrev;
  delete mTail;
  mTail = prev;
  mTail->mNext = nullptr;
}

template<typename T>
void List<T>::PopFront() {
  LogAbortIf(mHead == nullptr, "The list is empty");
  --mSize;
  if (mHead->mNext == nullptr) {
    delete mHead;
    mHead = nullptr;
    mTail = nullptr;
    return;
  }
  Node* next = mHead->mNext;
  delete mHead;
  mHead = next;
  mHead->mPrev = nullptr;
}

template<typename T>
typename List<T>::Iter List<T>::Erase(Iter it) {
  it.VerifyCurrent();
  --mSize;
  if (it.mCurrent->mNext == nullptr && it.mCurrent->mPrev == nullptr) {
    LogAbortIf(
      it.mCurrent != mHead, "Erasing only element of a different list.");
    mTail = nullptr;
    mHead = nullptr;
    delete it.mCurrent;
    return end();
  }
  if (it.mCurrent->mPrev == nullptr) {
    LogAbortIf(it.mCurrent != mHead, "Erasing head of a different list.");
    mHead = it.mCurrent->mNext;
    mHead->mPrev = nullptr;
    delete it.mCurrent;
    return begin();
  }
  if (it.mCurrent->mNext == nullptr) {
    LogAbortIf(it.mCurrent != mTail, "Erasing tail of a different list.");
    mTail = it.mCurrent->mPrev;
    mTail->mNext = nullptr;
    delete it.mCurrent;
    return end();
  }
  Node* next = it.mCurrent->mNext;
  next->mPrev = it.mCurrent->mPrev;
  it.mCurrent->mPrev->mNext = next;
  delete it.mCurrent;
  return Iter(next);
}

template<typename T>
void List<T>::Clear() {
  Node* current = mHead;
  Node* next = nullptr;
  while (current != nullptr) {
    next = current->mNext;
    delete current;
    current = next;
  }
  mHead = nullptr;
  mTail = nullptr;
  mSize = 0;
}

template<typename T>
typename List<T>::Iter List<T>::Front() {
  return Iter(mHead);
}

template<typename T>
typename List<T>::Iter List<T>::Back() {
  return Iter(mTail);
}

template<typename T>
size_t List<T>::Size() const {
  return mSize;
}

template<typename T>
void List<T>::InsertNode(Iter it, Node* newNode) {
  ++mSize;
  if (mHead == nullptr) {
    mHead = newNode;
    mTail = newNode;
    return;
  }
  else if (it.mCurrent == mHead) {
    mHead->mPrev = newNode;
    newNode->mNext = mHead;
    mHead = newNode;
    return;
  }
  else if (it.mCurrent == nullptr) {
    mTail->mNext = newNode;
    newNode->mPrev = mTail;
    mTail = newNode;
    return;
  }
  Node* prev = it.mCurrent->mPrev;
  prev->mNext = newNode;
  newNode->mPrev = prev;
  newNode->mNext = it.mCurrent;
  it.mCurrent->mPrev = newNode;
}

} // namespace Ds
