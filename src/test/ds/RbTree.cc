#include <iostream>
#include <string>

#include "debug/MemLeak.h"
#include "ds/RbTree.h"

struct BTreeIndenter
{
  BTreeIndenter(): mDepth(0), mIndent("") {}

  void PushRight()
  {
    mIndent += "|  ";
    ++mDepth;
  }
  void PushRightNullLeft()
  {
    mIndent += "   ";
    ++mDepth;
  }
  void PopRight()
  {
    mIndent.erase(mIndent.size() - 3);
    --mDepth;
  }

  void PushLeft()
  {
    mIndent += " ";
    ++mDepth;
  }
  void PopLeft()
  {
    mIndent.erase(mIndent.size() - 1);
    --mDepth;
  }

  friend std::ostream& operator<<(std::ostream& os, BTreeIndenter& indenter)
  {
    if (indenter.mDepth > 0)
    {
      indenter.mIndent[indenter.mIndent.size() - 1] = '\\';
      os << indenter.mIndent;
      indenter.mIndent[indenter.mIndent.size() - 1] = ' ';
    }
    return os;
  }

  int mDepth;
  std::string mIndent;
};

template<typename T>
void PrintRbTreeNode(
  const typename Ds::RbTree<T>::Node& node, BTreeIndenter& indenter)
{
  // We first print the current node we are on with the proper indent.
  std::cout << indenter;
  if (node.mColor == Ds::RbTree<T>::Node::Color::Red)
  {
    std::cout << "R: " << node.mValue << std::endl;
  } else
  {
    std::cout << "B: " << node.mValue << std::endl;
  }

  // We then print out the right and left nodes and apply an indent that depends
  // on the side of the tree that is traversed.
  if (node.mRight != nullptr)
  {
    if (node.mLeft == nullptr)
    {
      indenter.PushRightNullLeft();
    } else
    {
      indenter.PushRight();
    }
    PrintRbTreeNode<T>(*node.mRight, indenter);
    indenter.PopRight();
  }
  if (node.mLeft != nullptr)
  {
    indenter.PushLeft();
    PrintRbTreeNode<T>(*node.mLeft, indenter);
    indenter.PopLeft();
  }
}

template<typename T>
void PrintRbTree(const Ds::RbTree<T>& rbTree)
{
  const Ds::RbTree<T>::Node& head = *rbTree.GetHead();
  BTreeIndenter indenter;
  PrintRbTreeNode<T>(head, indenter);
}

void LeftInsert()
{
  // Every time a value is inserted, it will become the leftmost leaf node.
  std::cout << "<= LeftInsert =>" << std::endl;
  Ds::RbTree<int> tree;
  for (int i = 20; i > 0; --i)
  {
    tree.Insert(i);
  }
  PrintRbTree(tree);
  std::cout << std::endl;
}

void RightInsert()
{
  // Every time a value is inserted, it will become the rightmost leaf node.
  std::cout << "<= RightInsert =>" << std::endl;
  Ds::RbTree<int> tree;
  for (int i = 0; i < 20; ++i)
  {
    tree.Insert(i);
  }
  PrintRbTree(tree);
  std::cout << std::endl;
}

void ExplicitInsert()
{
  // The insertions for this tree make use of every possible type of
  // transforation used when balancing the tree.
  std::cout << "<= ExplicitInsert =>" << std::endl;
  int sequence[] = {18, 5,  10, 15, 16, 9,  6,  4,  2, 19, 20, 21,
                    22, 23, 24, 25, 26, 27, 28, 29, 1, 3,  0};
  int sequenceSize = sizeof(sequence) / sizeof(int);
  Ds::RbTree<int> tree;
  for (int i = 0; i < sequenceSize; ++i)
  {
    tree.Insert(sequence[i]);
  }
  PrintRbTree(tree);
}

int main()
{
  InitMemLeakOutput();
  LeftInsert();
  RightInsert();
  ExplicitInsert();
}
