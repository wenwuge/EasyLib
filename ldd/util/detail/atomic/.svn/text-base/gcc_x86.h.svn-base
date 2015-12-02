// gcc_x86.h (2013-07-10)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_UTIL_DETAIL_ATOMIC_GCC_X86_H_
#define LDD_UTIL_DETAIL_ATOMIC_GCC_X86_H_

namespace ldd {
namespace util {
namespace detail {

#if defined(__x86_64__) || defined(__SSE2__)
# define LDD_UTIL_ATOMIC_X86_FENCE_INSTR "mfence\n"
#else
# define LDD_UTIL_ATOMIC_X86_FENCE_INSTR "lock ; addl $0, (%%esp)\n"
#endif

#define LDD_UTIL_ATOMIC_X86_PAUSE() __asm__ __volatile__ ("pause\n")

inline void FenceBefore(MemoryOrder::Type order) {
    switch (order) {
    case MemoryOrder::kRelaxed:
    case MemoryOrder::kAcquire:
    case MemoryOrder::kConsume:
        break;
    case MemoryOrder::kRelease:
    case MemoryOrder::kAcquireRelease:
        __asm__ __volatile__ ("" ::: "memory");
        break;
    case MemoryOrder::kSequential:
        __asm__ __volatile__ ("" ::: "memory");
        break;
    default:;
    }
}

inline void FenceAfter(MemoryOrder::Type order) {
    switch (order) {
    case MemoryOrder::kRelaxed:
    case MemoryOrder::kRelease:
        break;
    case MemoryOrder::kAcquire:
    case MemoryOrder::kAcquireRelease:
        __asm__ __volatile__ ("" ::: "memory");
        break;
    case MemoryOrder::kConsume:
        break;
    case MemoryOrder::kSequential:
        __asm__ __volatile__ ("" ::: "memory");
        break;
    default:;
    }
}

inline void FenceAfterLoad(MemoryOrder::Type order) {
    switch (order) {
    case MemoryOrder::kRelaxed:
    case MemoryOrder::kRelease:
        break;
    case MemoryOrder::kAcquire:
    case MemoryOrder::kAcquireRelease:
        __asm__ __volatile__ ("" ::: "memory");
        break;
    case MemoryOrder::kConsume:
        break;
    case MemoryOrder::kSequential:
        __asm__ __volatile__ ("" ::: "memory");
        break;
    default:;
    }
}

inline void FenceBeforeStore(MemoryOrder::Type order) {
    switch (order) {
    case MemoryOrder::kRelaxed:
    case MemoryOrder::kAcquire:
    case MemoryOrder::kConsume:
        break;
    case MemoryOrder::kRelease:
    case MemoryOrder::kAcquireRelease:
        __asm__ __volatile__ ("" ::: "memory");
        break;
    case MemoryOrder::kSequential:
        __asm__ __volatile__ ("" ::: "memory");
        break;
    default:;
    }
}

inline void FenceAfterStore(MemoryOrder::Type order) {
    switch (order) {
    case MemoryOrder::kRelaxed:
    case MemoryOrder::kRelease:
        break;
    case MemoryOrder::kAcquire:
    case MemoryOrder::kAcquireRelease:
        __asm__ __volatile__ ("" ::: "memory");
        break;
    case MemoryOrder::kConsume:
        break;
    case MemoryOrder::kSequential:
        __asm__ __volatile__ ("" ::: "memory");
        break;
    default:;
    }
}

} // namespace detail

class AtomicFlag {
private:
    AtomicFlag(const AtomicFlag&);
    AtomicFlag& operator=(const AtomicFlag&);
    uint32_t v_;
public:
    AtomicFlag(void) : v_(0) {}

    bool
    TestAndSet(MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        uint32_t v = 1;
        detail::FenceBefore(order);
        __asm__ __volatile__ (
            "xchgl %0, %1"
            : "+r" (v), "+m" (v_)
        );
        detail::FenceAfter(order);
        return v;
    }

    void
    Clear(MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        if (order == MemoryOrder::kSequential) {
            uint32_t v = 0;
            __asm__ __volatile__ (
                "xchgl %0, %1"
                : "+r" (v), "+m" (v_)
            );
        } else {
            detail::FenceBefore(order);
            v_ = 0;
        }
    }
};

} // namespace util
} // namespace ldd

#define LDD_UTIL_ATOMIC_FLAG_LOCK_FREE 1
#include "ldd/util/detail/atomic/base.h"

