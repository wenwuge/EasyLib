// retry_policy.h (2013-09-07)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_CAGE_RETRY_POLICY_H_
#define LDD_CAGE_RETRY_POLICY_H_

#include <ldd/util/time_diff.h>

namespace ldd {
namespace cage {

class RetryPolicy {
public:
    virtual ~RetryPolicy() {}
    virtual bool ShouldRetry(int retry_count,
            const ldd::util::TimeDiff& elapsed, ldd::util::TimeDiff* wait) = 0;

    static RetryPolicy* NewRetryNTimesPolicy(
            int n, const ldd::util::TimeDiff& wait);
};

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_RETRY_POLICY_H_
