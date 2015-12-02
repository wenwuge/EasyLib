// retry_policy.cc (2013-09-11)
// Li Xinjie (xjason.li@gmail.com)

#include "retry_policy.h"
#include "internal/retry_policy.h"

namespace ldd {
namespace cage {

RetryPolicy* RetryPolicy::NewRetryNTimesPolicy(int n,
        const ldd::util::TimeDiff& wait) {
    return new RetryNTimes(n, wait);
}

} // namespace cage
} // namespace ldd
