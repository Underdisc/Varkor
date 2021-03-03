namespace World {

template<typename T>
Table::Visitor<T>::Visitor(const Table* table): mTable(table), mCurrentIndex(0)
{
  ReachValidComponent();
}

template<typename T>
MemRef Table::Visitor<T>::CurrentOwner()
{
  return mTable->GetOwner(mCurrentIndex);
}

template<typename T>
T& Table::Visitor<T>::CurrentComponent()
{
  return *(T*)mTable->GetData(mCurrentIndex);
}

template<typename T>
void Table::Visitor<T>::Next()
{
  ++mCurrentIndex;
  ReachValidComponent();
}

template<typename T>
bool Table::Visitor<T>::End()
{
  return mTable == nullptr || mCurrentIndex >= mTable->Size();
}

template<typename T>
void Table::Visitor<T>::ReachValidComponent()
{
  if (mTable == nullptr)
  {
    return;
  }
  while (mTable->GetOwner(mCurrentIndex) == nInvalidMemRef &&
         mCurrentIndex < mTable->Size())
  {
    ++mCurrentIndex;
  }
}

template<typename T>
Table::Visitor<T> Table::CreateVisitor() const
{
  return Visitor<T>(this);
}

} // namespace World
