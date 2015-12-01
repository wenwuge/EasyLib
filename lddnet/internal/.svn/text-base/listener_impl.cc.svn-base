// listener_impl.cc (2013-08-21)
// Li Xinjie (xjason.li@gmail.com)

#include <unistd.h>
#include <fcntl.h>
#include <glog/logging.h>
#include "listener_impl.h"
#include "event_loop_impl.h"

namespace ldd {
namespace net {

Listener::Impl::Impl(EventLoop* event_loop)
    : event_loop_(event_loop),
      fd_(-1)
{
}

Listener::Impl::~Impl() {
    Close();
}

bool Listener::Impl::Open(const Endpoint& endpoint) {
    CHECK(!IsOpen());
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        PLOG(ERROR) << "socket()";
        return false;
    }
    int reuse = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        PLOG(ERROR) << "setsockopt()";
        close(fd);
        return false;
    }
    int flags = 0;
    if ((flags = fcntl(fd, F_GETFL, 0)) < 0) {
        PLOG(ERROR) << "fcntl(F_GETFL)";
        close(fd);
        return false;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        PLOG(ERROR) << "fcntl(F_SETFL, O_NONBLOCK)";
        close(fd);
        return false;
    }
    struct sockaddr_in addr;
    addr.sin_family = endpoint.address().family();
    addr.sin_addr = endpoint.address().ToV4();
    addr.sin_port = htons(endpoint.port());
    if (bind(fd, (const sockaddr*)&addr, sizeof(addr)) < 0) {
        PLOG(ERROR) << "bind()";
        close(fd);
        return false;
    }
    if (listen(fd, SOMAXCONN) < 0) {
        PLOG(ERROR) << "listen()";
        close(fd);
        return false;
    }
    fd_ = fd;

    DLOG(INFO) << "open listen fd: " << fd_;

    return true;
}

void Listener::Impl::Close() {
    if (IsOpen()) {
        if (close(fd_) < 0) {
            PLOG(ERROR) << "close()";
        }
        fd_ = -1;
    }
}

} // namespace net
} // namespace ldd
