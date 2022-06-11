#include <utility>

#include "Error.h"
#include "debug/MemLeak.h"

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
  mValue(std::forward<T>(value)),
  mParent(nullptr),
  mLeft(nullptr),
  mRight(nullptr),
  mColor(Color::Red)
{}

template<typename T>
template<typename... Args>
RbTree<T>::Node::Node(Args&&... args):
  mValue(std::forward<Args>(args)...),
  mParent(nullptr),
  mLeft(nullptr),
  mRight(nullptr),
  mColor(Color::Red)
{}

template<typename T>
typename RbTree<T>::Node* RbTree<T>::Node::FindPredecessor() const
{
  if (mLeft == nullptr) {
    return nullptr;
  }
  Node* predecessor = mLeft;
  while (predecessor->mRight != nullptr) {
    predecessor = predecessor->mRight;
  }
  return predecessor;
}

template<typename T>
typename RbTree<T>::Node* RbTree<T>::Node::FindSuccessor() const
{
  if (mRight == nullptr) {
    return nullptr;
  }
  Node* successor = mRight;
  while (successor->mLeft != nullptr) {
    successor = successor->mLeft;
  }
  return successor;
}

////////////////////////////////////////////////////////////////////////////////
template<typename T>
void RbTree<T>::IterBase::operator++()
{
  Node* successor = mCurrent->FindSuccessor();
  if (successor != nullptr) {
    mCurrent = successor;
    return;
  }

  // There was no right subtree so we need to travel up the tree.
  while (mCurrent->mParent != nullptr) {
    Node* child = mCurrent;
    mCurrent = mCurrent->mParent;
    if (mCurrent->mLeft == child) {
      return;
    }
  }

  // mCurrent was at the rightmost node, hence we are done traversing the tree.
  mCurrent = nullptr;
}

template<typename T>
bool RbTree<T>::IterBase::operator==(const IterBase& other)
{
  return mCurrent == other.mCurrent;
}

template<typename T>
bool RbTree<T>::IterBase::operator!=(const IterBase& other)
{
  return mCurrent != other.mCurrent;
}

template<typename T>
T& RbTree<T>::Iter::operator*()
{
  return this->mCurrent->mValue;
}

template<typename T>
T* RbTree<T>::Iter::operator->()
{
  return &this->mCurrent->mValue;
}

template<typename T>
const T& RbTree<T>::CIter::operator*()
{
  return this->mCurrent->mValue;
}

template<typename T>
const T* RbTree<T>::CIter::operator->()
{
  return &this->mCurrent->mValue;
}

template<typename T>
typename RbTree<T>::Iter RbTree<T>::begin() const
{
  Iter begin;
  begin.mCurrent = LeftmostNode();
  return begin;
}

template<typename T>
typename RbTree<T>::Iter RbTree<T>::end() const
{
  Iter end;
  end.mCurrent = nullptr;
  return end;
}

template<typename T>
typename RbTree<T>::CIter RbTree<T>::cbegin() const
{
  CIter begin;
  begin.mCurrent = LeftmostNode();
  return begin;
}

template<typename T>
typename RbTree<T>::CIter RbTree<T>::cend() const
{
  CIter end;
  end.mCurrent = nullptr;
  return end;
}

////////////////////////////////////////////////////////////////////////////////
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

// todo: Instead of allocating and freeing memory for every node, it might be
// better to give the tree one underlying array and have all of the memory for
// the nodes come from that array. That way we can guarantee to load the minimum
// number of cache lines when accessing the tree.
template<typename T>
void RbTree<T>::Insert(const T& value)
{
  Node* newNode = alloc Node(value);
  InsertNode(newNode);
}

template<typename T>
void RbTree<T>::Insert(T&& value)
{
  Node* newNode = alloc Node(std::forward<T>(value));
  InsertNode(newNode);
}

template<typename T>
template<typename... Args>
void RbTree<T>::Emplace(Args&&... args)
{
  Node* newNode = alloc Node(std::forward<Args>(args)...);
  InsertNode(newNode);
}

template<typename T>
void RbTree<T>::Remove(const T& value)
{
  Node* node = FindNode<T>(value);
  LogAbortIf(node == nullptr, "The RbTree does not contain the value.");
  RemoveNode(node);
}

