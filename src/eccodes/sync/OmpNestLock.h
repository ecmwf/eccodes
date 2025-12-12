#pragma once

#include <omp.h>

namespace eccodes::sync {

class OmpNestLock {
  public:
    OmpNestLock() {
        omp_init_nest_lock(&mutex_);
    }

    ~OmpNestLock() {
        omp_destroy_nest_lock(&mutex_);
    }

    void lock() {
        omp_set_nest_lock(&mutex_);
    }

    void unlock() {
        omp_unset_nest_lock(&mutex_);
    }
  private:
    omp_nest_lock_t mutex_;
};

} // namespace eccodes::sync
