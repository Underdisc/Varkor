namespace Ds {

template<typename T>
const size_t HashSet<T>::smInitialBucketCount = 10;
template<typename T>
const float HashSet<T>::smGrowLoadFactor = 1.0f;

template<typename T>
void HashSet<T>::IterBase::operator++() {
  const Ds::Vector<Ds::Vector<T>>& buckets = mHashSet.mBuckets;
  if (++mIdx == buckets[mBucket].Size()) {
    ++mBucket;
    mIdx = 0;
  }
  while (mBucket < buckets.Size() && buckets[mBucket].Size() == 0) {
    ++mBucket;
  }
}

template<typename T>
void HashSet<T>::IterBase::operator--() {
  const Ds::Vector<Ds::Vector<T>>& buckets = mHashSet.mBuckets;
  while (mIdx == 0 && mBucket > 0) {
    --mBucket;
    mIdx = buckets[mBucket].Size();
  }
  if (mIdx > 0) {
    --mIdx;
  }
}

template<typename T>
bool HashSet<T>::IterBase::operator==(const IterBase& other) const {
  return &mHashSet == &other.mHashSet && mBucket == other.mBucket &&
    mIdx == other.mIdx;
}

template<typename T>
bool HashSet<T>::IterBase::operator!=(const IterBase& other) const {
  return !(*this == other);
}

template<typename T>
void HashSet<T>::IterBase::Verify() const {
  const Ds::Vector<Ds::Vector<T>>& buckets = mHashSet.mBuckets;
  if (mBucket >= buckets.Size() || mIdx >= buckets[mBucket].Size()) {
    LogAbort("Invalid iterator");
  }
}

template<typename T>
HashSet<T>::IterBase::IterBase(HashSet<T>& hashSet, size_t bucket, size_t idx):
  mHashSet(hashSet), mBucket(bucket), mIdx(idx) {}

template<typename T>
HashSet<T>::Iter::Iter(HashSet<T>& hashSet, size_t bucket, size_t idx):
  IterBase(hashSet, bucket, idx) {}

template<typename T>
HashSet<T>::CIter::CIter(HashSet<T>& hashSet, size_t bucket, size_t idx):
  IterBase(hashSet, bucket, idx) {}

template<typename T>
HashSet<T>::CIter::CIter(Iter& it):
  IterBase(it.mHashSet, it.mBucket, it.mIdx) {}

template<typename T>
T& HashSet<T>::Iter::operator*() {
  Verify();
  return mHashSet.mBuckets[mBucket][mIdx];
}

template<typename T>
T* HashSet<T>::Iter::operator->() {
  Verify();
  return &mHashSet.mBuckets[mBucket][mIdx];
}

template<typename T>
const T& HashSet<T>::CIter::operator*() const {
  Verify();
  return mHashSet.mBuckets[mBucket][mIdx];
}

template<typename T>
const T* HashSet<T>::CIter::operator->() const {
  Verify();
  return &mHashSet.mBuckets[mBucket][mIdx];
}

template<typename T>
typename HashSet<T>::Iter& HashSet<T>::Iter::operator++() {
  IterBase::operator++();
  return *this;
}

template<typename T>
typename HashSet<T>::Iter& HashSet<T>::Iter::operator--() {
  IterBase::operator--();
  return *this;
}

template<typename T>
typename HashSet<T>::Iter HashSet<T>::Iter::operator++(int) {
  Iter initialIt(*this);
  IterBase::operator++();
  return initialIt;
}

template<typename T>
typename HashSet<T>::Iter HashSet<T>::Iter::operator--(int) {
  Iter initialIt(*this);
  IterBase::operator--();
  return initialIt;
}

template<typename T>
typename HashSet<T>::CIter& HashSet<T>::CIter::operator++() {
  IterBase::operator++();
  return *this;
}

template<typename T>
typename HashSet<T>::CIter& HashSet<T>::CIter::operator--() {
  IterBase::operator--();
  return *this;
}

template<typename T>
typename HashSet<T>::CIter HashSet<T>::CIter::operator++(int) {
  CIter initialIt(*this);
  IterBase::operator++();
  return initialIt;
}

template<typename T>
typename HashSet<T>::CIter HashSet<T>::CIter::operator--(int) {
  CIter initialIt(*this);
  IterBase::operator--();
  return initialIt;
}

template<typename T>
typename HashSet<T>::Iter HashSet<T>::begin() const {
  return Iter(const_cast<HashSet<T>&>(*this), StartBucket(), 0);
}

template<typename T>
typename HashSet<T>::Iter HashSet<T>::end() const {
  return Iter(const_cast<HashSet<T>&>(*this), mBuckets.Size(), 0);
}

template<typename T>
typename HashSet<T>::CIter HashSet<T>::cbegin() const {
  CIter it(const_cast<HashSet<T>&>(*this), StartBucket(), 0);
  return it;
}

template<typename T>
typename HashSet<T>::CIter HashSet<T>::cend() const {
  CIter it(const_cast<HashSet<T>&>(*this), mBuckets.Size(), 0);
  return it;
}

template<typename T>
HashSet<T>::HashSet(): mSize(0) {}

template<typename T>
template<typename U>
typename HashSet<T>::Iter HashSet<T>::Insert(U&& key) {
  ++mSize;
  TryGrow();
  size_t bucket = Bucket(key);
  VResult<size_t> result = mBuckets[bucket].Find(key);
  LogAbortIf(result.Success(), "Key already in HashSet");
  mBuckets[bucket].Push(std::forward<U>(key));
  return Iter(*this, bucket, mBuckets[bucket].Size() - 1);
}

template<typename T>
void HashSet<T>::Clear() {
  for (Ds::Vector<T>& bucket: mBuckets) {
    bucket.Clear();
  }
}

template<typename T>
template<typename CT>
void HashSet<T>::Remove(const CT& key) {
  CIter it = Find(key);
  LogAbortIf(it == end(), "Key not in HashSet");
  it.mHashSet.mBuckets[it.mBucket].LazyRemove(it.mIdx);
  --mSize;
}

template<typename T>
template<typename CT>
typename HashSet<T>::Iter HashSet<T>::Find(const CT& key) const {
  if (mBuckets.Empty()) {
    return end();
  }
  size_t bucket = Bucket(key);
  VResult<size_t> result = mBuckets[bucket].Find(key);
  if (!result.Success()) {
    return end();
  }
  Iter it(const_cast<HashSet<T>&>(*this), bucket, result.mValue);
  return it;
}

template<typename T>
template<typename CT>
bool HashSet<T>::Contains(const CT& key) const {
  return Find(key) != cend();
}

template<typename T>
size_t HashSet<T>::Size() const {
  return mSize;
}

template<typename T>
bool HashSet<T>::Empty() const {
  return mSize == 0;
}

template<typename T>
const Ds::Vector<Ds::Vector<T>>& HashSet<T>::Buckets() const {
  return mBuckets;
}

template<typename T>
float HashSet<T>::LoadFactor() const {
  return LoadFactor(mSize);
}

template<typename T>
float HashSet<T>::LoadFactor(size_t size) const {
  return size / (float)mBuckets.Size();
}

template<typename T>
size_t HashSet<T>::StartBucket() const {
  size_t bucket = 0;
  while (bucket < mBuckets.Size() && mBuckets[bucket].Size() == 0) {
    ++bucket;
  }
  return bucket;
}

template<typename T>
void HashSet<T>::VerifySize() const {
  size_t sizeSum = 0;
  for (const Ds::Vector<T>& bucket: mBuckets) {
    sizeSum += bucket.Size();
  }
  LogAbortIf(sizeSum != mSize, "Size mismatch");
}

template<typename T>
template<typename CT>
size_t HashSet<T>::Bucket(const CT& key) const {
  return Hash(key) % mBuckets.Size();
}

template<typename T>
void HashSet<T>::TryGrow() {
  if (mBuckets.Empty()) {
    mBuckets.Resize(smInitialBucketCount);
    return;
  }
  if (LoadFactor() >= smGrowLoadFactor) {
    Grow();
  }
}

template<typename T>
void HashSet<T>::Grow() {
  Ds::Vector<Ds::Vector<T>> newBuckets;
  newBuckets.Resize(mBuckets.Size() * 2);
  for (auto it = begin(); it != end(); ++it) {
    size_t bucket = Hash(*it) % newBuckets.Size();
    newBuckets[bucket].Push(std::move(*it));
  }
  mBuckets = std::move(newBuckets);
}

} // namespace Ds