template<typename T>
void RbTree<T>::RemoveNode(Node* node)
{
  // Find out if there is a node that will take the place of the node to be
  // removed.
  Node* replace = node->FindPredecessor();
  if (replace == nullptr) {
    replace = node->mRight;
  }
  if (replace != nullptr) {
    node->mValue = std::move(replace->mValue);
    node = replace;
  }

  if (node == mHead) {
    delete mHead;
    mHead = nullptr;
    return;
  }

  // If the node to be removed is red, it will not have a child, so we can just
  // remove it. If it had a red child, we would have a double red situation. If
  // it had a black child, the black height for the tree would not be consistent
  // because the node only has one child.
  if (node->mColor == Node::Color::Red) {
    if (node->mParent->mLeft == node) {
      node->mParent->mLeft = nullptr;
    }
    else {
      node->mParent->mRight = nullptr;
    }
    delete node;
    return;
  }

  // If the node has a child, it can only be a left red child and we can move
  // that child up and change its color to black. This occurs because we choose
  // to find the predecessor when searching for a replacement node.
  if (node->mLeft != nullptr) {
    node->mLeft->mParent = node->mParent;
    if (node->mParent->mLeft == node) {
      node->mParent->mLeft = node->mLeft;
    }
    else {
      node->mParent->mRight = node->mLeft;
    }
    node->mLeft->mColor = Node::Color::Black;
    delete node;
    return;
  }

  // We have a black node with no children that needs to be removed, hence some
  // balancing needs to be done before finally removing the node.
  BalanceRemoval(node);
  if (node->mParent->mLeft == node) {
    node->mParent->mLeft = nullptr;
  }
  else {
    node->mParent->mRight = nullptr;
  }
  delete node;
}

template<typename T>
void RbTree<T>::Clear()
{
  Delete(mHead);
  mHead = nullptr;
}

template<typename T>
bool RbTree<T>::Contains(const T& value) const
{
  Node* node = FindNode<T>(value);
  return node != nullptr;
}

template<typename T>
bool RbTree<T>::HasDoubleRed()
{
  return HasDoubleRed(mHead);
}

template<typename T>
bool RbTree<T>::HasConsistentBlackHeight()
{
  // We find the expected black height by going down the left side of the tree
  // as far as we can.
  int expectedBh = 0;
  Node* node = mHead;
  while (node != nullptr) {
    if (node->mColor == Node::Color::Black) {
      ++expectedBh;
    }
    node = node->mLeft;
  }
  ++expectedBh;

  return HasBlackHeight(mHead, 0, expectedBh);
}

template<typename T>
const typename RbTree<T>::Node* RbTree<T>::GetHead() const
{
  return mHead;
}

template<typename T>
template<typename CT>
typename RbTree<T>::Node* RbTree<T>::FindNode(const CT& value) const
{
  Node* node = mHead;
  while (node != nullptr) {
    if (node->mValue > value) {
      node = node->mLeft;
    }
    else if (node->mValue < value) {
      node = node->mRight;
    }
    else {
      return node;
    }
  }
  return node;
}