namespace ldd {
namespace util {
namespace detail {

template<typename T, bool Sign>
class AtomicBase<T, int, 1, Sign>
{
    typedef AtomicBase This;
    typedef T Value;
    typedef T Difference;
public:
    AtomicBase(void) {}
    explicit AtomicBase(Value v) : v_(v) {}

    void
    Store(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        if (order != MemoryOrder::kSequential) {
            FenceBefore(order);
            const_cast<volatile Value&>(v_) = v;
        } else {
            Exchange(v, order);
        }
    }

    Value
    Load(MemoryOrder::Type order = MemoryOrder::kSequential) const volatile {
        Value v = const_cast<const volatile Value&>(v_);
        FenceAfterLoad(order);
        return v;
    }
    
    Value
    FetchAdd(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        FenceBefore(order);
        __asm__ (
                "lock ; xaddb %0, %1"
                : "+q" (v), "+m" (v_)
        );
        FenceAfter(order);
        return v;
    }
    
    Value
    FetchSub(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        return FetchAdd(-v, order);
    }

    Value
    Exchange(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        FenceBefore(order);
        __asm__ (
                "xchgb %0, %1"
                : "+q" (v), "+m" (v_)
        );
        FenceAfter(order);
        return v;
    }

    bool
    CompareExchangeStrong(Value* expected, Value desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        Value previous = *expected;
        FenceBefore(success_order);
        __asm__ (
                "lock ; cmpxchgb %2, %1"
                : "+a" (previous), "+m" (v_)
                : "q" (desired)
        );
        bool success = (previous == *expected);
        if (success) {
            FenceAfter(success_order);
        } else {
            FenceAfter(failure_order);
        }
        *expected = previous;
        return success;
    }

    bool
    CompareExchangeWeak(Value* expected, Value desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        return CompareExchangeStrong(expected, desired,
                success_order, failure_order);
    }

    Value
    FetchAnd(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        Value tmp = Load(MemoryOrder::kRelaxed);
        for (;!CompareExchangeWeak(&tmp, tmp&v, order, MemoryOrder::kRelaxed);){
            LDD_UTIL_ATOMIC_X86_PAUSE();
        }
        return tmp;
    }

    Value
    FetchOr(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        Value tmp = Load(MemoryOrder::kRelaxed);
        for (;!CompareExchangeWeak(tmp, tmp|v, order, MemoryOrder::kRelaxed);){
            LDD_UTIL_ATOMIC_X86_PAUSE();
        }
        return tmp;
    }

    Value
    FetchXor(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        Value tmp = Load(MemoryOrder::kRelaxed);
        for (;!CompareExchangeWeak(tmp, tmp^v, order, MemoryOrder::kRelaxed);){
            LDD_UTIL_ATOMIC_X86_PAUSE();
        }
        return tmp;
    }

    bool IsLockFree(void) const volatile {
        return true;
    }

    LDD_UTIL_ATOMIC_DECLARE_INTEGRAL_OPERATORS
private:
    AtomicBase(const AtomicBase&);
    void operator=(const AtomicBase&);
    Value v_;
};

template<typename T, bool Sign>
class AtomicBase<T, int, 2, Sign>
{
    typedef AtomicBase This;
    typedef T Value;
    typedef T Difference;
public:
    AtomicBase(void) {}
    explicit AtomicBase(Value v) : v_(v) {}

    void
    Store(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        if (order == MemoryOrder::kSequential) {
            FenceBefore(order);
            const_cast<volatile Value&>(v_) = v;
        } else {
            Exchange(v, order);
        }
    }
    
    Value
    Load(MemoryOrder::Type order = MemoryOrder::kSequential) const volatile {
        Value v = const_cast<volatile Value&>(v_);
        FenceAfterLoad(order);
        return v;
    }

    Value
    FetchAdd(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        FenceBefore(order);
        __asm__ (
                "lock ; xaddw %0, %1"
                : "+q" (v), "+m" (v_)
        );
        FenceAfter(order);
        return v;
    }

    Value
    FetchSub(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        return FetchAdd(-v, order);
    }

    Value
    Exchange(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        FenceBefore(order);
        __asm__(
                "xchgw %0, %1"
                : "+q" (v), "+m" (v_)
        );
        FenceAfter(order);
        return v;
    }

