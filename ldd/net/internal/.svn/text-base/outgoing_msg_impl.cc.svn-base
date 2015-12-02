// outgoing_msg_impl.cc (2013-08-26)
// Li Xinjie (xjason.li@gmail.com)

#include <boost/bind.hpp>
#include <glog/logging.h>
#include "outgoing_msg_impl.h"
#include "outgoing_packet.h"

namespace ldd {
namespace net {

#ifdef RES_COUNTER
util::Atomic<uint64_t> OutgoingMsg::Impl::next_id_;
util::Atomic<uint64_t> OutgoingMsg::Impl::free_id_;
#endif

void OutgoingMsg::Impl::Init(Id id,
        const boost::shared_ptr<Channel>& c,
        const util::TimeDiff& send_timeout) {
    id_ = id;
    channel_ = c;
    timer1_.reset(new TimerEvent(channel()->event_loop()));
    timer2_.reset(new TimerEvent(channel()->event_loop()));

    request_ = new OutgoingRequest(owner());
    SetTimer(timer1_.get(), send_timeout,
        util::TimeDiff::Milliseconds(kDefaultSendTimeout));
    channel()->impl_->Write(request_);

    init_ = true;
}

void OutgoingMsg::Impl::Cancel() {
    DoCancel();
}

void OutgoingMsg::Impl::DoCancel() {
    CHECK_GE(id(), 0);
    CHECK(channel());

    boost::shared_ptr<OutgoingMsg> msg =
        channel()->impl_->GetOutgoingMsg(id(), true);
    if (!msg || owner() != msg) {
        LOG(INFO) << channel()->name()
            << " cancel non-active OutgoingMsg id = " << id();
        return;
    }
    OutgoingCancel* cancel = new OutgoingCancel(id());
    channel()->impl_->Write(cancel);
    OnCancel();
}

bool OutgoingMsg::Impl::OnRequest(Payload* payload) {

    DLOG(INFO) << "enter OnRequest";

    CHECK_NOTNULL(request_);
    request_ = NULL;

    util::TimeDiff recv_timeout;
    util::TimeDiff done_timeout;
    bool ok = owner_->Init(payload, &recv_timeout, &done_timeout);
    if (ok) {
        SetTimer(timer1_.get(), recv_timeout,
                util::TimeDiff::Milliseconds(kDefaultRecvTimeout));
        SetTimer(timer2_.get(), done_timeout,
                util::TimeDiff::Milliseconds(kDefaultDoneTimeout));
        return true;
    } else {
        OnCancel();
        return false;
    }
}

void OutgoingMsg::Impl::OnResponse(const Payload& payload, bool last,
        Code code) {
    util::TimeDiff next_timeout;
    bool ok = owner_->Recv(payload, &next_timeout);
    if (ok) {
        if (last) {
            OnOk(code);
        } else {
            SetTimer(timer1_.get(), next_timeout,
                    util::TimeDiff::Milliseconds(kDefaultNextTimeout));
        }
    } else {
        DoCancel();
    }
}

void OutgoingMsg::Impl::OnOk(Code code) {
    CHECK(!done_);
    owner_->Done(Result::Ok(code));
    CleanUp();
}

void OutgoingMsg::Impl::OnCancel() {
    CHECK(!done_);
    owner_->Done(Result::Canceled());
    CleanUp();
}

void OutgoingMsg::Impl::OnFailed() {
    CHECK(!done_);
    owner_->Done(Result::Failed());
    CleanUp();
}

void OutgoingMsg::Impl::OnTimeout() {
    CHECK(!done_);
    LOG(INFO) << channel()->name() << "id = " << id() << " timeout";
    owner_->Done(Result::TimedOut());
    CleanUp();
}
void OutgoingMsg::Impl::OnTimeout(TimerEvent* timer) {
    if (timer == timer1_.get()) {
        timer2_->Cancel();
    } else {
        timer1_->Cancel();
    }
    OnTimeout();
}

void OutgoingMsg::Impl::SetTimer(TimerEvent* timer,
        const util::TimeDiff& timeout,
        const util::TimeDiff& default_timeout) {
    if (timeout.ToMicroseconds() > 0) {
        DLOG(INFO) << "SetTimer call AsyncWait, obj: " << timer;
        timer->AsyncWait(boost::bind(&Impl::OnTimeout, this, timer),
                timeout);
    } else {
        DLOG(INFO) << "SetTimer call AsyncWait, obj: " << timer;
        timer->AsyncWait(boost::bind(&Impl::OnTimeout, this, timer),
                default_timeout);
    }
}

void OutgoingMsg::Impl::ClearTimer() {

    if (!init_) {
        LOG(WARNING) << "Uninit outgoing msg when clear timer";
        return;
    }

    if (timer1_) {
        timer1_->Cancel();
    }

    if (timer2_) {
        timer2_->Cancel();
    }
}

void OutgoingMsg::Impl::ClearRequest() {
    if (request_) {
        request_->Disable();
        request_ = NULL;
    }
}

void OutgoingMsg::Impl::CleanUp() {
    if (!init_) {
        LOG(WARNING) << "uninit outgoing msg when clean up";
        return;
    }

    ClearRequest();
    ClearTimer();
    done_ = true;
    channel()->impl_->GetOutgoingMsg(id(), true);
}

} // namespace net
} // namespace ldd
