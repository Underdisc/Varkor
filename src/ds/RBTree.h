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
    void Recolor();

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
  const Node* GetHead() const;

private:
  void Insert(Node* newNode);
  void BalanceInsertion(Node* child);
  void RotateLeft(Node* oldRoot);
  void RotateRight(Node* oldRoot);
  void Delete(Node* node);

  Node* mHead;
};

} // namespace Ds

#include "RBTree.hh"

#endif
