template<typename T>
void PrintVector(const Ds::Vector<T>& vector, bool stats)
{
  if (stats)
  {
    std::cout << "Size: " << vector.Size() << std::endl;
    std::cout << "Capactiy: " << vector.Capacity() << std::endl;
  }
  std::cout << vector << std::endl;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Ds::Vector<T>& vector)
{
  if (vector.Size() == 0)
  {
    os << "[]";
    return os;
  }
  os << "[";
  for (int i = 0; i < vector.Size() - 1; ++i)
  {
    os << vector[i] << ", ";
  }
  os << vector[vector.Size() - 1] << "]";
  return os;
}

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
  const Ds::RbTree<T>::Node* head = rbTree.GetHead();
  if (head == nullptr)
  {
    std::cout << "Empty Tree" << std::endl;
    return;
  }
  BTreeIndenter indenter;
  PrintRbTreeNode<T>(*head, indenter);
}

template<typename K, typename V>
std::ostream& operator<<(std::ostream& os, const Ds::KvPair<K, V>& kv)
{
  os << "[" << kv.Key() << ", " << kv.mValue << "]";
  return os;
}

template<typename K, typename V>
void PrintMap(const Ds::Map<K, V>& map)
{
  const Ds::RbTree<Ds::KvPair<K, V>>::Node* head = map.GetHead();
  if (head == nullptr)
  {
    std::cout << "Empty Map" << std::endl;
    return;
  }
  BTreeIndenter indenter;
  PrintRbTreeNode<Ds::KvPair<K, V>>(*head, indenter);
}
