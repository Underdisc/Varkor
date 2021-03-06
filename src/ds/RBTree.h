#ifndef ds_RBTree_h
#define ds_RBTree_h

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
  struct CIter: IterBase
  {
    const T& operator*();
    const T* operator->();
  };
  CIter CBegin();
  CIter CEnd();

protected:
  struct Iter: IterBase
  {
    T& operator*();
    T* operator->();
  };
  Iter Begin();
  Iter End();

public:
  RbTree();
  ~RbTree();
  void Insert(const T& value);
  void Insert(T&& value);
  template<typename... Args>
  void Emplace(Args&&... args);
  void Remove(const T& value);
  void Clear();
  bool Contains(const T& value);

  const Node* GetHead() const;
  bool HasConsistentBlackHeight();
  bool HasDoubleRed();

protected:
  // CT is a type that is comparable to the type stored in the tree. In other
  // words, T can be compared to CT with the > and < operators.
  template<typename CT>
  Node* FindNode(const CT& value);
  void RemoveNode(Node* node);

private:
  void InsertNode(Node* newNode);
  void BalanceInsertion(Node* child);
  void BalanceRemoval(Node* node);
  void RotateLeft(Node* oldRoot);
  void RotateRight(Node* oldRoot);
  void Delete(Node* node);
  Node* LeftmostNode();

  bool HasBlackHeight(Node* node, int currentBh, int expectedBh);
  bool HasDoubleRed(Node* node);

  Node* mHead;
};

} // namespace Ds

#include "RBTree.hh"

#endif
