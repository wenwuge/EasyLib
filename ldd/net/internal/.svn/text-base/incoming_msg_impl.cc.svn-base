// incoming_msg_impl.cc (2013-08-26)
// Li Xinjie (xjason.li@gmail.com)

#include <boost/bind.hpp>
#include <glog/logging.h>
#include "incoming_msg_impl.h"
#include "channel_impl.h"
#include "outgoing_packet.h"

namespace ldd {
namespace net {

#ifdef RES_COUNTER
util::Atomic<uint64_t> IncomingMsg::Impl::next_id_;
util::Atomic<uint64_t> IncomingMsg::Impl::free_id_;
#endif

IncomingMsg::Impl::~Impl() {
    if (response_) {
        delete response_;
    }
#ifdef RES_COUNTER
    free_id_++;
#endif
}

void IncomingMsg::Impl::Init(Id id, 
        const boost::shared_ptr<Channel>& channel) {
    DLOG(INFO) << "enter Init";
    id_ = id;
    channel_ = channel;
}

void IncomingMsg::Impl::Notify(bool done) {

    DLOG(INFO) << "enter Notify";

    if (waiting_) {
        if (done) {
            DoDone();
        } else {
            DoEmit();
        }
    }
}

void IncomingMsg::Impl::OnRequest(const Payload& payload)
{
    DLOG(INFO) << "enter OnRequest";

    //packet_ = packet;

    Action next = kWait;
    owner_->Init(payload, &next);
    if (next == kEmit) {
        DoEmit();
    } else if (next == kDone) {
        DoDone();
    } else if (next == kWait) {
        waiting_ = true;
    } else {
        LOG(FATAL) << channel()->name()
            << "Invalid Next step for IncomingMsg::Init() = "
            << next;
    }
}

void IncomingMsg::Impl::DoEmit() {
    DLOG(INFO) << "enter DoEmit";
    waiting_ = false;
    Action next;
    do {
        Payload payload;
        next = kWait;
        owner_->Emit(&payload, &next);
        WriteResponse(payload);
    } while (next == kEmit);
    if (next == kDone) {
        DoDone();
    } else if (next == kWait) {
        waiting_ = true;
    } else {
        LOG(FATAL) << channel()->name()
            << "Invalid Next step for IncomingMsg::Emit() = "
            << next;
    }
}

void IncomingMsg::Impl::DoDone() {

    DLOG(INFO) << "enter DoDone";

    waiting_ = false;
    Code code = 0;
    owner_->Done(&code);
    WriteDone(code);
}

void IncomingMsg::Impl::DoCancel() {

    DLOG(INFO) << "enter DoCancel";

    if (!canceled_) {
        canceled_ = true;
        owner_->Cancel();
        channel()->impl_->GetIncomingMsg(id(), true);
    }
}

void IncomingMsg::Impl::WriteResponse(const Payload& payload) {
    DLOG(INFO) << "enter WriteResponse";
    if (canceled_) {
        return;
    }
    OutgoingResponse* response = new OutgoingResponse(id(), payload);
    if (response_) {
        channel()->impl_->Write(response_);
    }
    response_ = response;
}

void IncomingMsg::Impl::WriteDone(Code code) {
    DLOG(INFO) << "enter WriteDone";
    if (canceled_) {
        return;
    }
    if (response_) {
        DLOG(INFO) << "response is not empty";
        response_->set_last(true);
        response_->set_code(code);
        channel()->impl_->Write(response_);
        response_ = NULL;
    } else {
        DLOG(INFO) << "response is empty and ctor a outgoing end packet";
        OutgoingEnd* end = new OutgoingEnd(id(), code);
        channel()->impl_->Write(end);
    }
    channel()->impl_->GetIncomingMsg(id(), true);
}
} // namespace net
} // namespace ldd
