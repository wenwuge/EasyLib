// client.h (2013-08-19)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_CLIENT_H_
#define LDD_NET_CLIENT_H_

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <ldd/net/incoming_msg_registry.h>

namespace ldd {
namespace net {

class EventLoop;
class Endpoint;
class Channel;

class Client : public IncomingMsgRegistry {
public:
    class Impl;
    typedef boost::function<void(const boost::shared_ptr<Channel>&)> Notifier;
    struct Options {
        // Heart beat interval in milliseconds
        // default: 100
        int pulse_interval;

        // Use relaxed heart-beat checking policy.
        // default: false
        bool pulse_relaxed_checking;
        
        // Use lazy heart-beat emitting policy.
        // default: false
        bool pulse_lazy_emitting;

        // Resolve timeout in milliseconds
        // default: 10
        int resolve_timeout;

        // Connect timeout in milliseconds
        // default: 10
        int connect_timeout;

        // Delay connect if consecutive failure reach this count.
        // default: 3
        int connect_delay_count;

        // Delay connect initial time, in milliseconds
        // default: 100
        int connect_delay_time_initial;

        // Delay connect final time, in milliseconds
        // default: 1000
        int connect_delay_time_final;

        // Delay connect unit time, each time the delayed connection failed,
        // next delay time will be adding this value.
        // default: 100
        int connect_delay_time_step;

        // Notifier for connected channel.
        // default: NULL
        Notifier notify_connected;

        // Notifier for connecting channel.
        // default: NULL
        Notifier notify_connecting;

        // Notifier for closed channel.
        // default: NULL
        Notifier notify_closed;

        // Socket read buffer
        // default: 4K
        int socket_read_buffer_size;

        Options();
    };

    explicit Client(const Options& options);
    ~Client();

    boost::shared_ptr<Channel> Create(EventLoop* event_loop,
            const std::string& host, uint16_t port);
    boost::shared_ptr<Channel> Create(EventLoop* event_loop,
            const Endpoint& remote);

    const Options& options() const;
private:
    Impl *impl_;
};

} // namespace net
} // namespace ldd

#endif // LDD_NET_CLIENT_H_
