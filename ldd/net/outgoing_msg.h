#ifndef LDD_NET_OUTGOING_MSG_H_
#define LDD_NET_OUTGOING_MSG_H_

#include <map>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <ldd/util/time_diff.h>
#include "ldd/net/payload.h"
#include "ldd/net/buffer.h"
#include "ldd/net/result.h"

namespace ldd {
namespace net {

class Channel;

class OutgoingMsg : public boost::enable_shared_from_this<OutgoingMsg>,
                    private boost::noncopyable {
public:
    class Impl;
    typedef ldd::net::Payload Payload;
    typedef ldd::net::MessageId Id;
    typedef ldd::net::MessageType Type;
    typedef ldd::net::Result Result;

    virtual ~OutgoingMsg();
    virtual Type type() const = 0;

    Id id() const;
    const boost::shared_ptr<Channel>& channel() const;

    // Cancel the message processing.
    void Cancel();
protected:
    OutgoingMsg();
    virtual bool Init(Payload* request,
            ldd::util::TimeDiff* recv_timeout,
            ldd::util::TimeDiff* done_timeout) = 0;
    virtual bool Recv(const Payload& response,
            ldd::util::TimeDiff* recv_timeout) = 0;
    virtual void Done(const Result& result) = 0;
private:
    void DoCancel();
    Impl *impl_;
    friend class Channel;
    friend class OutgoingRequest;
    friend class IncomingResponse;
    friend class IncomingEnd;
};

template <MessageType N>
class TypedOutgoingMsg : public OutgoingMsg {
public:
    static const Type kType = N;
    Type type() const { return kType; }
protected:
    virtual bool Init(Payload* request,
            ldd::util::TimeDiff* recv_timeout,
            ldd::util::TimeDiff* done_timeout) = 0;
    virtual bool Recv(const Payload& response,
            ldd::util::TimeDiff* recv_timeout) = 0;
    virtual void Done(const Result& result) = 0;
};

class DynamicOutgoingMsg : public OutgoingMsg {
public:
    ~DynamicOutgoingMsg(); 
    void set_type(Type type);
    Type type() const { return type_; }
protected:
    DynamicOutgoingMsg(): type_(0) {}
    DynamicOutgoingMsg(Type type);
    virtual bool Init(Payload* request,
            ldd::util::TimeDiff* recv_timeout,
            ldd::util::TimeDiff* done_timeout) = 0;
    virtual bool Recv(const Payload& response,
            ldd::util::TimeDiff* recv_timeout) = 0;
    virtual void Done(const Result& result) = 0;
private:
    Type type_;

#if 0
private:
    friend class Stat;

    struct NextFree {
        NextFree() 
            : next_id_(new util::Atomic<uint64_t>()), 
              free_id_(new util::Atomic<uint64_t>())
        { *next_id_ = 0; *free_id_ = 0; }
        ~NextFree() {}

        boost::shared_ptr<util::Atomic<uint64_t> > next_id_;
        boost::shared_ptr<util::Atomic<uint64_t> > free_id_;
    };

    static std::map<Type,  NextFree> stat_;

    typedef std::map<Type, NextFree>::iterator STAT_IT;

    static void Inc(Type type);
    static void Dec(Type type);
#endif /*RES_COUNTER*/
};

} // namespace net
} // namespace ldd

#endif // LDD_NET_OUTGOING_MSG_H_

