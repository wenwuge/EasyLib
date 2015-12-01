// server.h (2013-08-17)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_SERVER_H_
#define LDD_NET_SERVER_H_

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include "ldd/net/incoming_msg_registry.h"
#include "ldd/net/event_loop.h"

namespace ldd {
namespace net {

class Listener;

class Server : public IncomingMsgRegistry {
public:
    enum ThreadDispatchPolicy {
        kNormal,
        kHashing,
    };
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

        // Batch accept incoming connections at one time.
        // default: 10
        int batch_accept;

        // Notifier for connected channel.
        // default: NULL
        Notifier notify_connected;

        // Notifier for closed channel.
        // default: NULL
        Notifier notify_closed;

        // A group of EventLoopThread objects for network io.
        // If NULL is specified, all network io will use the base event_loop.
        // default: NULL
        boost::ptr_vector<EventLoopThread>* threads;

        // Thread dispatching policy.
        // default: kNormal
        int threads_dispatching;

        // Socket read buffer
        // default: 4K
        int read_buffer_size;
        int write_buffer_size;

        Options();
    };

    explicit Server(const Options& options);
    ~Server();

    bool Start(Listener* listener);
    void Stop();

    const Options& options() const;
private:
    Impl* impl_;
};

} // namespace net
} // namespace ldd

#endif // LDD_NET_SERVER_H_
