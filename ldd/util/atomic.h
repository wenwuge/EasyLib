// atomic.h (2013-07-10)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_UTIL_ATOMIC_H_
#define LDD_UTIL_ATOMIC_H_

#include <cstddef>
#include <boost/cstdint.hpp>
#include <boost/type_traits/is_signed.hpp>
#include <boost/type_traits/is_integral.hpp>
#include "ldd/util/memory_order.h"
#include "ldd/util/detail/atomic/platform.h"
#include "ldd/util/detail/atomic/classification.h"

namespace ldd {
namespace util {

template <typename T>
class Atomic :
    public detail::AtomicBase<
        T,
        typename detail::Classify<T>::Type, 
        detail::StorageSizeOf<T>::value,
        boost::is_signed<T>::value
    >
{
private:
    typedef T Value;
    typedef detail::AtomicBase<T,
            typename detail::Classify<T>::Type,
            detail::StorageSizeOf<T>::value,
            boost::is_signed<T>::value> Super;
public:
    Atomic(void) : Super() {}
    Atomic(Value v) : Super(v) {}

    Value operator=(Value v) volatile {
        this->Store(v);
        return v;
    }
    operator Value(void) volatile const {
        return this->Load();
    }
private:
    Atomic(const Atomic&);
    Atomic& operator=(const Atomic&) volatile;
};

#ifndef LDD_UTIL_ATOMIC_FLAG_LOCK_FREE
#define LDD_UTIL_ATOMIC_FLAG_LOCK_FREE 0
class AtomicFlag {
public:
    AtomicFlag(void) : v_(false) {}

    bool
    TestAndSet(MemoryOrder::Type order = MemoryOrder::kSequential) {
        return v_.Exchange(true, order);
    }

    void
    Clear(MemoryOrder::Type order = MemoryOrder::kSequential) volatile {
        v_.Store(false, order);
    }
private:
    AtomicFlag(const AtomicFlag&);
    AtomicFlag& operator=(const AtomicFlag&);
    Atomic<bool> v_;
};
#endif

} // namespace util
} // namespace ldd

#endif // LDD_UTIL_ATOMIC_H_
