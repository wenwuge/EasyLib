// channel_impl.h (2013-08-19)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_INTERNAL_CHANNEL_IMPL_H_
#define LDD_INTERNAL_CHANNEL_IMPL_H_

#include <boost/any.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/ptr_deque.hpp>

#include "ldd/net/incoming_msg_registry.h"
#include "ldd/net/channel.h"
#include "ldd/net/event.h"
#include "ldd/net/endpoint.h"
#include "ldd/net/server.h"
#include "ldd/net/client.h"
#include "server_impl.h"
#include "client_impl.h"
#include "protocol.h"
#include "task.h"
//#include "socket.h"

namespace ldd {
namespace net {

class OutgoingPacket;
class PulseEmitter;
class PulseChecker;
class OutgoingChannel;

class EventCtx {
public:
    EventCtx(const boost::shared_ptr<Channel> &owner
            , Channel::Impl *impl)
        : owner_(owner)
          , impl_(impl)
    {
#ifdef RES_COUNTER
        next_id_++;
#endif
    }

    ~EventCtx() {
#ifdef RES_COUNTER
        free_id_++;
#endif
    }

    boost::shared_ptr<Channel> owner_;
    Channel::Impl *impl_;

#ifdef RES_COUNTER
private:
    friend class Stat;
    static util::Atomic<uint64_t> next_id_;
    static util::Atomic<uint64_t> free_id_;
    static const uint64_t next_id() { return next_id_; }
    static const uint64_t free_id() { return free_id_; }
#endif
};

class DnsCtx {
public:
    DnsCtx(OutgoingChannel *oc
            , const boost::shared_ptr<Channel> &owner
            , struct evdns_base *dns_base) 
        : oc_(oc)
          , owner_(owner)
          , dns_base_(dns_base)
    {
#ifdef RES_COUNTER
        next_id_++;
#endif
    }
    ~DnsCtx() {
#ifdef RES_COUNTER
        free_id_++;
#endif
    }

    OutgoingChannel *oc_;
    boost::shared_ptr<Channel> owner_;
    struct evdns_base *dns_base_;

#ifdef RES_COUNTER
private:
    friend class Stat;
    static util::Atomic<uint64_t> next_id_;
    static util::Atomic<uint64_t> free_id_;
    static const uint64_t next_id() { return next_id_; }
    static const uint64_t free_id() { return free_id_; }
#endif
};

class Channel::Impl {
public:
    typedef boost::function<void(const boost::shared_ptr<Channel>&)> Notifier;
    typedef Channel Owner;
    typedef Channel::Type Type;

    enum {
        kReadHeader=0,
        kReadBody,
    };


    Impl(Owner* owner, EventLoop* event_loop);
    virtual ~Impl();

    virtual Type type() const = 0;
    virtual void Open() = 0;
    void Close();
    void CloseSocket();
    virtual IncomingMsgRegistry* registry() const = 0;

    virtual std::string name() const = 0;
    EventLoop* event_loop() const { return event_loop_; }
    struct event_base *event_base();
    State state() const { return state_; }
    void set_state(State state) { state_ = state; }
    void set_socket(int socket);

    Header::Parser header() { return Header::Parser(header_->data()); }

    const Endpoint& self_endpoint() const { return self_endpoint_; }
    const Endpoint& peer_endpoint() const { return peer_endpoint_; }
    uint64_t id() const { return id_; }
    
    void Post(const boost::shared_ptr<OutgoingMsg>& message,
            const ldd::util::TimeDiff& timeout);
    
    bool AddIncomingMsg(const boost::shared_ptr<IncomingMsg>& incoming_msg);
    boost::shared_ptr<IncomingMsg> GetIncomingMsg(
            int32_t id, bool remove = false);
    boost::shared_ptr<OutgoingMsg> GetOutgoingMsg(
            int32_t id, bool remove = false);
    void Write(OutgoingPacket* packet);
    
    int socket() { return socket_; }
    boost::shared_ptr<Channel> owner() { return owner_->shared_from_this(); }

    int32_t NextOutgoingMsgId() {
        return (++outgoing_msg_id_) & 0x7fffffff;
    }

    void set_context(const boost::any& context)
    { context_ = context; }

    const boost::any& context() const
    { return context_; }

    virtual void OnPing() = 0;
    virtual void OnPong() = 0;
    virtual void OnError() = 0;
    virtual void OnClose() = 0;
    virtual void DoPulse() = 0;
    virtual void OnConnect(int error) = 0;

    void OnIncomingPacket();
    void OnOutgoingPacket();
    void OnPulseFailed();

protected:
    friend class PacketReader;
    bool FillEndpoints(bool remote);
    void Notify(const Notifier& notifier);
    void ClearIncomingMsg();
    void ClearOutgoingMsg();

