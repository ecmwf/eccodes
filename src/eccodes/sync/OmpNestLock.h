#pragma once

#include <omp.h>
// #include <sanitizer/tsan_interface.h>

namespace eccodes::sync {

// Note:
// The GCC15 thread sanitiser doesn't fully support OpenMP.
// See commented calls to tsan...

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
        // __tsan_acquire(&mutex_);
    }

    void unlock() {
        // __tsan_release(&mutex_);
        omp_unset_nest_lock(&mutex_);
    }
  private:
    omp_nest_lock_t mutex_;
};

} // namespace eccodes::sync