    bool
    CompareExchangeStrong(Value* expected, Value desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        Value previous = *expected;
        FenceBefore(success_order);
        __asm__(
                "lock ; cmpxchgw %2, %1"
                : "+a" (previous), "+m" (v_)
                : "q" (desired)
        );
        bool success = (previous == *expected);
        if (success) {
            FenceAfter(success_order);
        } else {
            FenceAfter(failure_order);
        }
        *expected = previous;
        return success;
    }

    bool
    CompareExchangeWeak(Value* expected, Value desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        return CompareExchangeStrong(expected, desired,
                success_order, failure_order);
    }

    Value
    FetchAnd(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        Value tmp = Load(MemoryOrder::kRelaxed);
        for (; !CompareExchangeWeak(tmp, tmp&v, order, MemoryOrder::kRelaxed);){
            LDD_UTIL_ATOMIC_X86_PAUSE();
        }
        return tmp;
    }

    Value
    FetchOr(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        Value tmp = Load(MemoryOrder::kRelaxed);
        for (; !CompareExchangeWeak(tmp, tmp|v, order, MemoryOrder::kRelaxed);){
            LDD_UTIL_ATOMIC_X86_PAUSE();
        }
        return tmp;
    }

    Value
    FetchXor(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        Value tmp = Load(MemoryOrder::kRelaxed);
        for (; !CompareExchangeWeak(tmp, tmp^v, order, MemoryOrder::kRelaxed);){
            LDD_UTIL_ATOMIC_X86_PAUSE();
        }
        return tmp;
    }

    bool IsLockFree(void) const volatile {
        return true;
    }
    
    LDD_UTIL_ATOMIC_DECLARE_INTEGRAL_OPERATORS
private:
    AtomicBase(const AtomicBase&);
    void operator=(const AtomicBase&);
    Value v_;
};

template<typename T, bool Sign>
class AtomicBase<T, int, 4, Sign>
{
    typedef AtomicBase This;
    typedef T Value;
    typedef T Difference;
public:
    AtomicBase(void) {}
    explicit AtomicBase(Value v) : v_(v) {}

    void 
    Store(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        if (order != MemoryOrder::kSequential) {
            FenceBefore(order);
            const_cast<volatile Value&>(v_) = v;
        } else {
            Exchange(v, order);
        }
    }

    Value
    Load(MemoryOrder::Type order = MemoryOrder::kSequential) const volatile {
        Value v = const_cast<const volatile Value&>(v_);
        FenceAfterLoad(order);
        return v;
    }

    Value
    FetchAdd(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        FenceBefore(order);
        __asm__(
                "lock ; xaddl %0, %1"
                : "+r" (v), "+m" (v_)
        );
        FenceAfter(order);
        return v;
    }

    Value
    FetchSub(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        return FetchAdd(-v, order);
    }
    
    Value
    Exchange(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        FenceBefore(order);
        __asm__(
                "xchg %0, %1"
                : "+r" (v), "+m" (v_)
        );
        FenceAfter(order);
        return v;
    }

    bool
    CompareExchangeStrong(Value* expected, Value desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        Value previous = *expected;
        FenceBefore(success_order);
        __asm__(
                "lock ; cmpxchgl %2, %1"
                : "+a" (previous), "+m" (v_)
                : "r" (desired)
        );
        bool success = (previous == *expected);
        if (success) {
            FenceAfter(success_order);
        } else {
            FenceAfter(failure_order);
        }
        *expected = previous;
        return success;
    }

    bool
    CompareExchangeWeak(Value* expected, Value desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        return CompareExchangeStrong(expected, desired,
                success_order, failure_order);
    }

    Value
    FetchAnd(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        Value tmp = Load(MemoryOrder::kRelaxed);
        for (; !CompareExchangeWeak(tmp, tmp&v, order, MemoryOrder::kRelaxed);){
            LDD_UTIL_ATOMIC_X86_PAUSE();
        }
        return tmp;
    }

    Value
    FetchOr(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        Value tmp = Load(MemoryOrder::kRelaxed);
        for (; !CompareExchangeWeak(tmp, tmp|v, order, MemoryOrder::kRelaxed);){
            LDD_UTIL_ATOMIC_X86_PAUSE();
        }
        return tmp;
    }
    Value
    FetchXor(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        Value tmp = Load(MemoryOrder::kRelaxed);
        for (; !CompareExchangeWeak(tmp, tmp^v, order, MemoryOrder::kRelaxed);){
            LDD_UTIL_ATOMIC_X86_PAUSE();
        }
        return tmp;
    }

