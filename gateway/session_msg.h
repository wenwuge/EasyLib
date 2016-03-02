// session_msg.h (2014-02-12)
// Yan Gaofeng (yangaofeng@360.cn)
#ifndef __SESSION_MSG_H__
#define __SESSION_MSG_H_

#include <ldd/util/timestamp.h>
#include <ldd/net/outgoing_msg.h>

namespace lcs { namespace gateway {

class Ship;

class ToSession: public ldd::net::OutgoingMsg {
public:
    explicit ToSession(ldd::net::MessageType msg_type
            , const ldd::net::Buffer &buffer
            , int recv_timeout,int retried_times, Ship* ship) 
        : msg_type_(msg_type)
          , msg_content_(buffer)
          , recv_timeout_(recv_timeout)
          , retried_times_(retried_times) 
          , ship_(ship) {
                next_id_++;
                ctor_time_ = ldd::util::Timestamp::Now();
          }

    virtual ~ToSession() { free_id_++; }

    virtual ldd::net::MessageType type() const { return msg_type_; }

protected:
    virtual bool Init(Payload* request,
            ldd::util::TimeDiff* recv_timeout,
            ldd::util::TimeDiff* done_timeout);

    virtual bool Recv(const Payload& response,
            ldd::util::TimeDiff* recv_timeout);

    virtual void Done(const Result& result);


private:
    ToSession();
    ToSession(const ToSession &);

    ldd::net::MessageType msg_type_;
    ldd::net::Buffer msg_content_;

    ldd::util::Timestamp ctor_time_;
    ldd::util::Timestamp init_time_;
    int recv_timeout_;
    int retried_times_;
    Ship* ship_;

    std::string channel_name_;

private:
    friend class GateWayModule;
    static ldd::util::Atomic<uint64_t> next_id_;
    static ldd::util::Atomic<uint64_t> free_id_;
    static const uint64_t next_id() { return next_id_; }
    static const uint64_t free_id() { return free_id_; }
};

} /*gateway*/
} /*lcs*/

#endif /*__SESSION_MSG_H_*/
