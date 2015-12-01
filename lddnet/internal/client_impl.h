// client_impl.h (2013-08-26)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_INTERNAL_CLIENT_IMPL_H_
#define LDD_NET_INTERNAL_CLIENT_IMPL_H_

#include "ldd/net/client.h"

namespace ldd {
namespace net {

class Client::Impl {
public:
    Impl(Client* client, const Client::Options& options);

    boost::shared_ptr<Channel> Create(EventLoop* event_loop,
            const std::string& host, uint16_t port);
    boost::shared_ptr<Channel> Create(EventLoop* event_loop,
            const Endpoint& remote);

    Client* owner() const { return client_; }
    const Client::Options& options() const { return options_; }

private:
    Client* client_;
    Client::Options options_;

#ifdef RES_COUNTER
private:
    //counter for accept fd
    friend class Stat;
    friend class OutgoingChannel;
    static util::Atomic<uint64_t> next_id_;
    static util::Atomic<uint64_t> free_id_;
    static const uint64_t next_id() { return next_id_; }
    static const uint64_t free_id() { return free_id_; }
#endif
};

} // namespace net
} // namespace ldd

#endif // LDD_NET_INTERNAL_CLIENT_IMPL_H_
