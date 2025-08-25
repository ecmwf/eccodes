#include <unordered_map>

#if GRIB_PTHREADS || GRIB_OMP_THREADS || STL_THREADS
#include "sync/Mutex.h"
#endif

namespace eccodes {

#if GRIB_PTHREADS || GRIB_OMP_THREADS || STL_THREADS

template <typename K, typename V>
class ThreadSafeUnorderedMap
{
  public:
    using KeyType = K;
    using ValueType = V; // Use shared_ptr for thread safety
    using MapType = std::unordered_map<KeyType, ValueType>;

    ThreadSafeUnorderedMap() = default;

    void insert(const KeyType& key, const ValueType& value) {
        sync::LockGuard<sync::Mutex> lock(mutex_);
        map_[key] = value;
    }

    ValueType get(const KeyType& key) {
        sync::LockGuard<sync::Mutex> lock(mutex_);
        auto it = map_.find(key);
        return (it != map_.end()) ? it->second : nullptr;
    }
    bool contains(const KeyType& key) {
        sync::LockGuard<sync::Mutex> lock(mutex_);
        return map_.find(key) != map_.end();
    }
    void remove(const KeyType& key) {
        sync::LockGuard<sync::Mutex> lock(mutex_);
        map_.erase(key);
    }
    void clear() {
        sync::LockGuard<sync::Mutex> lock(mutex_);
        map_.clear();
    }
    MapType getMap() {
        sync::LockGuard<sync::Mutex> lock(mutex_);
        return map_;
    }
    std::size_t size() const {
        sync::LockGuard<sync::Mutex> lock(mutex_);
        return map_.size();
    }
    typename MapType::iterator find(const KeyType& key) {
        sync::LockGuard<sync::Mutex> lock(mutex_);
        return map_.find(key);
    }
    typename MapType::iterator end() {
        sync::LockGuard<sync::Mutex> lock(mutex_);
        return map_.end();
    }
    ValueType& operator[](const KeyType& key) {
        sync::LockGuard<sync::Mutex> lock(mutex_);
        return map_[key];
    }
  private:
    mutable sync::Mutex mutex_;
    MapType map_;
};
#else
template <typename K, typename V>
class ThreadSafeUnorderedMap
{
  public:
    using KeyType = K;
    using ValueType = V;
    using MapType = std::unordered_map<KeyType, ValueType>;
    ThreadSafeUnorderedMap() = default;
    void insert(const KeyType& key, const ValueType& value) {
        map_[key] = value;
    }
    ValueType get(const KeyType& key) {
        auto it = map_.find(key);
        return (it != map_.end()) ? it->second : nullptr;
    }
    bool contains(const KeyType& key) {
        return map_.find(key) != map_.end();
    }
    void remove(const KeyType& key) {
        map_.erase(key);
    }
    void clear() {
        map_.clear();
    }
    MapType getMap() {
        return map_;
    }
    std::size_t size() const {
        return map_.size();
    }
    typename MapType::iterator find(const KeyType& key) {
        return map_.find(key);
    }
    typename MapType::iterator end() {
        return map_.end();
    }
    ValueType& operator[](const KeyType& key) {
        return map_[key];
    }
  private:
    MapType map_;
};
#endif

template <typename K, typename V>
using Map = ThreadSafeUnorderedMap<K, V>;

} // namespace eccodes
