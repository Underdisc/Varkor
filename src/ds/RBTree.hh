#include "Error.h"
#include "debug/MemLeak.h"
#include "util/Utility.h"

namespace Ds {

template<typename T>
RbTree<T>::Node::Node(const T& value):
  mValue(value),
  mParent(nullptr),
  mLeft(nullptr),
  mRight(nullptr),
  mColor(Color::Red)
{}

template<typename T>
RbTree<T>::Node::Node(T&& value):
  mValue(Util::Move(value)),
  mParent(nullptr),
  mLeft(nullptr),
  mRight(nullptr),
  mColor(Color::Red)
{}

template<typename T>
template<typename... Args>
RbTree<T>::Node::Node(const Args&... args):
  mValue(args...),
  mParent(nullptr),
  mLeft(nullptr),
  mRight(nullptr),
  mColor(Color::Red)
{}

template<typename T>
void RbTree<T>::Node::Recolor()
{
  if (mColor == Color::Black)
  {
    mColor = Color::Red;
  } else
  {
    mColor = Color::Black;
  }
}

template<typename T>
RbTree<T>::RbTree()
{
  mHead = nullptr;
}

template<typename T>
RbTree<T>::~RbTree()
{
  Delete(mHead);
}

template<typename T>
void RbTree<T>::Insert(const T& value)
{
  Node* newNode = alloc Node(value);
  Insert(newNode);
}

template<typename T>
void RbTree<T>::Insert(T&& value)
{
  Node* newNode = alloc Node(Util::Move(value));
  Insert(newNode);
}

template<typename T>
template<typename... Args>
void RbTree<T>::Emplace(const Args&... args)
{
  Node* newNode = alloc Node(args...);
  Insert(newNode);
}

template<typename T>
void RbTree<T>::RotateLeft(Node* oldRoot)
{
  // Perform the primary rotation operation.
  Node* newRoot = oldRoot->mRight;
  oldRoot->mRight = newRoot->mLeft;
  newRoot->mLeft = oldRoot;

  // Update the parent pointers of the 3 affected nodes.
  newRoot->mParent = oldRoot->mParent;
  oldRoot->mParent = newRoot;
  if (oldRoot->mRight != nullptr)
  {
    oldRoot->mRight->mParent = oldRoot;
  }

  // Update the connection from the parent to the new root.
  Node* parent = newRoot->mParent;
  if (parent == nullptr)
  {
    mHead = newRoot;
  } else if (parent->mLeft == oldRoot)
  {
    parent->mLeft = newRoot;
  } else
  {
    parent->mRight = newRoot;
  }
}

template<typename T>
void RbTree<T>::RotateRight(Node* oldRoot)
{
  // This is just the reflection of function for rotating left.
  Node* newRoot = oldRoot->mLeft;
  oldRoot->mLeft = newRoot->mRight;
  newRoot->mRight = oldRoot;

  newRoot->mParent = oldRoot->mParent;
  oldRoot->mParent = newRoot;
  if (oldRoot->mLeft != nullptr)
  {
    oldRoot->mLeft->mParent = oldRoot;
  }

  Node* parent = newRoot->mParent;
  if (parent == nullptr)
  {
    mHead = newRoot;
  } else if (parent->mLeft == oldRoot)
  {
    parent->mLeft = newRoot;
  } else
  {
    parent->mRight = newRoot;
  }
}

template<typename T>
void RbTree<T>::Insert(Node* newNode)
{
  if (mHead == nullptr)
  {
    mHead = newNode;
    mHead->mColor = Node::Color::Black;
    return;
  }

  // Traverse the tree to find the connection where the new node will be
  // inserted.
  Node* parent = nullptr;
  Node** insertionPoint = &mHead;
  while (*insertionPoint != nullptr)
  {
    parent = *insertionPoint;
    if (newNode->mValue > (*insertionPoint)->mValue)
    {
      insertionPoint = &(*insertionPoint)->mRight;
    } else if (newNode->mValue < (*insertionPoint)->mValue)
    {
      insertionPoint = &(*insertionPoint)->mLeft;
    } else
    {
      LogAbort("The RbTree already contains this value.");
    }
  }

  // Insert the new node and balance the tree.
  newNode->mParent = parent;
  *insertionPoint = newNode;
  BalanceInsertion(newNode);
}

template<typename T>
void RbTree<T>::BalanceInsertion(Node* child)
{
  while (child != nullptr && child->mColor == Node::Color::Red &&
         child->mParent->mColor == Node::Color::Red)
  {
    Node* parent = child->mParent;
    Node* grandparent = parent->mParent;
    if (grandparent->mLeft == parent)
    {
      Node* uncle = grandparent->mRight;
      if (uncle == nullptr || uncle->mColor == Node::Color::Black)
      {
        // We need make sure that the child is to the left of the parent.
        if (child == parent->mRight)
        {
          RotateLeft(parent);
          Node* temp = child;
          child = parent;
          parent = temp;
        }

        // We need to rotate the parent to the grandparent's position because
        // the uncle is either non-existant or black.
        RotateRight(grandparent);
        grandparent->Recolor();
        parent->Recolor();
      } else
      {
        // We push down the blackness when the uncle is red.
        if (grandparent != mHead)
        {
          grandparent->Recolor();
        }
        parent->Recolor();
        uncle->Recolor();
      }
    } else
    {
      // This is just a reflection of the code that is used when the parent is
      // to the left of the grandparent.
      Node* uncle = grandparent->mLeft;
      if (uncle == nullptr || uncle->mColor == Node::Color::Black)
      {
        if (child == parent->mLeft)
        {
          RotateRight(parent);
          Node* temp = child;
          child = parent;
          parent = temp;
        }

        RotateLeft(grandparent);
        grandparent->Recolor();
        parent->Recolor();
      } else
      {
        if (grandparent != mHead)
        {
          grandparent->Recolor();
        }
        parent->Recolor();
        uncle->Recolor();
      }
    }
    child = child->mParent->mParent;
  }
}

template<typename T>
const typename RbTree<T>::Node* RbTree<T>::GetHead() const
{
  return mHead;
}

template<typename T>
void RbTree<T>::Delete(Node* node)
{
  if (node == nullptr)
  {
    return;
  }
  Delete(node->mLeft);
  Delete(node->mRight);
  delete node;
}

} // namespace Ds