    void AddEvent(short flag) { SetEvent(events_ | flag); }
    void DelEvent(short flag) { SetEvent(events_ & ~flag); }
    void SetEvent(short events);

    void OnRead();
    void OnWrite();
    void ReadHeader();
    void ReadBody();
    void ProcessPacket();
    void DoWrite();

    static void EventNotify(int fd, short what, void *arg);

    void RealWrite();

    int socket_;
    ReadTask read_header_task_;
    ReadTask read_body_task_;
    WriteTask write_task_;
    EventCtx *event_ctx_;

private:
    typedef std::map<int32_t, boost::shared_ptr<OutgoingMsg> > OutgoingMsgMap;
    typedef std::map<int32_t, boost::shared_ptr<IncomingMsg> > IncomingMsgMap;

    Owner* owner_;
    EventLoop* event_loop_;
    State state_;

    short events_;
    int read_state_;
    struct event *socket_ev_;

    boost::shared_ptr<Buffer> header_;
    boost::shared_ptr<Buffer> packet_;
    int packet_size_;

    IncomingMsgMap imsgs_;
    OutgoingMsgMap omsgs_;

    boost::ptr_deque<OutgoingPacket> pending_packets_;
    boost::ptr_vector<OutgoingPacket> writing_packets_;
    std::deque<boost::shared_ptr<Buffer> > writing_buffers_;

    int32_t outgoing_msg_id_;

    boost::any context_;
    uint64_t id_;

protected:
    Endpoint self_endpoint_;
    Endpoint peer_endpoint_;
    boost::scoped_ptr<PulseEmitter> pulse_emitter_;
    boost::scoped_ptr<PulseChecker> pulse_checker_;

private:
    static util::Atomic<uint64_t> next_id_;
};

class IncomingChannel : public Channel::Impl {
    typedef Channel::Impl Super;
public:
    typedef Channel::Type Type;
    IncomingChannel(Owner* owner, EventLoop* event_loop,
            Server::Impl* server);
    virtual ~IncomingChannel();
    virtual Type type() const { return Channel::kIncoming; }
    virtual std::string name() const;
    
    virtual void Open();
    virtual IncomingMsgRegistry* registry() const { return server_->owner(); }
private:
    void InitPulse();
    virtual void OnPing();
    virtual void OnPong();
    virtual void OnError();
    virtual void OnClose();
    virtual void DoPulse();
    virtual void OnConnect(int error) {}

    const Server::Options& options() const { return server_->options(); }
private:
    Server::Impl* server_;

#ifdef RES_COUNTER
private:
    friend class Stat;
    static util::Atomic<uint64_t> next_id_;
    static util::Atomic<uint64_t> free_id_;
    static const uint64_t next_id() { return next_id_; }
    static const uint64_t free_id() { return free_id_; }
#endif
};

class OutgoingChannel : public Channel::Impl {
public:
    typedef Channel::Impl Super;
    typedef Channel::Type Type;


    OutgoingChannel(Owner* owner, EventLoop* event_loop,
            Client::Impl* client, const std::string& host, uint16_t port);
    OutgoingChannel(Owner* owner, EventLoop* event_loop,
            Client::Impl* client, const Endpoint& remote);
    virtual ~OutgoingChannel();

    virtual Type type() const { return Channel::kOutgoing; }
    virtual std::string name() const;

    virtual void Open();
    virtual IncomingMsgRegistry* registry() const { return client_->owner(); }
private:
    void InitPulse();
    virtual void OnPing();
    virtual void OnPong();
    virtual void OnError();
    virtual void OnClose();
    virtual void DoPulse();

    void DoConnect();
    void Reconnect();
    void Resolve();
    void OnResolveTimeout(const boost::shared_ptr<Channel>& o);
    static void OnResolve(int errcode
            , struct evutil_addrinfo *addr
            , void *ptr);
    void DoResolve(int errcode, struct evutil_addrinfo *addr);
    void Connect();
    void OnConnectTimeout(const boost::shared_ptr<Channel>& o);
    virtual void OnConnect(int error);

    void AsyncConnect(const std::string &host
            , uint16_t port
            , int timeout);

    void DelayConnect();
    void DoDelayConnect(const boost::shared_ptr<Channel>&);

    const Client::Options& options() const { return client_->options(); }

private:
    Client::Impl* client_;
    TimerEvent timer_;

    std::string host_;
    uint16_t port_;
    bool need_resolve_;
    int connect_failure_;
    bool connect_delay_;
    struct evdns_getaddrinfo_request *dns_req_;
    DnsCtx *dns_ctx_;

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

#endif // LDD_INTERNAL_CHANNEL_IMPL_H_
