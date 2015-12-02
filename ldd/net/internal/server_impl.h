// server_impl.h (2013-08-21)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_INTERNAL_SERVER_IMPL_H_
#define LDD_NET_INTERNAL_SERVER_IMPL_H_

#include "ldd/net/server.h"
#include "listener_impl.h"
#include "ldd/net/event.h"

namespace ldd {
namespace net {

class Server::Impl {
public:
    explicit Impl(Server* server, const Options& options);
    ~Impl();

    bool Start(Listener::Impl* listener);
    void Stop();

    Server* owner() { return server_; }
    const Options& options() { return options_; }
private:
    void Accept();
    boost::shared_ptr<Channel> NewChannel();
    boost::shared_ptr<Channel> NewChannel(const Address& addr);

    void HandleAccept(int);
    void OpenChannel(const boost::shared_ptr<Channel>& channel);
    static void OpenThreadChannel(const boost::shared_ptr<Channel>& channel);

    Server* server_;
    Options options_;
    Listener::Impl* listener_;
    boost::scoped_ptr<FdEvent> event_;
    uint32_t thread_idx_;

#ifdef RES_COUNTER
private:
    //counter for accept fd
    friend class Stat;
    friend class IncomingChannel;
    static util::Atomic<uint64_t> next_id_;
    static util::Atomic<uint64_t> free_id_;
    static const uint64_t next_id() { return next_id_; }
    static const uint64_t free_id() { return free_id_; }
#endif
};

} // namespace net
} // namespace ldd

#endif // LDD_NET_INTERNAL_SERVER_IMPL_H_
