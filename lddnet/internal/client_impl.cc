// client_impl.cc (2013-08-26)
// Li Xinjie (xjason.li@gmail.com)

#include "client_impl.h"
#include "event_loop_impl.h"
#include "channel_impl.h"

namespace ldd {
namespace net {

namespace {

const int kDefaultPulseInterval = 100;
const int kDefaultResolveTimeout = 10;
const int kDefaultConnectTimeout = 10;
const int kDefaultConnectDelayCount = 3;
const int kDefaultConnectDelayTimeInitial = 100;
const int kDefaultConnectDelayTimeFinal = 2000;
const int kDefaultConnectDelayTimeStep = 100;
const int kDefaultSocketReadBufferSize = 4;//kb

Client::Options SanitizeOptions(const Client::Options& options) {
    Client::Options o(options);
    if (o.pulse_interval <= 0) {
        o.pulse_interval = kDefaultPulseInterval;
    }
    if (o.resolve_timeout <= 0) {
        o.resolve_timeout = kDefaultResolveTimeout;
    }
    if (o.connect_timeout <= 0) {
        o.connect_timeout = kDefaultConnectTimeout;
    }
    if (o.connect_delay_count <= 0) {
        o.connect_delay_count = kDefaultConnectDelayCount;
    }
    if (o.connect_delay_time_initial <= 0) {
        o.connect_delay_time_initial = kDefaultConnectDelayTimeInitial;
    }
    if (o.connect_delay_time_final <= 0) {
        o.connect_delay_time_final = kDefaultConnectDelayTimeFinal;
    }
    if (o.connect_delay_time_step <= 0) {
        o.connect_delay_time_step = kDefaultConnectDelayTimeStep;
    }

    return o;
}

} // anonymouse namespace

#ifdef RES_COUNTER
util::Atomic<uint64_t> Client::Impl::next_id_;
util::Atomic<uint64_t> Client::Impl::free_id_;
#endif

Client::Options::Options()
    : pulse_interval(kDefaultPulseInterval),
      pulse_relaxed_checking(false),
      pulse_lazy_emitting(false),
      resolve_timeout(kDefaultResolveTimeout),
      connect_timeout(kDefaultConnectTimeout),
      connect_delay_count(kDefaultConnectDelayCount),
      connect_delay_time_initial(kDefaultConnectDelayTimeInitial),
      connect_delay_time_final(kDefaultConnectDelayTimeFinal),
      connect_delay_time_step(kDefaultConnectDelayTimeStep),
      notify_connected(NULL),
      notify_connecting(NULL),
      notify_closed(NULL),
      socket_read_buffer_size(kDefaultSocketReadBufferSize)
{
}

Client::Impl::Impl(Client* client, const Client::Options& options)
    : client_(client),
      options_(SanitizeOptions(options))
{
}

boost::shared_ptr<Channel> Client::Impl::Create(EventLoop* event_loop,
        const std::string& host, uint16_t port) {
    boost::shared_ptr<Channel> channel =
        boost::make_shared<Channel>(event_loop, this, host, port);
    channel->impl_->Open();
    return channel;
}

boost::shared_ptr<Channel> Client::Impl::Create(EventLoop* event_loop,
        const Endpoint& remote) {
    boost::shared_ptr<Channel> channel =
        boost::make_shared<Channel>(event_loop, this, remote);
    channel->impl_->Open();
    return channel;
}

} // namespace net
} // namespace ldd
