#include <glog/logging.h>
#include <boost/bind.hpp>
#include "ldd/net/channel.h"
#include "ldd/net/outgoing_msg.h"
#include "ldd/net/incoming_msg.h"
#include "internal/channel_impl.h"

namespace ldd {
namespace net {

Channel::Channel(EventLoop* event_loop, Server::Impl* server)
    : impl_(new IncomingChannel(this, event_loop, server))
{
}

Channel::Channel(EventLoop* event_loop, Client::Impl* client,
        const std::string& host, uint16_t port)
    : impl_(new OutgoingChannel(this, event_loop, client, host, port))
{
}

Channel::Channel(EventLoop* event_loop, Client::Impl* client,
        const Endpoint& remote)
    : impl_(new OutgoingChannel(this, event_loop, client, remote))
{
}

Channel::~Channel() {
    delete impl_;
}

EventLoop *Channel::event_loop() const {
    return impl_->event_loop();
}

Channel::State Channel::state() const {
    return impl_->state();
}

Channel::Type Channel::type() const {
    return impl_->type();
}

std::string Channel::name() const {
    return impl_->name();
}

const Endpoint& Channel::peer_endpoint() const {
    return impl_->peer_endpoint();
}

const Endpoint& Channel::self_endpoint() const {
    return impl_->self_endpoint();
}

bool Channel::Post(const boost::shared_ptr<OutgoingMsg>& omsg,
        const ldd::util::TimeDiff& timeout, bool force) {
    CHECK_LT(omsg->id(), 0);
    CHECK(!omsg->channel());
    if (state() == kClosed || (state() == kConnecting && !force)) {
        return false;
    }
    event_loop()->QueueInLoop(
            boost::bind(&Channel::DoPost, shared_from_this(),
                omsg, timeout));
    return true;
}

void Channel::DoPost(const boost::shared_ptr<OutgoingMsg>& message,
        const ldd::util::TimeDiff& timeout) {
    impl_->Post(message, timeout);
}

void Channel::Close() {
    event_loop()->QueueInLoop(
            boost::bind(&Channel::DoClose, shared_from_this()));
}

void Channel::DoClose() {
    impl_->Close();
}

void Channel::set_context(const boost::any& context) {
    impl_->set_context(context);
}
int Channel::socket()
{
    return impl_->socket();
}


const boost::any& Channel::context() const {
    return impl_->context();
}

} // namespace net
} // namespace ldd

