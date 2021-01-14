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
    Node(const Args&... args);

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

public:
  RbTree();
  ~RbTree();
  void Insert(const T& value);
  void Insert(T&& value);
  template<typename... Args>
  void Emplace(const Args&... args);
  void Remove(const T& value);
  void Clear();
  bool Contains(const T& value);

  const Node* GetHead() const;
  bool HasConsistentBlackHeight();
  bool HasDoubleRed();

private:
  void Insert(Node* newNode);
  void BalanceInsertion(Node* child);
  void BalanceRemoval(Node* node);
  void RotateLeft(Node* oldRoot);
  void RotateRight(Node* oldRoot);
  void Delete(Node* node);

  Node* FindPredecessor(Node* node);
  bool HasBlackHeight(Node* node, int currentBh, int expectedBh);
  bool HasDoubleRed(Node* node);

  Node* mHead;
};

} // namespace Ds

#include "RBTree.hh"

#endif
