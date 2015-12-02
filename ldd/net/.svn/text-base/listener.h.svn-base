// listener.h (2013-08-19)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_LISTENER_H_
#define LDD_NET_LISTENER_H_

#include <boost/noncopyable.hpp>
#include "ldd/net/endpoint.h"

namespace ldd {
namespace net {

class EventLoop;
class Endpoint;

class Listener : boost::noncopyable {
public:
    class Impl;
    Listener(EventLoop* event_loop);
    ~Listener();

    bool Open(const Endpoint& endpoint);
    void Close();

    EventLoop* event_loop() const;
    bool IsOpen() const;
private:
    Impl* impl_;
    friend class Server;
};

} // namespace net
} // namespace ldd

#endif // LDD_NET_LISTENER_H_
