// lockpool.h (2013-07-15)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_UTIL_DETAIL_ATOMIC_LOCKPOOL_H_
#define LDD_UTIL_DETAIL_ATOMIC_LOCKPOOL_H_

#ifndef LDD_UTIL_ATOMIC_FLAG_LOCK_FREE
#include "ldd/util/mutex.h"
#endif

namespace ldd {
namespace util {
namespace detail {

#ifndef LDD_UTIL_ATOMIC_FLAG_LOCK_FREE
class LockPool {
public:
    typedef Mutex Lock;
    class ScopedLock {
        Lock* mtx_;
        ScopedLock(ScopedLock const&);
        ScopedLock& operator=(ScopedLock const&);
    public:
        explicit ScopedLock(const volatile void* addr): mtx_(GetLockFor(addr)) {
            mtx_->Lock();
        }
        ~ScopedLock() {
            mtx_->Unlock();
        }
    };
private:
    static Lock* GetLockFor(const volatile void* addr);
};

#else

class LockPool {
public:
    typedef AtomicFlag Lock;

    class ScopedLock {
        AtomicFlag* flag_;
        uint8_t padding[128 - sizeof(AtomicFlag)];
        ScopedLock(const ScopedLock&);
        ScopedLock& operator=(const ScopedLock&);
    public:
        explicit ScopedLock(const volatile void* addr): flag_(GetLockFor(addr)){
            for (; flag_->TestAndSet(MemoryOrder::kAcquire); ) {
#if defined(LDD_UTIL_ATOMIC_X86_PAUSE)
                LDD_UTIL_ATOMIC_X86_PAUSE();
#endif
            }
        }
        ~ScopedLock(void) {
            flag_->Clear(MemoryOrder::kRelease);
        }
    };
private:
    static Lock* GetLockFor(const volatile void* addr);
};
#endif

} // namespace detail
} // namespace util
} // namespace ldd

#endif // LDD_UTIL_DETAIL_ATOMIC_LOCKPOOL_H_
