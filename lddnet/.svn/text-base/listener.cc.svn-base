// listener.cc (2013-08-21)
// Li Xinjie (xjason.li@gmail.com)

#include "ldd/net/listener.h"
#include "internal/listener_impl.h"

namespace ldd {
namespace net {

Listener::Listener(EventLoop* event_loop)
    : impl_(new Impl(event_loop))
{
}

Listener::~Listener() {
    delete impl_;
}

bool Listener::Open(const Endpoint& endpoint) {
    return impl_->Open(endpoint);
}

void Listener::Close() {
    impl_->Close();
}

EventLoop* Listener::event_loop() const {
    return impl_->event_loop();
}

bool Listener::IsOpen() const {
    return impl_->IsOpen();
}

} // namespace net
} // namespace ldd
