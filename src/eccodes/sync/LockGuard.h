#pragma once

namespace eccodes::sync {

template <typename MutexType>
class LockGuard {
  public:
    explicit LockGuard(MutexType& m) : mutex_{m} {
        mutex_.lock();
    }
    ~LockGuard() {
        mutex_.unlock();
    }
  private:
    MutexType& mutex_;
};

} // namespace eccodes::sync
