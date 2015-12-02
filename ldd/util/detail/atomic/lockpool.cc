// lockpool.cc (2013-07-15)
// Li Xinjie (xjason.li@gmail.com)

#include "ldd/util/atomic.h"

namespace ldd {
namespace util {
namespace detail {

static LockPool::Lock g_lock_pool[41];

LockPool::Lock* LockPool::GetLockFor(const volatile void* addr) {
    std::size_t index = reinterpret_cast<std::size_t>(addr) %
        (sizeof(g_lock_pool) / sizeof(*g_lock_pool));
    return &g_lock_pool[index];
}

} // namespace detail
} // namespace util
} // namespace ldd
