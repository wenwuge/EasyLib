// client.cc (2013-08-19)
// Li Xinjie (xjason.li@gmail.com)

#include "client.h"
#include "internal/client_impl.h"

namespace ldd {
namespace net {

Client::Client(const Options& options)
    : impl_(new Impl(this, options))
{
}

Client::~Client() {
    delete impl_;
}


boost::shared_ptr<Channel> Client::Create(EventLoop* event_loop,
        const std::string& host, uint16_t port) {
    return impl_->Create(event_loop, host, port);
}

boost::shared_ptr<Channel> Client::Create(EventLoop* event_loop,
        const Endpoint& remote) {
    return impl_->Create(event_loop, remote);
}

const Client::Options& Client::options() const {
    return impl_->options();
}

} // namespace net
} // namespace ldd
