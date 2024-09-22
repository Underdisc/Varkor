#include <iomanip>

template<typename T>
std::ostream& operator<<(std::ostream& os, const Ds::Vector<T>& vector)
{
  if (vector.Size() == 0) {
    os << "[]";
    return os;
  }
  os << "[";
  for (int i = 0; i < vector.Size() - 1; ++i) {
    os << vector[i] << ", ";
  }
  os << vector[vector.Size() - 1] << "]";
  return os;
}

template<typename K, typename V>
std::ostream& operator<<(std::ostream& os, const Ds::KvPair<K, V>& kv)
{
  os << "[" << kv.Key() << ", " << kv.mValue << "]";
  return os;
}

template<typename T>
void PrintVector(const Ds::Vector<T>& vector, bool stats)
{
  if (stats) {
    std::cout << "Size: " << vector.Size() << std::endl;
    std::cout << "Capactiy: " << vector.Capacity() << std::endl;
  }
  std::cout << vector << std::endl;
}

template<typename T>
void PrintList(const Ds::List<T>& list)
{
  std::cout << "-List-\n";
  std::cout << "Size: " << list.Size();
  const size_t rowCount = 5;
  size_t elementIndex = 0;
  for (const T& value : list) {
    size_t columnIndex = elementIndex % rowCount;
    if (columnIndex == 0) {
      std::cout << "\n"
                << std::setfill('0') << std::setw(2) << elementIndex << ": ";
    }
    else {
      std::cout << ", ";
    }
    std::cout << value;
    ++elementIndex;
  }
  std::cout << "\n";
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
    if (indenter.mDepth > 0) {
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
  if (node.mColor == Ds::RbTree<T>::Node::Color::Red) {
    std::cout << "R: " << node.mValue << std::endl;
  }
  else {
    std::cout << "B: " << node.mValue << std::endl;
  }

  // We then print out the right and left nodes and apply an indent that depends
  // on the side of the tree that is traversed.
  if (node.mRight != nullptr) {
    if (node.mLeft == nullptr) {
      indenter.PushRightNullLeft();
    }
    else {
      indenter.PushRight();
    }
    PrintRbTreeNode<T>(*node.mRight, indenter);
    indenter.PopRight();
  }
  if (node.mLeft != nullptr) {
    indenter.PushLeft();
    PrintRbTreeNode<T>(*node.mLeft, indenter);
    indenter.PopLeft();
  }
}

template<typename T>
void PrintRbTree(const Ds::RbTree<T>& rbTree)
{
  const typename Ds::RbTree<T>::Node* head = rbTree.GetHead();
  if (head == nullptr) {
    std::cout << "Empty Tree" << std::endl;
    return;
  }
  BTreeIndenter indenter;
  PrintRbTreeNode<T>(*head, indenter);
}

template<typename K, typename V>
void PrintMap(const Ds::Map<K, V>& map)
{
  const typename Ds::RbTree<Ds::KvPair<K, V>>::Node* head = map.GetHead();
  if (head == nullptr) {
    std::cout << "Empty Map" << std::endl;
    return;
  }
  BTreeIndenter indenter;
  PrintRbTreeNode<Ds::KvPair<K, V>>(*head, indenter);
}

template<typename T>
void PrintPool(const Ds::Pool<T>& pool)
{
  // The width of each column is chosen based on the number of charaters in the
  // words sparse, dense, and data.
  int widths[3] = {6, 5, 4};

  // Print the header.
  // clang-format off
  std::cout << std::left
            << '+' << std::setw(widths[0]) << "Sparse"
            << '+' << std::setw(widths[1]) << "Dense"
            << '+' << std::setw(widths[2]) << "Data"
            << "+\n";
  // clang-format on
  if (pool.Data().Size() == 0) {
    return;
  }

  auto printRow = [&](int index, char* bars, int* barCount, int nextBarCount)
  {
    if (*barCount >= 2) {
      std::stringstream sparseElementStream;
      sparseElementStream << pool.Sparse()[index];
      if (pool.Valid(index)) {
        sparseElementStream << "*";
      }
      else {
        sparseElementStream << "-";
      }
      std::cout << bars[0] << std::right << std::setw(widths[0])
                << sparseElementStream.str() << bars[1];
    }
    if (*barCount >= 3) {
      std::cout << std::right << std::setw(widths[1]) << pool.Dense()[index]
                << bars[2];
    }
    if (*barCount >= 4) {
      std::cout << std::right << std::setw(widths[2]) << pool.Data()[index]
                << bars[3];
    }
    std::cout << '\n';
    *barCount = nextBarCount;
  };

  // Print all of the rows.
  char bars[4] = {'|', '|', '|', '|'};
  int barCount = 4;
  int nextBarCount = barCount;
  for (int i = 0; i < pool.Data().Size() - 1; ++i) {
    printRow(i, bars, &barCount, nextBarCount);
  }
  --nextBarCount;
  bars[3] = '+';
  for (int i = pool.Data().Size() - 1; i < pool.Dense().Size() - 1; ++i) {
    printRow(i, bars, &barCount, nextBarCount);
  }
  --nextBarCount;
  bars[2] = '+';
  for (int i = pool.Dense().Size() - 1; i < pool.Sparse().Size() - 1; ++i) {
    printRow(i, bars, &barCount, nextBarCount);
  }
  nextBarCount = 0;
  bars[1] = '+';
  bars[0] = '+';
  printRow(pool.Sparse().Size() - 1, bars, &barCount, nextBarCount);
}
