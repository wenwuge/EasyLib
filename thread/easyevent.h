#ifndef __EASYEVENT
#define __EASYEVENT
#if 0
#include <event.h>
#else
#include <event2/event.h>
#endif
#include <map>
#include "../common/noncopyable.h"
#include <tr1/functional>

using namespace std;

class FdEvent : noncopyable {
public:
    enum Type {
        kReadable = 1,
        kWritable = 2,
    };

    enum {
        kNoTime = 0,
        kTimeDiff = 1,
        kTimeStamp = 2,
    };
    typedef std::tr1::function<void(int)> Functor;
    FdEvent(struct event_base* base);
    ~FdEvent();
    void AsyncWait(int fd, int events, const Functor& handler);
#if 0
    void AsyncWait(int fd, int events, const Functor& handler,
            const ldd::util::Timestamp& timeout);
    void AsyncWait(int fd, int events, const Functor& handler,
            const ldd::util::TimeDiff& timeout);
#endif
    void Cancel();
private:
    void Start(int fd, int events);
    static void Notify(int fd, short what, void *arg); 
protected:
    struct event_base  *base_;
    struct event *ev_;
#if 0
    ldd::util::TimeDiff time_diff_;
    ldd::util::Timestamp time_stamp_;
#endif
    Functor handler_;
    int8_t flags_;
    bool active_;
    int8_t time_flag_;
};

class TimerEvent : noncopyable {
public:
    typedef std::tr1::function<void()> Functor;
    TimerEvent(struct event_base* base_);
    ~TimerEvent();
    void AsyncWait(const Functor& handler,
            const struct timeval& timeout);
    void Cancel();
private:
    void Start();
    static void OnTimer(int fd, short what, void *arg); 
private:
    struct event_base *base_;
    Functor handler_;
    struct timeval timeout_;
    struct event * timer_ev_;
    bool active_;

};

class SignalEvent : noncopyable {
public:
    typedef std::tr1::function<void(int)> Functor;
    SignalEvent(struct event_base* base);
    ~SignalEvent();

    bool Add(int signo);
    bool Remove(int signo);
    bool Clear();

    void AsyncWait(const Functor& handler);
#if 0
    void AsyncWait(const Functor& handler,
            const ldd::util::Timestamp& timeout);
    void AsyncWait(const Functor& handler,
            const ldd::util::TimeDiff& timeout);
#endif
    void Cancel();
    static void OnSignal(int fd, short what, void *arg);
private:
    struct event_base * base_;
    map<int, struct event*> sig_ev_;
    Functor handler_; 
};


#endif
