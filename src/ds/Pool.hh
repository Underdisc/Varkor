namespace Ds {

template<typename T>
PoolId Pool<T>::Add(const T& element)
{
  mData.Push(element);
  if (mDense.Size() < mData.Size()) {
    mSparse.Push((int)mSparse.Size());
    mDense.Push((PoolId)mDense.Size());
  }
  else {
    mSparse[mDense[mData.Size() - 1]] = (int)mData.Size() - 1;
  }
  return mDense[mData.Size() - 1];
}

template<typename T>
T& Pool<T>::Request(PoolId id)
{
  // Ensure that the sparse array is large enough to include the requested id.
  if (id >= mSparse.Size()) {
    size_t oldSize = mSparse.Size();
    size_t newSize = id + 1;
    mSparse.Resize(newSize);
    for (size_t i = oldSize; i < newSize; ++i) {
      mSparse[i] = nInvalidPoolId;
    }
  }

  if (mSparse[id] != nInvalidPoolId) {
    LogAbort("The requested id is already being used.");
  }

  // Oblige the request.
  int denseIndex = (int)mDense.Size();
  mSparse[id] = denseIndex;
  mDense.Push(id);
  mData.Resize(mDense.Size());
  return mData[denseIndex];
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
void Pool<T>::Clear()
{
  mData.Clear();
  mSparse.Clear();
  mDense.Clear();
}

template<typename T>
bool Pool<T>::Valid(PoolId id) const
{
  return id >= 0 && id < mSparse.Size() && mSparse[id] != nInvalidPoolId;
}

template<typename T>
void Pool<T>::Verify(PoolId id) const
{
  LogAbortIf(!Valid(id), "The provided PoolId is invalid.");
}

template<typename T>
T& Pool<T>::GetWithDenseIndex(size_t denseIndex)
{
  return mData[denseIndex];
}

template<typename T>
const T& Pool<T>::GetWithDenseIndex(size_t denseIndex) const
{
  return mData[denseIndex];
}

template<typename T>
T& Pool<T>::operator[](PoolId id)
{
  Verify(id);
  return mData[mSparse[id]];
}

template<typename T>
const T& Pool<T>::operator[](PoolId id) const
{
  Verify(id);
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