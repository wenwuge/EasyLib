// base.h (2013-07-12)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_UTIL_DETAIL_ATOMIC_BASE_H_
#define LDD_UTIL_DETAIL_ATOMIC_BASE_H_

#include <string.h>
#include <stdint.h>
#include <cstddef>
#include "ldd/util/detail/atomic/lockpool.h"

#define LDD_UTIL_ATOMIC_DECLARE_BASE_OPERATORS \
    bool \
    CompareExchangeStrong(Value* expected, Value desired, \
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile { \
        return CompareExchangeStrong(expected, desired, \
                order, CalculateFailureOrder(order)); \
    } \
    bool \
    CompareExchangeWeak(Value* expected, Value desired, \
            MemoryOrder::Type order = MemoryOrder::kSequential) volatile { \
        return CompareExchangeWeak(expected, desired, \
                order, CalculateFailureOrder(order)); \
    } \

#define LDD_UTIL_ATOMIC_DECLARE_ADDITIVE_OPERATORS \
    Value operator++(int) volatile { \
        return FetchAdd(1); \
    } \
    Value operator++(void) volatile { \
        return FetchAdd(1) + 1; \
    } \
    Value operator--(int) volatile { \
        return FetchSub(1); \
    } \
    Value operator--(void) volatile { \
        return FetchSub(1) -1; \
    } \
    Value operator+=(Difference v) volatile { \
        return FetchAdd(v) + v; \
    } \
    Value operator-=(Difference v) volatile { \
        return FetchSub(v) - v; \
    } \

#define LDD_UTIL_ATOMIC_DECLARE_VOID_POINTER_ADDITIVE_OPERATORS \
    Value operator++(int) volatile { \
        return FetchAdd(1); \
    } \
    Value operator++(void) volatile { \
        return (char*)FetchAdd(1) + 1; \
    } \
    Value operator--(int) volatile { \
        return FetchSub(1); \
    } \
    Value operator--(void) volatile { \
        return (char*)FetchSub(1) - 1; \
    } \
    Value operator+=(Difference v) volatile { \
        return (char*)FetchAdd(v) + v; \
    } \
    Value operator-=(Difference v) volatile { \
        return (char*)FetchSub(v) - v; \
    } \

#define LDD_UTIL_ATOMIC_DECLARE_BIT_OPERATORS \
    Value operator&=(Difference v) volatile { \
        return FetchAnd(v) & v; \
    } \
    Value operator|=(Difference v) volatile { \
        return FetchOr(v) | v; \
    } \
    Value operator^=(Difference v) volatile { \
        return FetchXor(v) ^ v; \
    } \

#define LDD_UTIL_ATOMIC_DECLARE_POINTER_OPERATORS \
    LDD_UTIL_ATOMIC_DECLARE_BASE_OPERATORS \
    LDD_UTIL_ATOMIC_DECLARE_ADDITIVE_OPERATORS \

#define LDD_UTIL_ATOMIC_DECLARE_VOID_POINTER_OPERATORS \
    LDD_UTIL_ATOMIC_DECLARE_BASE_OPERATORS \
    LDD_UTIL_ATOMIC_DECLARE_VOID_POINTER_ADDITIVE_OPERATORS \

#define LDD_UTIL_ATOMIC_DECLARE_INTEGRAL_OPERATORS \
    LDD_UTIL_ATOMIC_DECLARE_BASE_OPERATORS \
    LDD_UTIL_ATOMIC_DECLARE_ADDITIVE_OPERATORS \
    LDD_UTIL_ATOMIC_DECLARE_BIT_OPERATORS \

namespace ldd {
namespace util {
namespace detail {

inline MemoryOrder::Type
CalculateFailureOrder(MemoryOrder::Type order) {
    switch (order) {
    case MemoryOrder::kAcquireRelease:
        return MemoryOrder::kAcquire;
    case MemoryOrder::kRelease:
        return MemoryOrder::kRelaxed;
    default:
        return order;
    }
}

template <typename T, typename C, unsigned int Size, bool Sign>
class AtomicBase {
private:
    typedef AtomicBase This;
    typedef T Value;
    typedef T Difference;
    typedef LockPool::ScopedLock Guard;
    typedef char Storage[sizeof(Value)]; 

public:
    AtomicBase(void) {}
    explicit AtomicBase(Value const& v): v_(v) {}

    void
    Store(Value const& v,
            MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) volatile
    {
        char* storage = storage_ptr();
        Guard guard(storage);
        memcpy(storage, &v, sizeof(Value));
    }

    Value
    Load(MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) volatile const{
        char* storage = storage_ptr();
        Guard guard(storage);
        Value v;
        memcpy(&v, storage, sizeof(Value));
        return v;
    }
    
    bool
    CompareExchangeStrong(Value* expected, Value const& desired,
            MemoryOrder::Type /*success_order*/,
            MemoryOrder::Type /*failure_order*/) volatile {
        char* storage = storage_ptr();
        Guard guard(storage);
        if (memcmp(storage, expected, sizeof(Value)) == 0) {
            memcpy(storage, &desired, sizeof(Value));
            return true;
        } else {
            memcpy(expected, storage, sizeof(Value));
            return false;
        }
    }
    
    bool
    CompareExchangeWeak(Value* expected, Value const& desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        return CompareExchangeStrong(expected, desired,
                success_order, failure_order);
    }

    Value
    Exchange(Value const& v,
            MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) volatile {
        char* storage = storage_ptr();
        Guard guard(storage);
        Value tmp;
        memcpy(&tmp, storage, sizeof(Value));
        memcpy(storage, &v, sizeof(Value));
        return tmp;
    }

    bool IsLockFree(void) const volatile {
        return false;
    }
    
    LDD_UTIL_ATOMIC_DECLARE_BASE_OPERATORS
private:
    char* storage_ptr() volatile const {
        return const_cast<char*>(&reinterpret_cast<char const volatile&>(v_));
    }

    AtomicBase(const AtomicBase&);
    void operator=(const AtomicBase&);
    Value v_;
};

template<typename T, unsigned int Size, bool Sign>
class AtomicBase<T, int, Size, Sign>
{
private:
    typedef AtomicBase This;
    typedef T Value;
    typedef T Difference;
    typedef LockPool::ScopedLock Guard;
public:
    AtomicBase(void) {}
    explicit AtomicBase(Value v) : v_(v) {}

    void
    Store(Value v,
            MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) volatile {
        Guard guard(const_cast<Value*>(&v_));
        v_ = v;
    }

    Value
    Load(MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) const volatile{
        Guard guard(const_cast<Value*>(&v_));
        Value v = const_cast<const volatile Value&>(v_);
        return v;
    }

    Value
    Exchange(Value v,
            MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) volatile {
        Guard guard(const_cast<Value*>(&v_));
        Value old = v_;
        v_ = v;
        return old;
    }

    bool
    CompareExchangeStrong(Value* expected, Value desired,
            MemoryOrder::Type /*success_order*/,
            MemoryOrder::Type /*failure_order*/) volatile {
        Guard guard(const_cast<Value*>(&v_));
        if (v_ == *expected) {
            v_ = desired;
            return true;
        } else {
            *expected = v_;
            return false;
        }
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
            MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) volatile {
        Guard guard(const_cast<Value*>(&v_));
        Value old = v_;
        v_ += v;
        return old;
    }

    Value
    FetchSub(Difference v,
            MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) volatile {
        Guard guard(const_cast<Value*>(&v_));
        Value old = v_;
        v_ -= v;
        return old;
    }
    
    Value
    FetchAnd(Value v,
            MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) volatile {
        Guard guard(const_cast<Value*>(&v_));
        Value old = v_;
        v_ &= v;
        return old;
    }

    Value
    FetchOr(Value v,
            MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) volatile {
        Guard guard(const_cast<Value*>(&v_));
        Value old = v_;
        v_ |= v;
        return old;
    }

    Value
    FetchXor(Value v,
            MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) volatile {
        Guard guard(const_cast<Value*>(&v_));
        Value old = v_;
        v_ ^= v;
        return old;
    }

    bool IsLockFree(void) const volatile {
        return false;
    }

    LDD_UTIL_ATOMIC_DECLARE_INTEGRAL_OPERATORS
private:
    AtomicBase(const AtomicBase&);
    void operator=(const AtomicBase&);
    Value v_;
};

template<typename T, unsigned int Size, bool Sign>
class AtomicBase<T*, void*, Size, Sign>
{
private:
    typedef AtomicBase This;
    typedef T* Value;
    typedef ptrdiff_t Difference;
    typedef LockPool::ScopedLock Guard;
public:
    AtomicBase(void) {}
    explicit AtomicBase(Value v): v_(v) {}

    void
    Store(Value v,
            MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) volatile {
        Guard guard(const_cast<Value*>(&v_));
        v_ = v;
    }
    
    Value
    Load(MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) const volatile{
        Guard guard(const_cast<Value*>(&v_));
        Value v = const_cast<const volatile Value&>(v_);
        return v;
    }
    
    Value
    Exchange(Value v,
            MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) volatile {
        Guard guard(const_cast<Value*>(&v_));
        Value old = v_;
        v_ = v;
        return old;
    }

    bool
    CompareExchangeStrong(Value* expected, Value desired,
            MemoryOrder::Type /*success_order*/,
            MemoryOrder::Type /*failure_order*/) volatile {
        Guard guard(const_cast<Value*>(&v_));
        if (v_ == *expected) {
            v_ = desired;
            return true;
        } else {
            *expected = v_;
            return false;
        }
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
            MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) volatile {
        Guard guard(const_cast<Value*>(&v_));
        Value old = v_;
        v_ += v;
        return old;
    }

    Value
    FetchSub(Difference v,
            MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) volatile {
        Guard guard(const_cast<Value*>(&v_));
        Value old = v_;
        v_ -= v;
        return old;
    }

    bool IsLockFree(void) const volatile {
        return false;
    }

    LDD_UTIL_ATOMIC_DECLARE_POINTER_OPERATORS
private:
    AtomicBase(const AtomicBase&);
    void operator=(const AtomicBase&);
    Value v_;
};

template<unsigned int Size, bool Sign>
class AtomicBase<void*, void*, Size, Sign>
{
private:
    typedef AtomicBase This;
    typedef ptrdiff_t Difference;
    typedef void* Value;
    typedef LockPool::ScopedLock Guard;
public:
    AtomicBase(void) {}
    explicit AtomicBase(Value v) : v_(v) {}

    void
    Store(Value v,
            MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) volatile {
        Guard guard(const_cast<Value*>(&v_));
        v_ = v;
    }
    
    Value
    Load(MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) const volatile{
        Guard guard(const_cast<Value*>(&v_));
        Value v = const_cast<const volatile Value&>(v_);
        return v;
    }
    
    Value
    Exchange(Value v,
            MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) volatile {
        Guard guard(const_cast<Value*>(&v_));
        Value old = v_;
        v_ = v;
        return old;
    }

    bool
    CompareExchangeStrong(Value* expected, Value desired,
            MemoryOrder::Type /*success_order*/,
            MemoryOrder::Type /*failure_order*/) volatile {
        Guard guard(const_cast<Value*>(&v_));
        if (v_ = *expected) {
            v_ = desired;
            return true;
        } else {
            *expected = v_;
            return false;
        }
    }
    
    bool
    CompareExchangeWeak(Value* expected, Value desired,
            MemoryOrder::Type success_order,
            MemoryOrder::Type failure_order) volatile {
        return CompareExchangeStrong(expected, desired,
                success_order, failure_order);
    }

    bool IsLockFree(void) const volatile {
        return false;
    }

    Value FetchAdd(Difference v,
            MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) volatile {
        Guard guard(const_cast<Value*>(&v_));
        Value old = v_;
        char* cv = reinterpret_cast<char*>(old);
        cv += v;
        v_ = cv;
        return old;
    }

    Value FetchSub(Difference v,
            MemoryOrder::Type /*order*/ = MemoryOrder::kSequential) volatile {
        Guard guard(const_cast<Value*>(&v_));
        Value old = v_;
        char* cv = reinterpret_cast<char*>(old);
        cv -= v;
        v_ = cv;
        return old;
    }

    LDD_UTIL_ATOMIC_DECLARE_VOID_POINTER_OPERATORS
private:
    AtomicBase(const AtomicBase&);
    void operator=(const AtomicBase&);
    Value v_;
};

} // namespace detail
} // namespace util
} // namespace ldd

#endif // LDD_UTIL_DETAIL_ATOMIC_BASE_H_
