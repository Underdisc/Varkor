#include "Error.h"
#include "util/Utility.h"

namespace Ds {

template<typename T>
List<T>::List(): mHead(nullptr), mTail(nullptr), mSize(0)
{}

template<typename T>
List<T>::~List()
{
  Clear();
}

template<typename T>
void List<T>::PushBack(const T& value)
{
  Node* newNode = new Node(value);
  PushNodeBack(newNode);
}

template<typename T>
void List<T>::PushFront(const T& value)
{
  Node* newNode = new Node(value);
  PushNodeFront(newNode);
}

template<typename T>
void List<T>::PushBack(T&& value)
{
  Node* newNode = new Node(Util::Forward(value));
  PushNodeBack(newNode);
}

template<typename T>
void List<T>::PushFront(T&& value)
{
  Node* newNode = new Node(Util::Forward(value));
  PushNodeFront(newNode);
}

template<typename T>
template<typename... Args>
void List<T>::EmplaceBack(Args&&... args)
{
  Node* newNode = new Node(Util::Forward(args)...);
  PushNodeBack(newNode);
}

template<typename T>
template<typename... Args>
void List<T>::EmplaceFront(Args&&... args)
{
  Node* newNode = new Node(Util::Forward(args)...);
  PushNodeFront(newNode);
}

template<typename T>
void List<T>::PopBack()
{
  LogAbortIf(mTail == nullptr, "The list is empty");
  --mSize;
  if (mTail->mPrev == nullptr)
  {
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
void List<T>::PopFront()
{
  LogAbortIf(mHead == nullptr, "The list is empty");
  --mSize;
  if (mHead->mNext == nullptr)
  {
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
void List<T>::Clear()
{
  Node* current = mHead;
  Node* next = nullptr;
  while (current != nullptr)
  {
    next = current->mNext;
    delete current;
    current = next;
  }
  mHead = nullptr;
  mTail = nullptr;
  mSize = 0;
}

template<typename T>
size_t List<T>::Size() const
{
  return mSize;
}

template<typename T>
List<T>::Node::Node(const T& value):
  mValue(value), mNext(nullptr), mPrev(nullptr)
{}

template<typename T>
List<T>::Node::Node(T&& value):
  mValue(Util::Forward(value)), mNext(nullptr), mPrev(nullptr)
{}

template<typename T>
template<typename... Args>
List<T>::Node::Node(Args&&... args):
  mValue(Util::Forward(args)...), mNext(nullptr), mPrev(nullptr)
{}

template<typename T>
void List<T>::PushNodeBack(Node* newNode)
{
  ++mSize;
  if (mTail == nullptr)
  {
    mTail = newNode;
    mHead = newNode;
    return;
  }
  mTail->mNext = newNode;
  newNode->mPrev = mTail;
  mTail = newNode;
}

template<typename T>
void List<T>::PushNodeFront(Node* newNode)
{
  ++mSize;
  if (mHead == nullptr)
  {
    mHead = newNode;
    mTail = newNode;
    return;
  }
  mHead->mPrev = newNode;
  newNode->mNext = mHead;
  mHead = newNode;
}

template<typename T>
void List<T>::IterBase::operator++()
{
  VerifyCurrent();
  mCurrent = mCurrent->mNext;
}

template<typename T>
bool List<T>::IterBase::operator==(const IterBase& other)
{
  return mCurrent == other.mCurrent;
}

template<typename T>
bool List<T>::IterBase::operator!=(const IterBase& other)
{
  return mCurrent != other.mCurrent;
}

template<typename T>
void List<T>::IterBase::VerifyCurrent()
{
  LogAbortIf(mCurrent == nullptr, "The Iter does not point to a Node.");
}

template<typename T>
T& List<T>::Iter::operator*()
{
  VerifyCurrent();
  return mCurrent->mValue;
}

template<typename T>
T* List<T>::Iter::operator->()
{
  VerifyCurrent();
  return &mCurrent->mValue;
}

template<typename T>
const T& List<T>::CIter::operator*()
{
  VerifyCurrent();
  return mCurrent->mValue;
}

template<typename T>
const T* List<T>::CIter::operator->()
{
  VerifyCurrent();
  return &mCurrent->mValue;
}

template<typename T>
typename List<T>::Iter List<T>::begin() const
{
  Iter begin;
  begin.mCurrent = mHead;
  return begin;
}

template<typename T>
typename List<T>::Iter List<T>::end() const
{
  Iter end;
  end.mCurrent = nullptr;
  return end;
}

template<typename T>
typename List<T>::CIter List<T>::cbegin() const
{
  CIter cbegin;
  cbegin.mCurrent = mHead;
  return cbegin;
}

template<typename T>
typename List<T>::CIter List<T>::cend() const
{
  CIter cend;
  cend.mCurrent = nullptr;
  return cend;
}

} // namespace Ds
