// incoming_msg_impl.h (2013-08-20)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_INTERNAL_INCOMING_MSG_IMPL_H_
#define LDD_NET_INTERNAL_INCOMING_MSG_IMPL_H_

#include "ldd/net/incoming_msg.h"
#include "ldd/util/atomic.h"

namespace ldd {
namespace net {

class Channel;
class OutgoingResponse;

class IncomingMsg::Impl {
public:
    typedef ldd::net::Payload Payload;
    typedef ldd::net::Code Code;
    typedef ldd::net::MessageId Id;
    typedef ldd::net::MessageType Type;

    Impl(IncomingMsg* m)
        : owner_(m), id_(-1), canceled_(false),
          waiting_(false),
          response_(NULL)
    {
#ifdef RES_COUNTER
        next_id_++;
#endif
    }

    virtual ~Impl();

    boost::shared_ptr<IncomingMsg> owner() const {
        return owner_->shared_from_this();
    }
    const boost::shared_ptr<Channel>& channel() const { return channel_; }
    Id id() const { return id_; }
    bool IsCanceled() const { return canceled_; }

    void Init(Id id, const boost::shared_ptr<Channel>& channel);
    void Notify(bool done);

    void OnRequest(const Payload& payload);
    void DoEmit();
    void DoDone();
    void DoCancel();
private:
    void WriteResponse(const Payload& payload);
    void WriteDone(Code code);

private:
    IncomingMsg* owner_;
    Id id_;
    boost::shared_ptr<Channel> channel_;
    bool canceled_;
    bool waiting_;
    OutgoingResponse* response_;
    //boost::shared_ptr<char> packet_;

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

#endif // LDD_NET_INTERNAL_INCOMING_MSG_IMPL_H_
