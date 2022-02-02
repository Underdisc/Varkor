#ifndef ds_List_h
#define ds_List_h

namespace Ds {

template<typename T>
struct List
{
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

  Node* mHead;
  Node* mTail;
  size_t mSize;

  struct IterBase
  {
  public:
    void operator++();
    bool operator==(const IterBase& other) const;
    bool operator!=(const IterBase& other) const;

  protected:
    IterBase(Node* current);
    void VerifyCurrent() const;
    Node* mCurrent;
    friend List<T>;
  };

public:
  struct Iter: IterBase
  {
  public:
    T& operator*() const;
    T* operator->() const;

  private:
    Iter(Node* current);
    friend List<T>;
  };

  struct CIter: IterBase
  {
  public:
    const T& operator*() const;
    const T* operator->() const;

  private:
    CIter(Node* current);
    friend List<T>;
  };

  Iter begin() const;
  Iter end() const;
  CIter cbegin() const;
  CIter cend() const;

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
  Iter Insert(Iter it, const T& value);
  Iter Insert(Iter it, T&& value);
  template<typename... Args>
  Iter Emplace(Iter it, Args&&... args);
  void PopBack();
  void PopFront();
  Iter Erase(Iter it);
  void Clear();
  size_t Size() const;

private:
  void InsertNode(Iter it, Node* newNode);
};

} // namespace Ds

#include "ds/List.hh"

#endif
