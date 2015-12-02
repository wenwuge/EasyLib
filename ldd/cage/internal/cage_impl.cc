// cage_impl.cc (2013-09-12)
// Li Xinjie (xjason.li@gmail.com)

#include <boost/bind.hpp>
#include <glog/logging.h>
#include "cage_impl.h"
#include "operation.h"
#include "retry_policy.h"
#include "retry_loop_impl.h"

namespace ldd {
namespace cage {

Cage::Impl::Impl(net::EventLoop* event_loop, const CageListener& listener,
        RetryPolicy* policy)
    : event_loop_(event_loop),
      keeper_(event_loop, boost::bind(&Impl::ListenKeeper, this, _1)),
      timer_(event_loop),
      listener_(listener),
      policy_(policy == NULL ? new NoRetry() : policy)
{
}

Cage::Impl::~Impl() {
    Close();
    delete policy_;
}

bool Cage::Impl::Open(const std::string& dest, int timeout) {
    return keeper_.Open(dest, timeout);
}

void Cage::Impl::Close() {
    if (keeper_.IsOpen()) {
        RetryList::iterator it = retries_.begin();
        for (; it != retries_.end();) {
            (it++)->Stop();
        }
        keeper_.Close();
        CHECK_EQ(retries_.size(), 0UL);
    }
}

void Cage::Impl::AddAuth(const std::string& scheme, const std::string& cert,
        const AddAuthCallback& callback) {
    Operation<AddAuthResult>* op = new Operation<AddAuthResult>(scheme, cert);
    RetryLoop* retry =
        new RetryLoop::Impl<AddAuthResult>(this, policy_, op, callback);
    retries_.push_back(*retry);
    retry->Start();
}

void Cage::Impl::Create(const std::string& path, const std::string& data,
        const std::vector<Acl>& acls, Mode::Type mode,
        const CreateCallback& callback) {
    Operation<CreateResult>* op =
        new Operation<CreateResult>(path, data, acls, mode);
    RetryLoop* retry =
        new RetryLoop::Impl<CreateResult>(this, policy_, op, callback);
    retries_.push_back(*retry);
    retry->Start();
}

void Cage::Impl::Delete(const std::string& path, int32_t version,
        const DeleteCallback& callback) {
    Operation<DeleteResult>* op =
        new Operation<DeleteResult>(path, version);
    RetryLoop* retry =
        new RetryLoop::Impl<DeleteResult>(this, policy_, op, callback);
    retries_.push_back(*retry);
    retry->Start();
}

void Cage::Impl::Exists(const std::string& path, const NodeWatcher& watcher,
        const ExistsCallback& callback) {
    Operation<ExistsResult>* op = 
        new Operation<ExistsResult>(path, watcher);
    RetryLoop* retry = 
        new RetryLoop::Impl<ExistsResult>(this, policy_, op, callback);
    retries_.push_back(*retry);
    retry->Start();
}

void Cage::Impl::Get(const std::string& path, const NodeWatcher& watcher,
        const GetCallback& callback) {
    Operation<GetResult>* op =
        new Operation<GetResult>(path, watcher);
    RetryLoop* retry =
        new RetryLoop::Impl<GetResult>(this, policy_, op, callback);
    retries_.push_back(*retry);
    retry->Start();
}

void Cage::Impl::Set(const std::string& path, const std::string& data,
        int32_t version, const SetCallback& callback) {
    Operation<SetResult>* op =
        new Operation<SetResult>(path, data, version);
    RetryLoop* retry =
        new RetryLoop::Impl<SetResult>(this, policy_, op, callback);
    retries_.push_back(*retry);
    retry->Start();
}

void Cage::Impl::GetAcl(const std::string& path, const GetAclCallback& callback) {
    Operation<GetAclResult>* op =
        new Operation<GetAclResult>(path);
    RetryLoop* retry =
        new RetryLoop::Impl<GetAclResult>(this, policy_, op, callback);
    retries_.push_back(*retry);
    retry->Start();
}

void Cage::Impl::SetAcl(const std::string& path, const std::vector<Acl>& acls,
        int32_t version, const SetAclCallback& callback) {
    Operation<SetAclResult>* op =
        new Operation<SetAclResult>(path, acls, version);
    RetryLoop* retry =
        new RetryLoop::Impl<SetAclResult>(this, policy_, op, callback);
    retries_.push_back(*retry);
    retry->Start();
}

void Cage::Impl::GetChildren(const std::string& path, const ChildWatcher& watcher, 
        const GetChildrenCallback& callback) {
    Operation<GetChildrenResult>* op =
        new Operation<GetChildrenResult>(path, watcher);
    RetryLoop* retry =
        new RetryLoop::Impl<GetChildrenResult>(this, policy_, op, callback);
    retries_.push_back(*retry);
    retry->Start();
}

void Cage::Impl::GetChildrenWithStat(const std::string& path,
        const ChildWatcher& watcher, 
        const GetChildrenWithStatCallback& callback) {
    Operation<GetChildrenWithStatResult>* op =
        new Operation<GetChildrenWithStatResult>(path, watcher);
    RetryLoop* retry =
        new RetryLoop::Impl<GetChildrenWithStatResult>(
                this, policy_, op, callback);
    retries_.push_back(*retry);
    retry->Start();
}

void Cage::Impl::Multi(const std::vector<Op*>& ops, const MultiCallback& callback) {
    Operation<MultiResult>* op =
        new Operation<MultiResult>(ops);
    RetryLoop* retry = 
        new RetryLoop::Impl<MultiResult>(this, policy_, op, callback);
    retries_.push_back(*retry);
    retry->Start();
}

void Cage::Impl::ListenKeeper(const KeeperState& kstate) {
    LOG(INFO) << "ListenKeeper: " << kstate.ToString();
    CHECK(!state_.IsLost()) << "Cage is already lost";
    CageState state;
    if (kstate.IsConnecting()) {
        CHECK(state_.IsConnected() || state_.IsReconnected());
        state_ = CageState::Suspended();
        timer_.AsyncWait(boost::bind(&Impl::SuspendTimeout, this),
                timeout_);
        NotifyListener();
    } else if (kstate.IsConnected()) {
        if (state_.IsUnknown()) {
            state_ = CageState::Connected();
        } else if (state_.IsSuspended()) {
            state_ = CageState::Reconnected();
            timer_.Cancel();
        } else {
            LOG(FATAL) << "Invalid cage state: " << state.ToString();
        }
        timeout_ = util::TimeDiff::Milliseconds(keeper_.timeout());
        NotifyListener();
    } else if (kstate.IsExpired() || kstate.IsAuthFailed()) {
        LOG(INFO) << "Keeper " << kstate.ToString();
        if (state_.IsSuspended()) {
            timer_.Cancel();
        }
        Lost();
    } else {
        LOG(FATAL) << "Invalid Keeper state: " << kstate.ToString();
    }
}

void Cage::Impl::SuspendTimeout() {
    CHECK(state_ == CageState::Suspended());
    LOG(INFO) << "Suspend timeout";
    Lost();
}

void Cage::Impl::NotifyListener() {
    listener_(state_);
}

void Cage::Impl::Lost() {
    state_ = CageState::Lost();
    Close();
    NotifyListener();
}

} // namespace cage
} // namespace ldd
