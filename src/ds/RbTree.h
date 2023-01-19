#ifndef ds_RbTree_h
#define ds_RbTree_h

namespace Ds {

template<typename T>
class RbTree
{
public:
  struct Node
  {
    Node(const T& value);
    Node(T&& value);
    template<typename... Args>
    Node(Args&&... args);
    Node* FindPredecessor() const;
    Node* FindSuccessor() const;

    enum class Color
    {
      Red,
      Black
    };

    T mValue;
    Node* mParent;
    Node* mLeft;
    Node* mRight;
    Color mColor;
  };

private:
  struct IterBase
  {
    void operator++();
    bool operator==(const IterBase& other);
    bool operator!=(const IterBase& other);

  protected:
    Node* mCurrent;
    friend RbTree<T>;
  };

public:
  struct Iter: IterBase
  {
    T& operator*();
    T* operator->();
  };
  Iter begin() const;
  Iter end() const;

  struct CIter: IterBase
  {
    const T& operator*();
    const T* operator->();
  };
  CIter cbegin() const;
  CIter cend() const;

public:
  RbTree();
  ~RbTree();
  void Insert(const T& value);
  void Insert(T&& value);
  template<typename... Args>
  T& Emplace(Args&&... args);
  template<typename CT>
  void Remove(const CT& value);
  void Clear();
  template<typename CT>
  T& Get(const CT& value);
  template<typename CT>
  T* TryGet(const CT& value);
  bool Contains(const T& value) const;
  bool Empty() const;

  const Node* GetHead() const;
  bool HasConsistentBlackHeight();
  bool HasDoubleRed();

protected:
  // CT is a type that is comparable to the type stored in the tree. In other
  // words, T can be compared to CT with the > and < operators.
  template<typename CT>
  Node* FindNode(const CT& value) const;
  void InsertNode(Node* newNode);
  void RemoveNode(Node* node);

private:
  void BalanceInsertion(Node* child);
  void BalanceRemoval(Node* node);
  void RotateLeft(Node* oldRoot);
  void RotateRight(Node* oldRoot);
  void SwapNodes(Node* a, Node* b);
  void SwapAttachedNodePointers(Node* above, Node* below);
  void SwapDetachedNodePointers(Node* a, Node* b);
  void Delete(Node* node);
  Node* LeftmostNode() const;

  bool HasBlackHeight(Node* node, int currentBh, int expectedBh);
  bool HasDoubleRed(Node* node);

  Node* mHead;
};

} // namespace Ds

#include "RbTree.hh"

#endif