    bool IsLockFree(void) const volatile {
        return true;
    }

    LDD_UTIL_ATOMIC_DECLARE_INTEGRAL_OPERATORS
private:
    AtomicBase(const AtomicBase&);
    void operator=(const AtomicBase&);
    Value v_;
};

#if defined(__x86_64__)
template<typename T, bool Sign>
class AtomicBase<T, int, 8, Sign>
{
    typedef AtomicBase This;
    typedef T Value;
    typedef T Difference;
public:
    AtomicBase(void) {}
    explicit AtomicBase(Value v) : v_(v) {}

    void
    Store(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        if (order != MemoryOrder::kSequential) {
            FenceBefore(order);
            const_cast<volatile Value&>(v_) = v;
        } else {
            Exchange(v, order);
        }
    }

    Value
    Load(MemoryOrder::Type order = MemoryOrder::kSequential) const volatile {
        Value v = const_cast<const volatile Value&>(v_);
        FenceAfterLoad(order);
        return v;
    }

    Value
    FetchAdd(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        FenceBefore(order);
        __asm__(
                "lock ; xaddq %0, %1"
                : "+r" (v), "+m" (v_)
        );
        FenceAfter(order);
        return v;
    }

    Value
    FetchSub(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        return FetchAdd(-v, order);
    }
    
    Value
    Exchange(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        FenceBefore(order);
        __asm__(
                "xchgq %0, %1"
                : "+r" (v), "+m" (v_)
        );
        FenceAfter(order);
        return v;
    }

    bool
    CompareExchangeStrong(Value* expected, Value desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        Value previous = *expected;
        FenceBefore(success_order);
        __asm__ (
            "lock ; cmpxchgq %2, %1"
            : "+a" (previous), "+m" (v_)
            : "r" (desired)
        );
        bool success = (previous == *expected);
        if (success) {
            FenceAfter(success_order);
        } else {
            FenceAfter(failure_order);
        }
        *expected = previous;
        return success;
    }

    bool
    CompareExchangeWeak(Value* expected, Value desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        return CompareExchangeStrong(expected, desired,
                success_order, failure_order);
    }

    Value
    FetchAnd(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        Value tmp = Load(MemoryOrder::kRelaxed);
        for (; !CompareExchangeWeak(tmp, tmp&v, order, MemoryOrder::kRelaxed);){
            LDD_UTIL_ATOMIC_X86_PAUSE();
        }
        return tmp;
    }

    Value
    FetchOr(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        Value tmp = Load(MemoryOrder::kRelaxed);
        for (; !CompareExchangeWeak(tmp, tmp|v, order, MemoryOrder::kRelaxed);){
            LDD_UTIL_ATOMIC_X86_PAUSE();
        }
        return tmp;
    }

    Value
    FetchXor(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        Value tmp = Load(MemoryOrder::kRelaxed);
        for (; !CompareExchangeWeak(tmp, tmp^v, order, MemoryOrder::kRelaxed);){
            LDD_UTIL_ATOMIC_X86_PAUSE();
        }
        return tmp;
    }

    bool IsLockFree(void) const volatile {
        return true;
    }

    LDD_UTIL_ATOMIC_DECLARE_INTEGRAL_OPERATORS
private:
    AtomicBase(const AtomicBase&);
    void operator=(const AtomicBase&);
    Value v_;
};
#endif

// Pointers

#if !defined(__x86_64__)
template <bool Sign>
class AtomicBase<void*, void*, 4, Sign>
{
    typedef AtomicBase This;
    typedef void* Value;
    typedef ptrdiff_t Difference;
public:
    AtomicBase(void) {}
    explicit AtomicBase(Value v) : v_(v) {}

    void
    Store(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        if (order != MemoryOrder::kSequential) {
            FenceBefore(order);
            const_cast<volatile Value&>(v_) = v;
        } else {
            Exchange(v, order);
        }
    }

    Value
    Load(MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        Value v = const_cast<const volatile Value&>(v_);
        FenceAfterLoad(order);
        return v;
    }

    Value
    Exchange(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        FenceBefore(order);
        __asm__(
            "xchgl %0, %1"
            : "+r" (v), "+m" (v_)
        );
        FenceAfter(order);
        return v;
    }

