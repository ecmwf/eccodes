#pragma once

#include <omp.h>

#if defined(__SANITIZE_THREAD__) || (defined(__has_feature) && __has_feature(thread_sanitizer))
#include <sanitizer/tsan_interface.h>
#define ECCODES_TSAN_ENABLED 1
#endif

namespace eccodes::sync {

// Note:
// The GCC15 thread sanitiser doesn't fully support OpenMP.
// Use TSan annotations to inform it about synchronization.

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
#if ECCODES_TSAN_ENABLED
        __tsan_acquire(&mutex_);
#endif
    }

    void unlock() {
#if ECCODES_TSAN_ENABLED
        __tsan_release(&mutex_);
#endif
        omp_unset_nest_lock(&mutex_);
    }
  private:
    omp_nest_lock_t mutex_;
};

} // namespace eccodes::sync
