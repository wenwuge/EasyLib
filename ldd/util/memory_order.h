// memory_order.h (2013-07-10)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_UTIL_MEMORY_ORDER_H_
#define LDD_UTIL_MEMORY_ORDER_H_

namespace ldd {
namespace util {

struct MemoryOrder {
    enum Type {
        kRelaxed = 0,
        kAcquire = 1,
        kRelease = 2,
        kAcquireRelease = 3,    // kAcquire | kRelease
        kSequential = 7,        // kAcquireRelease | 4
        kConsume = 8,
    };
};

} // namespace util
} // namespace ldd

#endif // LDD_UTIL_MEMORY_ORDER_H_