    bool
    CompareExchangeStrong(Value* expected, Value desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        Value previous = *expected;
        FenceBefore(success_order);
        __asm__(
            "lock ; cmbxchgl %2, %1"
            : "+a" (previous), "+m" (v_)
            : "r" (desire)
        );
        bool success = (previous == *expected);
        if (success) {
            FenceAfter(success_order);
        } else {
            FenceAfter(failure_order);
        }
        *expected = previous;
        return success;
    }
    
    bool
    CompareExchangeWeak(Value* expected, Value desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        return CompareExchangeStrong(expected, desired,
                success_order, failure_order);
    }
    
    bool IsLockFree(void) const volatile {
        return true;
    }

    Value
    FetchAdd(Difference v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        FenceBefore(order);
        __asm__(
            "lock ; xaddl %0, %1"
            : "+r" (v), "+m" (v_)
        );
        FenceAfter(order);
        return reinterpret_cast<Value>(v);
    }

    Value
    FetchSub(Difference v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        return FetchAdd(-v, order);
    }

    LDD_UTIL_ATOMIC_DECLARE_VOID_POINTER_OPERATORS
private:
    AtomicBase(const AtomicBase&);
    void operator=(const AtomicBase&);
    Value v_;
};

template<typename T, bool Sign>
class AtomicBase<T*, void*, 4, Sign>
{
    typedef AtomicBase This;
    typedef T* Value;
    typedef ptrdiff_t Difference;
public:
    AtomicBase(void) {}
    explicit AtomicBase(Value v) : v_(v) {}

    void
    Store(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        if (order != MemoryOrder::kSequential) {
            FenceBefore(order);
            const_cast<volatile Value&>(v_) = v;
        } else {
            Exchange(v, order);
        }
    }

    Value
    Load(MemoryOrder::Type order = MemoryOrder::kSequential) const volatile {
        Value v = const_cast<const volatile Value&>(v_);
        FenceAfterLoad(order);
        return v;
    }

    Value
    Exchange(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        FenceBefore(order);
        __asm__(
            "xchgl %0, %1"
            : "+r" (v), "+m" (v_)
        );
        FenceAfter(order);
        return v;
    }

    bool
    CompareExchangeStrong(Value* expected, Value desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        Value previous = *expected;
        FenceBefore(success_order);
        __asm__(
            "lock ; cmpxchgl %2, %1"
            : "+a" (previous), "+m" (v_)
            : "r" (desired)
        );
        bool success = (previous == *expected);
        if (success) {
            FenceAfter(success_order);
        } else {
            FenceAfter(failure_order);
        }
        *expected = previous;
        return success;
    }

    bool
    CompareExchangeWeak(Value* expected, VAlue desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        return CompareExchangeStrong(expected, desired,
                success_order, failure_order);
    }

    Value
    FetchAdd(Difference v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        v = v * sizeof(*v_);
        FenceBefore(order);
        __asm__ (
            "lock ; xaddl %0, %1"
            : "+r" (v), "+m" (v_)
        );
        FenceAfter(order);
        return reinterpret_cast<Value>(v);
    }

    Value
    FetchSub(Difference v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        return FetchAdd(-v, order);
    }

    bool IsLockFree(void) const volatile {
        return true;
    }

    LDD_UTIL_ATOMIC_DECLARE_POINTER_OPERATORS
private:
    AtomicBase(const AtomicBase&);
    void operator=(const AtomicBase&);
    Value v_;
};

#else

template <bool Sign>
class AtomicBase<void*, void*, 8, Sign>
{
    typedef AtomicBase This;
    typedef void* Value;
    typedef ptrdiff_t Difference;
public:
    AtomicBase(void) {}
    explicit AtomicBase(Value v) : v_(v) {}

    void
    Store(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        if (order != MemoryOrder::kSequential) {
            FenceBefore(order);
            const_cast<volatile Value&>(v_) = v;
        } else {
            Exchange(v, order);
        }
    }

    Value
    Load(MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        Value v = const_cast<const volatile Value&>(v_);
        FenceAfterLoad(order);
        return v;
    }

    Value
    Exchange(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        FenceBefore(order);
        __asm__(
            "xchgq %0, %1"
            : "+r" (v), "+m" (v_)
        );
        FenceAfter(order);
        return v;
    }

