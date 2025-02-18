namespace Ds {

template<typename T>
PoolId Pool<T>::Add(const T& element) {
  SparseId id = SparseSet::Add();
  mData.Push(element);
  return id;
}

template<typename T>
template<typename... Args>
T& Pool<T>::Request(PoolId id, Args&&... args) {
  SparseSet::Request(id);
  mData.Emplace(std::forward<Args>(args)...);
  return mData[mData.Size() - 1];
}

template<typename T>
void Pool<T>::Remove(PoolId id) {
  mData.LazyRemove(mSparse[id]);
  SparseSet::Remove(id);
}

template<typename T>
void Pool<T>::Clear() {
  mData.Clear();
  SparseSet::Clear();
}

template<typename T>
T& Pool<T>::GetWithDenseIndex(size_t denseIndex) {
  return mData[denseIndex];
}

template<typename T>
const T& Pool<T>::GetWithDenseIndex(size_t denseIndex) const {
  return mData[denseIndex];
}

template<typename T>
T& Pool<T>::operator[](PoolId id) {
  Verify(id);
  return mData[mSparse[id]];
}

template<typename T>
const T& Pool<T>::operator[](PoolId id) const {
  Verify(id);
  return mData[mSparse[id]];
}

template<typename T>
const Ds::Vector<T>& Pool<T>::Data() const {
  return mData;
}

} // namespace Ds
