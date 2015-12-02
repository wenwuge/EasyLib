// listener_impl.h (2013-08-21)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_INTERNAL_LISTENER_IMPL_H_
#define LDD_NET_INTERNAL_LISTENER_IMPL_H_

#include "ldd/net/endpoint.h"
#include "ldd/net/listener.h"

namespace ldd {
namespace net {

class Listener::Impl {
public:
    Impl(EventLoop* event_loop);
    ~Impl();

    bool Open(const Endpoint& endpoint);
    void Close();
    
    bool IsOpen() const { return fd_ >= 0; }
    EventLoop* event_loop() const { return event_loop_; }
    int fd() const { return fd_; }
private:
    EventLoop* event_loop_;
    int fd_;
};

} // namespace net
} // namespace ldd


#endif // LDD_NET_INTERNAL_LISTENER_IMPL_H_