    bool
    CompareExchangeStrong(Value* expected, Value desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        Value previous = *expected;
        FenceBefore(success_order);
        __asm__(
            "lock ; cmbxchgq %2, %1"
            : "+a" (previous), "+m" (v_)
            : "r" (desired)
        );
        bool success = (previous == *expected);
        if (success) {
            FenceAfter(success_order);
        } else {
            FenceAfter(failure_order);
        }
        *expected = previous;
        return success;
    }
    
    bool
    CompareExchangeWeak(Value* expected, Value desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        return CompareExchangeStrong(expected, desired,
                success_order, failure_order);
    }
    
    bool IsLockFree(void) const volatile {
        return true;
    }

    Value
    FetchAdd(Difference v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        FenceBefore(order);
        __asm__(
            "lock ; xaddq %0, %1"
            : "+r" (v), "+m" (v_)
        );
        FenceAfter(order);
        return reinterpret_cast<Value>(v);
    }

    Value
    FetchSub(Difference v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        return FetchAdd(-v, order);
    }

    LDD_UTIL_ATOMIC_DECLARE_VOID_POINTER_OPERATORS
private:
    AtomicBase(const AtomicBase&);
    void operator=(const AtomicBase&);
    Value v_;
};

template<typename T, bool Sign>
class AtomicBase<T*, void*, 8, Sign>
{
    typedef AtomicBase This;
    typedef T* Value;
    typedef ptrdiff_t Difference;
public:
    AtomicBase(void) {}
    explicit AtomicBase(Value v) : v_(v) {}

    void
    Store(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        if (order != MemoryOrder::kSequential) {
            FenceBefore(order);
            const_cast<volatile Value&>(v_) = v;
        } else {
            Exchange(v, order);
        }
    }

    Value
    Load(MemoryOrder::Type order = MemoryOrder::kSequential) const volatile {
        Value v = const_cast<const volatile Value&>(v_);
        FenceAfterLoad(order);
        return v;
    }

    Value
    Exchange(Value v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        FenceBefore(order);
        __asm__(
            "xchgq %0, %1"
            : "+r" (v), "+m" (v_)
        );
        FenceAfter(order);
        return v;
    }

    bool
    CompareExchangeStrong(Value* expected, Value desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        Value previous = *expected;
        FenceBefore(success_order);
        __asm__(
            "lock ; cmpxchgq %2, %1"
            : "+a" (previous), "+m" (v_)
            : "r" (desired)
        );
        bool success = (previous == *expected);
        if (success) {
            FenceAfter(success_order);
        } else {
            FenceAfter(failure_order);
        }
        *expected = previous;
        return success;
    }

    bool
    CompareExchangeWeak(Value* expected, Value desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        return CompareExchangeStrong(expected, desired,
                success_order, failure_order);
    }

    Value
    FetchAdd(Difference v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        v = v * sizeof(*v_);
        FenceBefore(order);
        __asm__ (
            "lock ; xaddq %0, %1"
            : "+r" (v), "+m" (v_)
        );
        FenceAfter(order);
        return reinterpret_cast<Value>(v);
    }

    Value
    FetchSub(Difference v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        return FetchAdd(-v, order);
    }

    bool IsLockFree(void) const volatile {
        return true;
    }

    LDD_UTIL_ATOMIC_DECLARE_POINTER_OPERATORS
private:
    AtomicBase(const AtomicBase&);
    void operator=(const AtomicBase&);
    Value v_;
};

#endif

template<typename T, bool Sign>
class AtomicBase<T, void, 1, Sign>
{
    typedef AtomicBase This;
    typedef T Value;
    typedef uint8_t Storage;
public:
    AtomicBase(void) {}
    explicit AtomicBase(Value const& v)
        : v_(reinterpret_cast<Storage const&>(v)) {}

    void
    Store(Value const& v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        if (order != MemoryOrder::kSequential) {
            Storage tmp;
            memcpy(&tmp, &v, sizeof(Value));
            FenceBefore(order);
            const_cast<volatile Storage&>(v_) = tmp;
        } else {
            Exchange(v, order);
        }
    }

    Value
    Load(MemoryOrder::Type order = MemoryOrder::kSequential) const volatile {
        Storage tmp = const_cast<volatile Storage&>(v_);
        FenceAfterLoad(order);
        Value v;
        memcpy(&v, &tmp, sizeof(Value));
        return v;
    }

    Value
    Exchange(Value const& v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        Storage tmp;
        memcpy(&tmp, &v, sizeof(Value));
        FenceBefore(order);
        __asm__(
            "xchgb %0, %1"
            : "+q" (tmp), "+m" (v_)
        );
        FenceAfter(order);
        Value res;
        memcpy(&res, &tmp, sizeof(Value));
        return res;
    }

