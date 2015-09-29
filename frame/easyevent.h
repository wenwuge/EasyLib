#ifndef __EVENTLOOP
#define __EVENTLOOP
#include <event.h>
#include <map>
#include "../common/noncopyable.h"
#include <tr1/functional>

using namespace std;

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
