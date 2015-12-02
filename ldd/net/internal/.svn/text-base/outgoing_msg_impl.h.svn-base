// outgoing_msg_impl.h (2013-08-20)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_INTERNAL_OUTGOING_MSG_IMPL_H_
#define LDD_NET_INTERNAL_OUTGOING_MSG_IMPL_H_

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include "ldd/net/event.h"
#include "ldd/net/outgoing_msg.h"
#include "channel_impl.h"

namespace ldd {
namespace net {

class OutgoingRequest;
class Channel;

class OutgoingMsg::Impl {
    static const int kDefaultSendTimeout = 5;
    static const int kDefaultRecvTimeout = 200;
    static const int kDefaultNextTimeout = 100;
    static const int kDefaultDoneTimeout = 300;
public:
    Impl(OutgoingMsg* owner)
        : owner_(owner), id_(-1), request_(NULL), done_(false), init_(false)
    {
#ifdef RES_COUNTER
        next_id_++;
#endif
    }

    ~Impl() {
#ifdef RES_COUNTER
        free_id_++;
#endif
    }


    boost::shared_ptr<OutgoingMsg> owner() const {
        return owner_->shared_from_this();
    }

    Id id() const { return id_; }
    const boost::shared_ptr<Channel>& channel() const { return channel_; }

    void Init(Id id, const boost::shared_ptr<Channel>& channel,
            const util::TimeDiff& send_timeout);
    bool OnRequest(Payload* payload);
    void OnResponse(const Payload& payload, bool last, Code code);
    void OnOk(Code code);
    void OnCancel();
    void OnFailed();
    void OnTimeout();

    void Cancel();
private:
    void DoCancel();
    void OnTimeout(TimerEvent* timer);
    void SetTimer(TimerEvent* timer, const util::TimeDiff& timeout,
            const util::TimeDiff& default_timeout);

    void ClearTimer();
    void ClearRequest();
    void CleanUp();
private:
    OutgoingMsg* owner_;
    Id id_;
    boost::shared_ptr<Channel> channel_;
    boost::scoped_ptr<TimerEvent> timer1_;
    boost::scoped_ptr<TimerEvent> timer2_;
    OutgoingRequest* request_;
    bool done_;
    bool init_;

#ifdef RES_COUNTER
private:
    friend class Stat;
    static util::Atomic<uint64_t> next_id_;
    static util::Atomic<uint64_t> free_id_;
    static const uint64_t next_id() { return next_id_; }
    static const uint64_t free_id() { return free_id_; }
#endif
};

} // namespace net
} // namespace ldd

#endif // LDD_NET_INTERNAL_OUTGOING_MSG_IMPL_H_
