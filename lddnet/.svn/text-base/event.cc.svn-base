// event.cc (2013-08-08)
// Li Xinjie (xjason.li@gmail.com)

#include "event.h"
#include "event_loop.h"
#include "internal/event_impl.h"

namespace ldd {
namespace net {

FdEvent::FdEvent(EventLoop* loop)
    : impl_(new Impl(loop->impl_))
{}

FdEvent::~FdEvent() {
    delete impl_;
}

void FdEvent::AsyncWait(int fd, int events, const Functor& handler) {
    impl_->AsyncWait(fd, events, handler);
}

void FdEvent::AsyncWait(int fd, int events, const Functor& handler,
        const ldd::util::Timestamp& timeout) {
    impl_->AsyncWait(fd, events, handler, timeout);
}

void FdEvent::AsyncWait(int fd, int events, const Functor& handler,
        const ldd::util::TimeDiff& timeout) {
    impl_->AsyncWait(fd, events, handler, timeout);
}

void FdEvent::Cancel() {
    impl_->Cancel();
}

SignalEvent::SignalEvent(EventLoop* loop)
    : impl_(new Impl(loop->impl_)) {}

SignalEvent::~SignalEvent() {
    delete impl_;
}

bool SignalEvent::Add(int signo) {
    return impl_->Add(signo);
}

bool SignalEvent::Remove(int signo) {
    return impl_->Remove(signo);
}

bool SignalEvent::Clear() {
    return impl_->Clear();
}

void SignalEvent::AsyncWait(const Functor& handler) {
    impl_->AsyncWait(handler);
}

void SignalEvent::AsyncWait(const Functor& handler,
        const ldd::util::Timestamp& timeout) {
    impl_->AsyncWait(handler, timeout);
}

void SignalEvent::AsyncWait(const Functor& handler,
        const ldd::util::TimeDiff& timeout) {
    impl_->AsyncWait(handler, timeout);
}

void SignalEvent::Cancel() {
    impl_->Cancel();
}

TimerEvent::TimerEvent(EventLoop* loop)
    : impl_(new Impl(loop->impl_)) {}

TimerEvent::~TimerEvent() {
    delete impl_;
}

bool TimerEvent::ExpiresAt(ldd::util::Timestamp* time) {
    return impl_->ExpiresAt(time);
}

void TimerEvent::AsyncWait(const Functor& handler,
        const ldd::util::Timestamp& timeout) {
    impl_->AsyncWait(handler, timeout);
}

void TimerEvent::AsyncWait(const Functor& handler,
        const ldd::util::TimeDiff& timeout) {
    impl_->AsyncWait(handler, timeout);
}

void TimerEvent::Cancel() {
    impl_->Cancel();
}

} // namespace net
} // namespace ldd
