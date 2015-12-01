// event_loop_impl.h (2013-08-08)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_INTERNAL_EVENT_LOOP_IMPL_H_
#define LDD_NET_INTERNAL_EVENT_LOOP_IMPL_H_

#include <pthread.h>
#include <event2/event.h>
#include "ldd/net/event_loop.h"

namespace ldd {
namespace net {

class EventLoop::Impl {
public:
    Impl();
    explicit Impl(struct ::event_base *base);
    ~Impl();

    void Run();
    void Stop();
    void AfterFork();
    void RunInLoop(const Functor& handler);
    void QueueInLoop(const Functor& handler);

    struct event_base *event_base() { return event_base_; }

private:
    void Post(const Functor& functor);
    static void PipeNotify(int fd, short what, void *arg);
    void RealStop();
    void Init(void);

private:
    struct event_base *event_base_;

    int fd_[2];
    int &read_fd_;
    int &write_fd_;
    struct event *read_ev_;

    pthread_t tid_;

#ifdef RES_COUNTER
private:
    friend class Stat;
    static util::Atomic<uint64_t> next_id_;
    static util::Atomic<uint64_t> free_id_;
    static const uint64_t next_id() { return next_id_; }
    static const uint64_t free_id() { return free_id_; }
#endif
};

} // namespace net
} // namespace ldd

#endif // LDD_NET_INTERNAL_EVENT_LOOP_IMPL_H_