template<typename T>
void RbTree<T>::InsertNode(Node* newNode)
{
  if (mHead == nullptr) {
    mHead = newNode;
    mHead->mColor = Node::Color::Black;
    return;
  }

  // Traverse the tree to find the connection where the new node will be
  // inserted.
  Node* parent = nullptr;
  Node** insertionPoint = &mHead;
  while (*insertionPoint != nullptr) {
    parent = *insertionPoint;
    if (newNode->mValue > (*insertionPoint)->mValue) {
      insertionPoint = &(*insertionPoint)->mRight;
    }
    else if (newNode->mValue < (*insertionPoint)->mValue) {
      insertionPoint = &(*insertionPoint)->mLeft;
    }
    else {
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
         child->mParent->mColor == Node::Color::Red) {
    Node* parent = child->mParent;
    Node* grandparent = parent->mParent;
    if (grandparent->mLeft == parent) {
      Node* uncle = grandparent->mRight;
      if (uncle == nullptr || uncle->mColor == Node::Color::Black) {
        // We need make sure that the child is to the left of the parent.
        if (child == parent->mRight) {
          RotateLeft(parent);
          Node* temp = child;
          child = parent;
          parent = temp;
        }

        // We need to rotate the parent to the grandparent's position because
        // the uncle is either non-existant or black.
        RotateRight(grandparent);
        grandparent->mColor = Node::Color::Red;
        parent->mColor = Node::Color::Black;
      }
      else {
        // We push down the blackness when the uncle is red.
        if (grandparent != mHead) {
          grandparent->mColor = Node::Color::Red;
        }
        parent->mColor = Node::Color::Black;
        uncle->mColor = Node::Color::Black;
      }
    }
    else {
      // This is just a reflection of the code that is used when the parent is
      // to the left of the grandparent.
      Node* uncle = grandparent->mLeft;
      if (uncle == nullptr || uncle->mColor == Node::Color::Black) {
        if (child == parent->mLeft) {
          RotateRight(parent);
          Node* temp = child;
          child = parent;
          parent = temp;
        }

        RotateLeft(grandparent);
        grandparent->mColor = Node::Color::Red;
        parent->mColor = Node::Color::Black;
      }
      else {
        if (grandparent != mHead) {
          grandparent->mColor = Node::Color::Red;
        }
        parent->mColor = Node::Color::Black;
        uncle->mColor = Node::Color::Black;
      }
    }
    child = child->mParent->mParent;
  }
}

template<typename T>
void RbTree<T>::BalanceRemoval(Node* node)
{
  // The node passed in is a double black node and we balance it out here.
  while (node->mParent != nullptr && node->mColor == Node::Color::Black) {
    Node* parent = node->mParent;
    if (parent->mLeft == node) {
      Node* sibling = node->mParent->mRight;
      if (sibling->mColor == Node::Color::Black) {
        bool redRightChild = sibling->mRight != nullptr &&
          sibling->mRight->mColor == Node::Color::Red;
        if (redRightChild) {
          // We rotate around the parent towards the double black node because
          // the sibling side has a greater black height.
          sibling->mColor = parent->mColor;
          parent->mColor = Node::Color::Black;
          sibling->mRight->mColor = Node::Color::Black;
          RotateLeft(parent);
          return;
        }
        bool redLeftChild = sibling->mLeft != nullptr &&
          sibling->mLeft->mColor == Node::Color::Red;
        if (redLeftChild) {
          // This rotates around the sibling so that the sibling's left child
          // becomes the new sibling. This new sibling will now have a right red
          // child.
          sibling->mColor = Node::Color::Red;
          sibling->mLeft->mColor = Node::Color::Black;
          RotateRight(sibling);
          continue;
        }
        if (parent->mColor == Node::Color::Red) {
          // We make the parent black and the sibling red to account for the
          // double black.
          parent->mColor = Node::Color::Black;
          sibling->mColor = Node::Color::Red;
          return;
        }

        // It appears the sibling is surrouned by black nodes, so now we make
        // the parent node the new double black node.
        sibling->mColor = Node::Color::Red;
        node = parent;
        continue;
      }
      else {
        // We need to rotate towards the double black because the sibling is red
        // and we know that it has black nodes underneath it that need to be
        // balanced to the other side of the tree.
        sibling->mColor = Node::Color::Black;
        parent->mColor = Node::Color::Red;
        RotateLeft(parent);
      }
    }
    else {
      // This code is just a reflection of the above code for handling the case
      // where the sibling is to the left of the parent.
      Node* sibling = node->mParent->mLeft;
      if (sibling->mColor == Node::Color::Black) {
        bool redLeftChild = sibling->mLeft != nullptr &&
          sibling->mLeft->mColor == Node::Color::Red;
        if (redLeftChild) {
          sibling->mColor = parent->mColor;
          parent->mColor = Node::Color::Black;
          sibling->mLeft->mColor = Node::Color::Black;
          RotateRight(parent);
          return;
        }
        bool redRightChild = sibling->mRight != nullptr &&
          sibling->mRight->mColor == Node::Color::Red;
        if (redRightChild) {
          sibling->mColor = Node::Color::Red;
          sibling->mRight->mColor = Node::Color::Black;
          RotateLeft(sibling);
          continue;
        }
        if (parent->mColor == Node::Color::Red) {
          parent->mColor = Node::Color::Black;
          sibling->mColor = Node::Color::Red;
          return;
        }
        sibling->mColor = Node::Color::Red;
        node = parent;
        continue;
      }
      else {
        sibling->mColor = Node::Color::Black;
        parent->mColor = Node::Color::Red;
        RotateRight(parent);
      }
    }
  }
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
  if (oldRoot->mRight != nullptr) {
    oldRoot->mRight->mParent = oldRoot;
  }

  // Update the connection from the parent to the new root.
  Node* parent = newRoot->mParent;
  if (parent == nullptr) {
    mHead = newRoot;
  }
  else if (parent->mLeft == oldRoot) {
    parent->mLeft = newRoot;
  }
  else {
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
  if (oldRoot->mLeft != nullptr) {
    oldRoot->mLeft->mParent = oldRoot;
  }

  Node* parent = newRoot->mParent;
  if (parent == nullptr) {
    mHead = newRoot;
  }
  else if (parent->mLeft == oldRoot) {
    parent->mLeft = newRoot;
  }
  else {
    parent->mRight = newRoot;
  }
}

template<typename T>
void RbTree<T>::SwapNodes(Node* a, Node* b)
{
  // Update all of the pointers on the nodes being swapped and the pointers on
  // nodes connected to them.
  if (b->mParent == a) {
    SwapAttachedNodePointers(a, b);
  }
  else if (a->mParent == b) {
    SwapAttachedNodePointers(b, a);
  }
  else {
    SwapDetachedNodePointers(a, b);
  }

  typename Node::Color aColor = a->mColor;
  a->mColor = b->mColor;
  b->mColor = aColor;
  if (a == mHead) {
    mHead = b;
  }
  else if (b == mHead) {
    mHead = a;
  }
}

template<typename T>
void RbTree<T>::SwapAttachedNodePointers(Node* above, Node* below)
{
  // Handle above's Parent.
  if (above->mParent != nullptr) {
    if (above->mParent->mLeft == above) {
      above->mParent->mLeft = below;
    }
    else {
      above->mParent->mRight = below;
    }
  }

  // Handle above's child that is not the below node.
  if (above->mLeft != nullptr && above->mLeft != below) {
    above->mLeft->mParent = below;
  }
  if (above->mRight != nullptr && above->mRight != below) {
    above->mRight->mParent = below;
  }

  // Handle below's children.
  if (below->mLeft != nullptr) {
    below->mLeft->mParent = above;
  }
  if (below->mRight != nullptr) {
    below->mRight->mParent = above;
  }

  // Handle above's and below's pointers.
  Node* belowLeft = below->mLeft;
  Node* belowRight = below->mRight;
  below->mParent = above->mParent;
  if (below == above->mLeft) {
    below->mLeft = above;
    below->mRight = above->mRight;
  }
  else {
    below->mRight = above;
    below->mLeft = above->mLeft;
  }
  above->mParent = below;
  above->mLeft = belowLeft;
  above->mRight = belowRight;
}

template<typename T>
void RbTree<T>::SwapDetachedNodePointers(Node* a, Node* b)
{
  // This will change all pointers that point to leaving to point to taking.
  auto takePlace = [](Node* taking, Node* leaving)
  {
    if (leaving->mParent != nullptr) {
      if (leaving->mParent->mLeft == leaving) {
        leaving->mParent->mLeft = taking;
      }
      else {
        leaving->mParent->mRight = taking;
      }
    }
    if (leaving->mLeft != nullptr) {
      leaving->mLeft->mParent = taking;
    }
    if (leaving->mRight != nullptr) {
      leaving->mRight->mParent = taking;
    }
  };
  takePlace(a, b);
  takePlace(b, a);

  // Swap the pointers on the nodes being swapped.
  Node* aParent = a->mParent;
  Node* aLeft = a->mLeft;
  Node* aRight = a->mRight;
  a->mParent = b->mParent;
  a->mLeft = b->mLeft;
  a->mRight = b->mRight;
  b->mParent = aParent;
  b->mLeft = aLeft;
  b->mRight = aRight;
}

template<typename T>
void RbTree<T>::Delete(Node* node)
{
  if (node == nullptr) {
    return;
  }
  Delete(node->mLeft);
  Delete(node->mRight);
  delete node;
}

template<typename T>
typename RbTree<T>::Node* RbTree<T>::LeftmostNode() const
{
  if (mHead == nullptr) {
    return nullptr;
  }
  Node* node = mHead;
  while (node->mLeft != nullptr) {
    node = node->mLeft;
  }
  return node;
}

template<typename T>
bool RbTree<T>::HasBlackHeight(Node* node, int currentBh, int expectedBh)
{
  if (node == nullptr) {
    ++currentBh;
    return currentBh == expectedBh;
  }

  if (node->mColor == Node::Color::Black) {
    ++currentBh;
  }
  bool correctLh = HasBlackHeight(node->mLeft, currentBh, expectedBh);
  bool correctRh = HasBlackHeight(node->mRight, currentBh, expectedBh);
  return correctLh && correctRh;
}

template<typename T>
bool RbTree<T>::HasDoubleRed(Node* node)
{
  if (node == nullptr) {
    return false;
  }
  bool doubleRed = node->mParent != nullptr &&
    node->mColor == Node::Color::Red &&
    node->mParent->mColor == Node::Color::Red;
  return doubleRed || HasDoubleRed(node->mLeft) || HasDoubleRed(node->mRight);
}

} // namespace Ds
