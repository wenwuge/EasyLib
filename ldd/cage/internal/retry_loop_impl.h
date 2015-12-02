// retry_loop_impl.h (2013-09-12)
// Li Xinjie (xjason.li@gmail.com)

#include "retry_loop.h"
#include <ldd/net/event.h>
#include "ldd/cage/retry_policy.h"
#include "ldd/cage/cage.h"
#include "operation.h"
#include "failure_result.h"
#include "cage_impl.h"

namespace ldd {
namespace cage {

template <class R>
class RetryLoop::Impl : public RetryLoop {
public:
    Impl(Cage::Impl* cage, RetryPolicy* policy, Operation<R>* operation,
            const boost::function<void(const R&)>& callback)
        : cage_(cage),
          policy_(policy),
          operation_(operation),
          callback_(callback),
          timer_(cage->event_loop()),
          retry_count_(0),
          waiting_(false) {}
    ~Impl() {
        delete operation_;
    }
    void Start() {
        start_ = ldd::util::Timestamp::Now();
        Try();
    }
    void Stop() {
        if (waiting_) {
            timer_.Cancel();
        }
        Callback(FailureResult<R>(Status::kClosing));
    }
private:
    void Try() {
        operation_->Do(&cage_->keeper_,
                boost::bind(&Impl::HandleResult, this, _1));
    }
    void HandleResult(const R& result) {
        if (result.status().IsConnectionLoss() ||
                result.status().IsOperationTimeout() ||
                result.status().IsSessionMoved()) {
            ldd::util::TimeDiff wait;
            if (policy_->ShouldRetry(retry_count_,
                        ldd::util::Timestamp::Now() - start_, &wait)) {
                retry_count_++;
                timer_.AsyncWait(boost::bind(&Impl::Retry, this), wait);
                waiting_ = true;
                return;
            }
        }
        Callback(result);
    }
    void Retry() {
        waiting_ = false;
        Try();
    }
    void Callback(const R& r) {
        CHECK(is_linked());
        unlink();
        callback_(r);
        delete this;
    }
    
private:
    Cage::Impl* cage_;
    RetryPolicy* policy_;
    Operation<R>* operation_;
    boost::function<void(const R&)> callback_;
    ldd::net::TimerEvent timer_;
    int retry_count_;
    ldd::util::Timestamp start_;
    bool waiting_;
};

} // namespace cage
} // namespace ldd