    bool
    CompareExchangeStrong(Value* expected, Value const& desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        Storage expected_s, desired_s;
        memcpy(&expected_s, expected, sizeof(Value));
        memcpy(&desired_s, &desired, sizeof(Value));
        Storage previous_s = expected_s;
        FenceBefore(success_order);
        __asm__(
            "lock ; cmpxchgb %2, %1"
            : "+a" (previous_s), "+m" (v_)
            : "q" (desired_s)
        );
        bool success = (previous_s == expected_s);
        if (success) {
            FenceAfter(success_order);
        } else {
            FenceAfter(failure_order);
        }
        memcpy(expected, &previous_s, sizeof(Value));
        return success;
    }

    bool
    CompareExchangeWeak(Value* expected, Value const& desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        return CompareExchangeStrong(expected, desired,
                success_order, failure_order);
    }

    bool IsLockFree(void) const volatile {
        return true;
    }

    LDD_UTIL_ATOMIC_DECLARE_BASE_OPERATORS
private:
    AtomicBase(const AtomicBase&);
    void operator=(const AtomicBase&);
    Storage v_;
};

template<typename T, bool Sign>
class AtomicBase<T, void, 2, Sign>
{
    typedef AtomicBase This;
    typedef T Value;
    typedef uint16_t Storage;
public:
    AtomicBase(void) {}
    explicit AtomicBase(Value const& v)
        : v_(reinterpret_cast<Storage const&>(v)) {}

    void
    Store(Value const& v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        if (order != MemoryOrder::kSequential) {
            Storage tmp;
            memcpy(&tmp, &v, sizeof(Value));
            FenceBefore(order);
            const_cast<volatile Storage&>(v_) = tmp;
        } else {
            Exchange(v, order);
        }
    }

    Value
    Load(MemoryOrder::Type order = MemoryOrder::kSequential) const volatile {
        Storage tmp = const_cast<volatile Storage&>(v_);
        FenceAfterLoad(order);
        Value v;
        memcpy(&v, &tmp, sizeof(Value));
        return v;
    }

    Value
    Exchange(Value const& v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        Storage tmp;
        memcpy(&tmp, &v, sizeof(Value));
        FenceBefore(order);
        __asm__(
            "xchgw %0, %1"
            : "+q" (tmp), "+m" (v_)
        );
        FenceAfter(order);
        Value res;
        memcpy(&res, &tmp, sizeof(Value));
        return res;
    }

    bool
    CompareExchangeStrong(Value* expected, Value const& desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        Storage expected_s, desired_s;
        memcpy(&expected_s, expected, sizeof(Value));
        memcpy(&desired_s, &desired, sizeof(Value));
        Storage previous_s = expected_s;
        FenceBefore(success_order);
        __asm__(
            "lock ; cmpxchgw %2, %1"
            : "+a" (previous_s), "+m" (v_)
            : "q" (desired_s)
        );
        bool success = (previous_s == expected_s);
        if (success) {
            FenceAfter(success_order);
        } else {
            FenceAfter(failure_order);
        }
        memcpy(expected, &previous_s, sizeof(Value));
        return success;
    }

    bool
    CompareExchangeWeak(Value* expected, Value const& desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        return CompareExchangeStrong(expected, desired,
                success_order, failure_order);
    }

    bool IsLockFree(void) const volatile {
        return true;
    }

    LDD_UTIL_ATOMIC_DECLARE_BASE_OPERATORS
private:
    AtomicBase(const AtomicBase&);
    void operator=(const AtomicBase&);
    Storage v_;
};

template<typename T, bool Sign>
class AtomicBase<T, void, 4, Sign>
{
    typedef AtomicBase This;
    typedef T Value;
    typedef uint32_t Storage;
public:
    AtomicBase(void) {}
    explicit AtomicBase(Value const& v) : v_(0) {
        memcpy(&v_, &v, sizeof(Value));
    }

    void
    Store(Value const& v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        if (order != MemoryOrder::kSequential) {
            Storage tmp;
            memcpy(&tmp, &v, sizeof(Value));
            FenceBefore(order);
            const_cast<volatile Storage&>(v_) = tmp;
        } else {
            Exchange(v, order);
        }
    }

