// default_msg.h (2013-07-30)
// Yan Gaofeng (yangaofeng@360.cn)

#include <pthread.h>
#include <set>
#include <ldd/util/timestamp.h>
#include <ldd/net/incoming_msg.h>
#include <ldd/net/outgoing_msg.h>
#include <ldd/util/atomic.h>

#include "error_code.h"
#include "osl/include/exp.h"
#include "osl/include/stat_time.h"

#define CLI_LOGIN_MSG 1000
#define CLI_SCONF_MSG 1001

namespace lcs {
namespace gateway {

class User;
class GateWayModule;
class ToServer;

extern int log_great_than;
extern uint32_t log_stat_interval;
extern __thread osl::StatTimeManager *stat_tsm;

class DefaultMsg : public ldd::net::DynamicIncomingMsg {
public:
    explicit DefaultMsg(GateWayModule *gateway);
    virtual ~DefaultMsg();

private:
    bool SendRequest();
    void CloseChannel();

protected:
    virtual void Init(const Payload& request, Action* next);
    virtual void Emit(Payload* response, Action* next);
    virtual void Done(Code* code);
    virtual void Cancel();

private:
    DefaultMsg();
    DefaultMsg(const DefaultMsg &);

    friend class ToServer;
    GateWayModule *gateway_;
    ldd::net::Buffer req_buf_;
    //boost::shared_ptr<ldd::net::OutgoingMsg> req_;
    ldd::net::Buffer res_buf_;
    bool first_;
    boost::weak_ptr<User> user_;
    ErrorCode error_code_;
    int retry_times_;
    bool first_send_;

    std::set<void *> retried_channels_;

    //ldd::util::Timestamp ctor_time_;
    ldd::util::Timestamp init_time_;
    //ldd::util::Timestamp emit_time_;
    //ldd::util::Timestamp done_time_;

    std::string used_server_;
    int used_server_port_;
    int used_server_stat_;

private:
    friend class GateWayModule;
    static ldd::util::Atomic<uint64_t> next_id_;
    static ldd::util::Atomic<uint64_t> free_id_;
    static const uint64_t next_id() { return next_id_; }
    static const uint64_t free_id() { return free_id_; }
};

class ToServer : public ldd::net::DynamicOutgoingMsg {
public:
    explicit ToServer(const boost::shared_ptr<DefaultMsg>& default_msg);
    virtual ~ToServer();
protected:
    virtual bool Init(Payload* request,
            ldd::util::TimeDiff* recv_timeout,
            ldd::util::TimeDiff* done_timeout);

    virtual bool Recv(const Payload& response,
            ldd::util::TimeDiff* recv_timeout);

    virtual void Done(const Result& result);

private:
    void ProcessServerError(int ec);

    std::string channel_name();

private:
    ToServer();
    ToServer(const ToServer &);

    friend class DefaultMsg;
    boost::weak_ptr<DefaultMsg> parent_;
    ldd::net::IncomingMsg::Id parent_id_;

    //ldd::util::Timestamp ctor_time_;
    ldd::util::Timestamp init_time_;
    //ldd::util::Timestamp recv_time_;
    //ldd::util::Timestamp done_time_;

private:
    friend class GateWayModule;
    static ldd::util::Atomic<uint64_t> next_id_;
    static ldd::util::Atomic<uint64_t> free_id_;
    static const uint64_t next_id() { return next_id_; }
    static const uint64_t free_id() { return free_id_; }
};


} /*gateway*/
} /*lcs*/

