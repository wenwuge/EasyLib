// event.h (2013-08-08)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_EVENT_H_
#define LDD_NET_EVENT_H_

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <ldd/util/timestamp.h>
#include <ldd/util/time_diff.h>

namespace ldd {
namespace net {

class EventLoop;

class FdEvent : boost::noncopyable {
public:
    class Impl;
    enum Type {
        kReadable = 1,
        kWritable = 2,
    };
    typedef boost::function<void(int)> Functor;
    FdEvent(EventLoop* loop);
    ~FdEvent();
    void AsyncWait(int fd, int events, const Functor& handler);
    void AsyncWait(int fd, int events, const Functor& handler,
            const ldd::util::Timestamp& timeout);
    void AsyncWait(int fd, int events, const Functor& handler,
            const ldd::util::TimeDiff& timeout);
    void Cancel();
protected:
    Impl* impl_;
};

class TimerEvent : boost::noncopyable {
public:
    class Impl;
    typedef boost::function<void()> Functor;
    TimerEvent(EventLoop* loop);
    ~TimerEvent();

    bool ExpiresAt(ldd::util::Timestamp* time);
    void AsyncWait(const Functor& handler,
            const ldd::util::Timestamp& timeout);
    void AsyncWait(const Functor& handler,
            const ldd::util::TimeDiff& timeout);
    void Cancel();
private:
    Impl* impl_;
};

class SignalEvent : boost::noncopyable {
public:
    class Impl;
    typedef boost::function<void(int)> Functor;
    SignalEvent(EventLoop* loop);
    ~SignalEvent();

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
    Impl* impl_;
};

} // namespace net
} // namespace ldd


#endif // LDD_NET_EVENT_H_
