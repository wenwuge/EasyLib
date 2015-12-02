// event_loop.cc (2013-08-08)
// Li Xinjie (xjason.li@gmail.com)

#include <boost/bind.hpp>
#include <glog/logging.h>
#include "event_loop.h"
#include "internal/event_loop_impl.h"

namespace ldd {
namespace net {

EventLoop::EventLoop()
    : impl_(new Impl)
{
}

EventLoop::EventLoop(struct ::event_base *base)
    : impl_(new Impl(base))
{
}

EventLoop::~EventLoop() {
    delete impl_;
}

void EventLoop::Run() {
    impl_->Run();
}

void EventLoop::Stop() {
    impl_->Stop();
}

void EventLoop::AfterFork() {
    impl_->AfterFork();
}

void EventLoop::RunInLoop(const Functor& handler) {
    impl_->RunInLoop(handler);
}

void EventLoop::QueueInLoop(const Functor& handler) {
    impl_->QueueInLoop(handler);
}

struct event_base *EventLoop::event_base()
{
    return impl_->event_base();
}

EventLoopThread::EventLoopThread()
    : event_loop_(new EventLoop),
      thread_(NULL),
      state_(kStopped)
{
}

EventLoopThread::~EventLoopThread() {
    Stop();
    Join();
}

bool EventLoopThread::Start(const Functor& pre, const Functor& post) {
    State oldstate = kStopped;
    CHECK(state_.CompareExchangeStrong(&oldstate, kStarting))
        << "Thread is already started.";
    CHECK(!thread_);

    thread_.reset(new util::Thread(
                boost::bind(&EventLoopThread::Run, this, pre, post)));
    bool r = thread_->Start();
    if (!r) {
        state_ = kStopped;
        thread_.reset();
    } else {
        state_ = kRunning;
    }
    return r;
}

void EventLoopThread::Run(const Functor& pre, const Functor& post) {
    if (pre) {
        pre();
    }
    event_loop_->Run();
    if (post) {
        post();
    }
}

void EventLoopThread::Stop() {
    State oldstate = kRunning;
    if (state_.CompareExchangeStrong(&oldstate, kStopping)) {
        event_loop_->Stop();
    }
}

void EventLoopThread::Join() {
    State oldstate = kStopping;
    if (state_.CompareExchangeStrong(&oldstate, kJoining)) {
        thread_->Join();
        thread_.reset();
        state_ = kStopped;
    }
}



} // namespace net
} // namespace ldd