    Value
    Load(MemoryOrder::Type order = MemoryOrder::kSequential) const volatile {
        Storage tmp = const_cast<volatile Storage&>(v_);
        FenceAfterLoad(order);
        Value v;
        memcpy(&v, &tmp, sizeof(Value));
        return v;
    }

    Value
    Exchange(Value const& v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        Storage tmp;
        memcpy(&tmp, &v, sizeof(Value));
        FenceBefore(order);
        __asm__(
            "xchgl %0, %1"
            : "+q" (tmp), "+m" (v_)
        );
        FenceAfter(order);
        Value res;
        memcpy(&res, &tmp, sizeof(Value));
        return res;
    }

    bool
    CompareExchangeStrong(Value* expected, Value const& desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        Storage expected_s, desired_s;
        memcpy(&expected_s, expected, sizeof(Value));
        memcpy(&desired_s, &desired, sizeof(Value));
        Storage previous_s = expected_s;
        FenceBefore(success_order);
        __asm__(
            "lock ; cmpxchgl %2, %1"
            : "+a" (previous_s), "+m" (v_)
            : "q" (desired_s)
        );
        bool success = (previous_s == expected_s);
        if (success) {
            FenceAfter(success_order);
        } else {
            FenceAfter(failure_order);
        }
        memcpy(expected, &previous_s, sizeof(Value));
        return success;
    }

    bool
    CompareExchangeWeak(Value* expected, Value const& desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        return CompareExchangeStrong(expected, desired,
                success_order, failure_order);
    }

    bool IsLockFree(void) const volatile {
        return true;
    }

    LDD_UTIL_ATOMIC_DECLARE_BASE_OPERATORS
private:
    AtomicBase(const AtomicBase&);
    void operator=(const AtomicBase&);
    Storage v_;
};

#if defined(__x86_64__)
template<typename T, bool Sign>
class AtomicBase<T, void, 8, Sign>
{
    typedef AtomicBase This;
    typedef T Value;
    typedef uint64_t Storage;
public:
    AtomicBase(void) {}
    explicit AtomicBase(Value const& v) : v_(0) {
        memcpy(&v_, &v, sizeof(Value));
    }

    void
    Store(Value const& v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        if (order != MemoryOrder::kSequential) {
            Storage tmp;
            memcpy(&tmp, &v, sizeof(Value));
            FenceBefore(order);
            const_cast<volatile Storage&>(v_) = tmp;
        } else {
            Exchange(v, order);
        }
    }

    Value
    Load(MemoryOrder::Type order = MemoryOrder::kSequential) const volatile {
        Storage tmp = const_cast<volatile Storage&>(v_);
        FenceAfterLoad(order);
        Value v;
        memcpy(&v, &tmp, sizeof(Value));
        return v;
    }

    Value
    Exchange(Value const& v,
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        Storage tmp;
        memcpy(&tmp, &v, sizeof(Value));
        FenceBefore(order);
        __asm__(
            "xchgq %0, %1"
            : "+q" (tmp), "+m" (v_)
        );
        FenceAfter(order);
        Value res;
        memcpy(&res, &tmp, sizeof(Value));
        return res;
    }

    bool
    CompareExchangeStrong(Value* expected, Value const& desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        Storage expected_s, desired_s;
        memcpy(&expected_s, expected, sizeof(Value));
        memcpy(&desired_s, &desired, sizeof(Value));
        Storage previous_s = expected_s;
        FenceBefore(success_order);
        __asm__(
            "lock ; cmpxchgq %2, %1"
            : "+a" (previous_s), "+m" (v_)
            : "q" (desired_s)
        );
        bool success = (previous_s == expected_s);
        if (success) {
            FenceAfter(success_order);
        } else {
            FenceAfter(failure_order);
        }
        memcpy(expected, &previous_s, sizeof(Value));
        return success;
    }

    bool
    CompareExchangeWeak(Value* expected, Value const& desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        return CompareExchangeStrong(expected, desired,
                success_order, failure_order);
    }

    bool IsLockFree(void) const volatile {
        return true;
    }

    LDD_UTIL_ATOMIC_DECLARE_BASE_OPERATORS
private:
    AtomicBase(const AtomicBase&);
    void operator=(const AtomicBase&);
    Storage v_;
};
#endif

} // namespace detail
} // namespace util
} // namespace ldd

#endif // LDD_UTIL_DETAIL_ATOMIC_GCC_X86_H_
