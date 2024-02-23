namespace Ds {

template<typename T>
PoolId Pool<T>::Add(const T& element)
{
  mData.Push(element);
  if (mDense.Size() < mData.Size()) {
    mSparse.Push(mSparse.Size());
    mDense.Push(mDense.Size());
  }
  else {
    mSparse[mDense[mData.Size() - 1]] = mData.Size() - 1;
  }
  return mDense[mData.Size() - 1];
}

template<typename T>
void Pool<T>::Remove(PoolId id)
{
  mData.LazyRemove(mSparse[id]);
  mSparse[mDense[mData.Size()]] = mSparse[id];
  mDense.Swap(mSparse[id], mData.Size());
  mSparse[id] = nInvalidPoolId;
}

template<typename T>
bool Pool<T>::Valid(PoolId id)
{
  return mSparse[id] != nInvalidPoolId;
}

template<typename T>
T& Pool<T>::GetWithDenseIndex(size_t denseIndex)
{
  return mData[denseIndex];
}

template<typename T>
T& Pool<T>::operator[](PoolId id)
{
  return mData[mSparse[id]];
}

template<typename T>
const T& Pool<T>::operator[](PoolId id) const
{
  return mData[mSparse[id]];
}

template<typename T>
size_t Pool<T>::Size() const
{
  return mData.Size();
}

template<typename T>
const Ds::Vector<T>& Pool<T>::Data() const
{
  return mData;
}

template<typename T>
const Ds::Vector<PoolId>& Pool<T>::Dense() const
{
  return mDense;
}

template<typename T>
const Ds::Vector<int>& Pool<T>::Sparse() const
{
  return mSparse;
}

} // namespace Ds