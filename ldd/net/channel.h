#ifndef LDD_NET_CHANNEL_H_
#define LDD_NET_CHANNEL_H_

#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>
#include <boost/any.hpp>

#include <ldd/util/time_diff.h>
#include "ldd/net/server.h"
#include "ldd/net/client.h"

namespace ldd {
namespace net {

class EventLoop;
class OutgoingMsg;
class Endpoint;

class Channel : public boost::enable_shared_from_this<Channel>,
                private boost::noncopyable
{
public:
    class Impl;
    enum Type {
        kIncoming,
        kOutgoing,
    };
    enum State {
        kClosed = 0,
        kConnecting = 1,
        kConnected  = 2,
    };
    Channel(EventLoop* event_loop, Server::Impl* server);
    Channel(EventLoop* event_loop, Client::Impl* client,
            const std::string&, uint16_t);
    Channel(EventLoop* event_loop, Client::Impl* client,
            const Endpoint&);
    ~Channel();

    std::string name() const;
    Type type() const;
    State state() const;
    bool IsConnected() const { return state() == kConnected; }
    bool IsConnecting() const { return state() == kConnecting; }
    bool IsClosed() const { return state() == kClosed; }
    EventLoop *event_loop() const;
    const Endpoint& self_endpoint() const;
    const Endpoint& peer_endpoint() const;

    void Close();
    bool Post(const boost::shared_ptr<OutgoingMsg>& message,
            const ldd::util::TimeDiff& timeout = util::TimeDiff(),
            bool force = false);

    int socket();
    void set_context(const boost::any& context);
    const boost::any& context() const;

private:
    void DoClose();
    void DoPost(const boost::shared_ptr<OutgoingMsg>& message,
            const ldd::util::TimeDiff& timeout);
    friend class Client;
    friend class Server;
    friend class IncomingMsg;
    friend class OutgoingMsg;
    Impl *impl_;
};

} // namespace net
} // namespace ldd

#endif // LDD_NET_CHANNEL_H_
