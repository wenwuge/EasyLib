// push_msg.h (2013-08-11)
// Yan Gaofeng (yangaofeng@360.cn)

#ifndef __msg_content_H__
#define __msg_content_H__

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <ldd/net/incoming_msg.h>
#include <ldd/net/outgoing_msg.h>
#include <ldd/util/timestamp.h>
#include <ldd/util/atomic.h>

#include "user.h"

#include "proto/lcs_push.pb.h"

namespace lcs { namespace gateway {

class Ship;

class GateWayModule;

typedef ::google::protobuf::RepeatedPtrField<lcs::Channel::ClientID> RepeatedClientID;

class PushMsg : public ldd::net::DynamicIncomingMsg {
public:
    explicit PushMsg(GateWayModule *gateway, Ship *ship);
    virtual ~PushMsg();

    void PushMsgToUser(User *user);
    RepeatedClientID cids() { return push_.cids(); }
    const lcs::push::Message& push() const { return push_; }
    const std::string & request_str() const { return request_str_;}
    int cids_size() { return push_user_count_; }
    void RecordSucPushMsg(User *user);
    uint64_t push_task_id() {return push_task_id_;} 

protected:
    virtual void Init(const Payload& request, Action* next);
    virtual void Emit(Payload* response, Action* next);
    virtual void Done(Code* code);

private:
    PushMsg();
    PushMsg(const PushMsg &);

    void SendBroadcast();
    void SendMulticast(const ldd::net::Buffer &buff);
    bool SendUnicast(const ldd::net::Buffer &buff);
    void PrintMsgDetail();

    void StartResponseTimer();
    void ResponseTimerHandler();

private:
    friend class ToClient;
    ldd::net::Buffer msg_content_;
    GateWayModule *gateway_;
    Ship *ship_;
    ErrorCode error_code_;
    //ldd::net::Buffer res_buf_;
    uint64_t total_count_;
    uint64_t complete_count_;
    uint64_t failed_count_;
    uint64_t timeout_count_;

    uint64_t local_total_count_;
    uint64_t local_complete_count_;
    uint64_t local_failed_count_;
    uint64_t local_timeout_count_;

    lcs::push::Message push_;
    std::string request_str_;
    boost::shared_ptr<lcs::push::Response> res_;
    int push_user_count_;
    int push_type_;
    uint64_t push_task_id_;

    ldd::net::TimerEvent response_timer_;

    ldd::util::Timestamp init_time_;
    ldd::util::Timestamp done_time_;
    ldd::util::Timestamp ctor_time_;
    ldd::util::Timestamp emit_time_;

    int used_time_;

private:
    friend class GateWayModule;
    static ldd::util::Atomic<uint64_t> next_id_;
    static ldd::util::Atomic<uint64_t> free_id_;
    static const uint64_t next_id() { return next_id_; }
    static const uint64_t free_id() { return free_id_; }
};

//ToClient的生存期只与channel有关，与PushMsg无关, PushMsg只管创建
class ToClient : public ldd::net::OutgoingMsg {
public:
    explicit ToClient(const boost::shared_ptr<PushMsg>& push_msg
            , ldd::net::MessageType msg_type
            , const ldd::net::Buffer &buffer
            , int recv_timeout, User *user) 
        : parent_(push_msg)
          , msg_type_(msg_type)
          , msg_content_(buffer)
          , recv_timeout_(recv_timeout)
          , user_(user) {
              next_id_++;
              ctor_time_ = ldd::util::Timestamp::Now();
          }

    virtual ~ToClient() { free_id_++; }

    virtual ldd::net::MessageType type() const { return msg_type_; }

protected:
    virtual bool Init(Payload* request,
            ldd::util::TimeDiff* recv_timeout,
            ldd::util::TimeDiff* done_timeout);

    virtual bool Recv(const Payload& response,
            ldd::util::TimeDiff* recv_timeout);

    virtual void Done(const Result& result);

private:
    std::string channel_name();

private:
    ToClient();
    ToClient(const ToClient &);

    friend class PushMsg;
    boost::weak_ptr<PushMsg> parent_;
    Ship *ship_;
    ldd::net::MessageType msg_type_;
    ldd::net::Buffer msg_content_;

    ldd::util::Timestamp ctor_time_;
    ldd::util::Timestamp init_time_;
    int recv_timeout_;

    User *user_;

private:
    friend class GateWayModule;
    static ldd::util::Atomic<uint64_t> next_id_;
    static ldd::util::Atomic<uint64_t> free_id_;
    static const uint64_t next_id() { return next_id_; }
    static const uint64_t free_id() { return free_id_; }
};

} /*gateway*/
} /*lcs*/

#endif /*__msg_content_H__*/

