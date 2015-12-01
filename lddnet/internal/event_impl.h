// event_impl.h (2013-08-08)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_INTERNAL_EVENT_IMPL_H_
#define LDD_NET_INTERNAL_EVENT_IMPL_H_

#include <event2/event.h>

#include <set>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "ldd/net/event.h"
#include "event_loop_impl.h"

namespace ldd {
namespace net {

class FdEvent::Impl {
    static const int kTimedOut = 4;
public:
    Impl(EventLoop::Impl* loop);
    ~Impl();

    enum {
        kNoTime = 0,
        kTimeDiff,
        kTimestamp
    };

    void AsyncWait(int fd, int events, const Functor& handler);
    void AsyncWait(int fd, int events, const Functor& handler,
            const ldd::util::Timestamp& timeout);
    void AsyncWait(int fd, int events, const Functor& handler,
            const ldd::util::TimeDiff& timeout);
    void Cancel();

protected:
    void Start(int fd, int events);
    static void Notify(int fd, short what, void *arg);

    EventLoop::Impl* loop_;

    struct event *ev_; 

    ldd::util::TimeDiff time_diff_;
    ldd::util::Timestamp time_stamp_; 

    int time_flag_;

    Functor handler_;
    int8_t flags_;
    bool active_;

#ifdef RES_COUNTER
private:
    friend class Stat;
    static util::Atomic<uint64_t> next_id_;
    static util::Atomic<uint64_t> free_id_;
    static const uint64_t next_id() { return next_id_; }
    static const uint64_t free_id() { return free_id_; }
#endif
};

class TimerEvent::Impl {
public:
    Impl(EventLoop::Impl* loop);
    ~Impl();

    enum {
        kNoTime = 0,
        kTimeDiff,
        kTimestamp
    };

    bool ExpiresAt(ldd::util::Timestamp* time);
    void AsyncWait(const Functor& handler,
            const ldd::util::Timestamp& timeout);
    void AsyncWait(const Functor& handler,
            const ldd::util::TimeDiff& timeout);
    void Cancel();

private:
    void Start();
    static void OnTimer(int fd, short what, void *arg);

    EventLoop::Impl* loop_;

    struct event *timer_ev_;
    ldd::util::TimeDiff time_diff_;
    ldd::util::Timestamp time_stamp_;
    int time_flag_;

    ldd::util::Timestamp start_stamp_;

    Functor handler_;
    bool active_;

#ifdef RES_COUNTER
private:
    friend class Stat;
    static util::Atomic<uint64_t> next_id_;
    static util::Atomic<uint64_t> free_id_;
    static const uint64_t next_id() { return next_id_; }
    static const uint64_t free_id() { return free_id_; }
#endif
};

class SignalEvent::Impl {
public:
    Impl(EventLoop::Impl* loop);
    ~Impl();

    bool Add(int signo);
    bool Remove(int signo);
    bool Clear();

    void AsyncWait(const Functor& handler);
    void AsyncWait(const Functor& handler,
            const ldd::util::Timestamp& timeout);
    void AsyncWait(const Functor& handler,
            const ldd::util::TimeDiff& timeout);
    void Cancel();

private:
    static void OnSignal(int fd, short what, void *arg);
    static void OnTimer(int fd, short what, void *arg);

    EventLoop::Impl* loop_;
    struct event *timer_ev_;
    std::map<int, struct event *> sig_ev_;

    Functor handler_;

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

#endif // LDD_NET_INTERNAL_EVENT_IMPL_H_

