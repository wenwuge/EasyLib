// server.cc (2013-08-19)
// Li Xinjie (xjason.li@gmail.com)

#include "server.h"
#include "internal/server_impl.h"

namespace ldd {
namespace net {

Server::Server(const Options& options)
    : impl_(new Impl(this, options))
{
}

Server::~Server() {
    delete impl_;
}

bool Server::Start(Listener* listener) {
    return impl_->Start(listener->impl_);
}

void Server::Stop() {
    impl_->Stop();
}

const Server::Options& Server::options() const {
    return impl_->options();
}


} // namespace net
} // namespace ldd
