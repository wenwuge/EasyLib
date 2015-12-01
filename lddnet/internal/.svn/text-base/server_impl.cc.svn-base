// server_impl.cc (2013-08-21)
// Li Xinjie (xjason.li@gmail.com)

#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <glog/logging.h>
#include "server_impl.h"
#include "channel_impl.h"
#include "event_loop_impl.h"

namespace ldd {
namespace net {

using namespace boost;

namespace {

const int kDefaultPulseInterval = 100;
const int kDefaultBatchAccept = 10;
const int kDefaultReadBufferSize = 4; //Kb
const int kDefaultWriteBufferSize = 4; //Kb

Server::Options SanitizeOptions(const Server::Options& options) {
    Server::Options o(options);
    if (o.pulse_interval <= 0) {
        o.pulse_interval = kDefaultPulseInterval;
    }
    if (o.batch_accept <= 0) {
        o.batch_accept = kDefaultBatchAccept;
    }
    if (o.threads_dispatching < 0 || 
            o.threads_dispatching > Server::kHashing) {
        o.threads_dispatching = Server::kNormal;
    }

    return o;
}

} // anonymouse namespace

#ifdef RES_COUNTER
util::Atomic<uint64_t> Server::Impl::next_id_;
util::Atomic<uint64_t> Server::Impl::free_id_;
#endif

Server::Options::Options()
    : pulse_interval(kDefaultPulseInterval),
      pulse_relaxed_checking(false),
      pulse_lazy_emitting(false),
      batch_accept(kDefaultBatchAccept),
      notify_connected(NULL),
      notify_closed(NULL),
      threads(NULL),
      threads_dispatching(kNormal),
      read_buffer_size(kDefaultReadBufferSize),
      write_buffer_size(kDefaultWriteBufferSize)
{
}

Server::Impl::Impl(Server* server, const Options& options)
    : server_(server),
      options_(SanitizeOptions(options)),
      listener_(NULL),
      thread_idx_(0)
{
}

Server::Impl::~Impl() {
    Stop();
}

bool Server::Impl::Start(Listener::Impl* listener) {
    CHECK_NOTNULL(listener);
    CHECK(listener_ == NULL);
    if (!listener->IsOpen()) {
        return false;
    }
    listener_ = listener;
    event_.reset(new FdEvent(listener_->event_loop()));
    Accept();
    return true;
}

void Server::Impl::Accept() {
    event_->AsyncWait(listener_->fd(), FdEvent::kReadable, 
            boost::bind(&Server::Impl::HandleAccept, this, _1));
}

boost::shared_ptr<Channel> Server::Impl::NewChannel() {
    EventLoop* event_loop = listener_->event_loop();
    if (options().threads && !options().threads->empty()) {
        event_loop = options().threads->at(
                thread_idx_++ % options().threads->size()).event_loop();
        DLOG(INFO) << "NewChannel, event_loop: " << event_loop
            << ", event_base: " << event_loop->impl_->event_base();
    }

    return make_shared<Channel>(event_loop, this);
}

boost::shared_ptr<Channel> Server::Impl::NewChannel(const Address& addr) {
    EventLoop* event_loop = listener_->event_loop();
    if (options().threads && !options().threads->empty()) {
        event_loop = options().threads->at(
                addr.ToU32() % options().threads->size()).event_loop();
        DLOG(INFO) << "NewCahnnel with addr, event_loop: " << event_loop
            << ", event_base: " << event_loop->impl_->event_base();
    }

    return make_shared<Channel>(event_loop, this);
}

void Server::Impl::OpenThreadChannel(const boost::shared_ptr<Channel>& channel) {
    channel->impl_->Open();
}

void Server::Impl::OpenChannel(const boost::shared_ptr<Channel>& channel) {
    if (channel->event_loop() != listener_->event_loop()) {
        channel->event_loop()->QueueInLoop(
                boost::bind(&Impl::OpenThreadChannel, channel));
    } else {
        channel->impl_->Open();
    }
}

void Server::Impl::HandleAccept(int) {
    int n = 0;
    while (n++ < options().batch_accept) {
        struct sockaddr_storage addr_storage;
        socklen_t addrlen = sizeof(addr_storage);
        int fd = accept(listener_->fd(), (sockaddr*)&addr_storage, &addrlen);
        if (fd < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                PLOG(INFO) << "Accept";
            }
            break;
        }
        Address addr;
        if (addr_storage.ss_family == AF_INET) {
            struct sockaddr_in* addr_in = (sockaddr_in*)&addr_storage;
            addr = Address(addr_in->sin_addr);
            LOG(INFO) << "accepted from : " << addr.ToString()
                << ", listen fd:" << listener_->fd()
                << ", client fd: " << fd;
        } else if (addr_storage.ss_family == AF_INET6) {
            LOG(WARNING) << "inet 6 socket connected, not supported";
            close(fd);
            continue;
        } else {
            LOG(ERROR) << "unknown socket family connected";
            close(fd);
            continue;
        }

#ifdef RES_COUNTER
        next_id_++;
#endif

        boost::shared_ptr<Channel> channel;
        if (options().threads_dispatching == kHashing) {
            channel = NewChannel(addr);
        } else {
            channel = NewChannel();
        }
        channel->impl_->set_socket(fd);
        OpenChannel(channel);
    }
    Accept();
}

void Server::Impl::Stop() {
    if (listener_) {
        event_.reset();
        listener_ = NULL;
    }
}

} // namespace net
} // namespace ldd
