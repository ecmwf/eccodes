#pragma once

#if GRIB_PTHREADS || GRIB_OMP_THREADS
    #include <mutex>
#endif

#if GRIB_PTHREADS
    #include "PThreadMutex.h"
#elif GRIB_OMP_THREADS
    #include "OmpNestLock.h"
#endif

namespace eccodes::sync {

#if GRIB_PTHREADS
    using Mutex = PThreadMutex;
#elif GRIB_OMP_THREADS
    using Mutex = OmpNestLock;
#else
    class Mutex {};
#endif

#if GRIB_PTHREADS || GRIB_OMP_THREADS
    template <typename MutexType>
    // using LockGuard = std::lock_guard<MutexType>;
    class LockGuard {
      public:
        explicit LockGuard(MutexType& mutex) : mutex_(mutex) {
            mutex_.lock();
        }
        ~LockGuard() {
            mutex_.unlock();
        }
      private:
        MutexType& mutex_;
    };
#else
    // Dummy implementations when threading is disabled
    // If optimisation level is equal or higher than -O1, the following
    // code will be optimized away.
    template <typename MutexType>
    class LockGuard {
      public:
        explicit LockGuard(MutexType&) {}
        ~LockGuard() {}
    };
#endif

} // namespace eccodes::sync
