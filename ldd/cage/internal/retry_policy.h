// retry_policy.h (2013-09-11)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_CAGE_INTERNAL_RETRY_POLICY_H_
#define LDD_CAGE_INTERNAL_RETRY_POLICY_H_

#include "ldd/cage/retry_policy.h"

namespace ldd {
namespace cage {

class NoRetry : public RetryPolicy {
public:
    bool ShouldRetry(int retry_count, const ldd::util::TimeDiff& elapsed,
            ldd::util::TimeDiff* wait) {
        return false;
    }
};

class WaitingRetry : public RetryPolicy {
public:
    WaitingRetry(int max) : max_(max >= 0 ? max : 0) {}
    bool ShouldRetry(int retry_count, const ldd::util::TimeDiff& elapsed,
            ldd::util::TimeDiff* wait) {
        if (retry_count > max_ || retry_count < 0) {
            return false;
        }
        WaitFor(retry_count, elapsed, wait);
        return true;
    }
protected:
    virtual void WaitFor(int retry_count,
            const ldd::util::TimeDiff& elapsed,
            ldd::util::TimeDiff* wait) = 0;
private:
    int max_;
};

class RetryNTimes : public WaitingRetry {
public:
    RetryNTimes(int n, const ldd::util::TimeDiff& wait)
        : WaitingRetry(n), wait_(wait) {}
private:
    void WaitFor(int retry_count, const ldd::util::TimeDiff& elapsed,
            ldd::util::TimeDiff* wait) {
        *wait = wait_;
    }
    ldd::util::TimeDiff wait_;
};


} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_INTERNAL_RETRY_POLICY_H_
