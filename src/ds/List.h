#ifndef ds_List_h
#define ds_List_h

namespace Ds {

template<typename T>
struct List
{
public:
  List();
  ~List();
  void PushBack(const T& value);
  void PushFront(const T& value);
  void PushBack(T&& value);
  void PushFront(T&& value);
  template<typename... Args>
  void EmplaceBack(Args&&... args);
  template<typename... Args>
  void EmplaceFront(Args&&... args);
  void PopBack();
  void PopFront();
  void Clear();
  size_t Size() const;

private:
  struct Node
  {
    Node(const T& value);
    Node(T&& value);
    template<typename... Args>
    Node(Args&&... args);

    T mValue;
    Node* mNext;
    Node* mPrev;
  };
  void PushNodeBack(Node* newNode);
  void PushNodeFront(Node* newNode);

  Node* mHead;
  Node* mTail;
  size_t mSize;

  struct IterBase
  {
  public:
    void operator++();
    bool operator==(const IterBase& other);
    bool operator!=(const IterBase& other);

  protected:
    void VerifyCurrent();
    Node* mCurrent;

    friend List<T>;
  };

public:
  struct Iter: IterBase
  {
    T& operator*();
    T* operator->();
  };
  struct CIter: IterBase
  {
    const T& operator*();
    const T* operator->();
  };
  Iter begin() const;
  Iter end() const;
  CIter cbegin() const;
  CIter cend() const;
};

} // namespace Ds

#include "ds/List.hh"

#endif
