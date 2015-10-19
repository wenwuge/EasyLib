#include "easyevent.h"
//timer event
#include <string.h>

FdEvent::FdEvent(struct event_base* base)
    : base_(base)
      , ev_(NULL)
      , time_flag_(kNoTime)
      , flags_(0)
      , active_(false)
{
}

FdEvent::~FdEvent() {

    Cancel();
}

void FdEvent::Start(int fd, int events) {


    if ((events & (kReadable | kWritable)) == 0 ) {
        return;
    }

    Cancel();

    if (fd >= 0) {
        short e = 0;
        if (events & kReadable) {
            e |= EV_READ;
        }

        if (events & kWritable) {
            e |= EV_WRITE;
        }

        ev_ = event_new(base_
                , fd
                , e /*no EV_PERSIST, because FdEvent is a once event*/
                , Notify
                , (void *)this);


        if (time_flag_ == kNoTime) {
            int rc = event_add(ev_, NULL);
            if (rc != 0) {
                return;
            }
        }
#if 0 
        else {
            struct timeval tv;
            if (time_flag_ == kTimeDiff) {
                time_diff_.To(&tv);
                    << time_diff_.ToMilliseconds()
                    << ", fd: " << fd;
            }
            else if (time_flag_ == FdEvent::kTimestamp) {
                ldd::util::TimeDiff td
                    = time_stamp_ - ldd::util::Timestamp::Now();;
                    << td.ToMilliseconds()
                    << ", fd: " << fd;

                td.To(&tv);
            }
            else {
                return;
            }

            int rc = event_add(ev_, &tv);
            if (rc != 0) {
                return;
            }
        }

        active_ = true;

        return;
    }
#endif
    else {
        return;
    }
}
}

void FdEvent::Cancel() {


    if (!active_) {
        return;
    }

    active_ = false;

    if (ev_ != NULL) {
        int rc = event_del(ev_);
        if (rc != 0) {
            return;
        }
        event_free(ev_);
        ev_ = NULL;
    }
    else {
    }

    handler_ = NULL;
    time_flag_ = kNoTime;
}

void FdEvent::Notify(int fd, short what, void *arg) {



    FdEvent *self = static_cast<FdEvent *>(arg);

    if (what & EV_READ) {
        self->flags_ |= kReadable;
    }
    else if (what & EV_WRITE) {
        self->flags_ |= kWritable;
    }
    else if (what & EV_TIMEOUT) {
    }
    else {
        return;
    }


    try {

        if (self->active_) {
            self->handler_(self->flags_);
        }
        else {
        }

    } catch (const std::exception& e) {
    } catch (...) {
    }

}

void FdEvent::AsyncWait(int fd, int events, const Functor& handler) {

    if (active_) {
        Cancel();
    }

    time_flag_ = kNoTime;
    handler_ = handler;

    Start(fd, events);
}
#if 0
void FdEvent::AsyncWait(int fd, int events, const Functor& handler,
        const ldd::util::Timestamp& timeout) {

    if (active_) {
        Cancel();
    }

    time_stamp_ = timeout;
    time_flag_ = kTimestamp;
    handler_ = handler;

    Start(fd, events);
}

void FdEvent::AsyncWait(int fd, int events, const Functor& handler,
        const ldd::util::TimeDiff& timeout) {


    if (active_) {
        Cancel();
    }

    time_diff_ = timeout;
    time_flag_ = kTimeDiff;
    handler_ = handler;

    Start(fd, events);
}
#endif

TimerEvent::TimerEvent(struct event_base* base)
    : base_(base)
      , timer_ev_(NULL)
      , active_(false)
{
    
}

TimerEvent::~TimerEvent() {
    Cancel();
}

void TimerEvent::AsyncWait(const Functor& handler,
        const struct timeval& timeout) {

    Cancel();
    memcpy(&timeout_, &timeout, sizeof(struct timeval));
    handler_ = handler;

    Start(); 
}

void TimerEvent::Start() {


    if (active_) {
        return;
    }

    timer_ev_ = evtimer_new(base_
            , OnTimer
            , (void *)this);

    evtimer_add(timer_ev_, &timeout_);

    active_ = true;
}

void TimerEvent::Cancel() {


    if (!active_) {
        return;
    }

    active_ = false;

    if (timer_ev_ != NULL) {
        int rc = evtimer_del(timer_ev_);
        if (rc != 0) {
            return;
        }
        event_free(timer_ev_);
        timer_ev_ = NULL;
    }

    handler_ = NULL;
}

void TimerEvent::OnTimer(int fd, short what, void *arg) {

    TimerEvent *self = static_cast<TimerEvent *>(arg);

    if (!(what & EV_TIMEOUT)) {
        return;
    }

    self->active_ = false;
    if (self->timer_ev_ != NULL) {
        int rc = evtimer_del(self->timer_ev_);
        if (rc != 0) {
            return;
        }
        event_free(self->timer_ev_);
        self->timer_ev_ = NULL;
    }

    self->handler_();

    return;
}

//signal event 
SignalEvent::SignalEvent(event_base* base)
{
    base_ = base;
}

SignalEvent::~SignalEvent()
{
    Clear();
}

bool SignalEvent::Add(int signo) {

    std::map<int, struct event *>::iterator pos = sig_ev_.find(signo);
    if (pos != sig_ev_.end()) {
        return false;
    }

    struct event *sig_ev = evsignal_new(base_
            , signo
            , OnSignal
            , (void *)this);


    std::pair<std::map<int, struct event *>::iterator, bool> ret;
    ret = sig_ev_.insert(std::make_pair(signo, sig_ev));
    if (ret.second == false) {
        return false;
    }

    return true;
}

bool SignalEvent::Remove(int signo) {

    std::map<int, struct event *>::iterator pos = sig_ev_.find(signo);
    if (pos == sig_ev_.end()) {
        return false;
    }

    evsignal_del(pos->second);
    event_free(pos->second);

    sig_ev_.erase(pos);

    return true;
}

bool SignalEvent::Clear() {
#if 0
    if (timer_ev_ != NULL) {
        int rc = evtimer_del(timer_ev_);
        if (rc != 0) {
            return false;
        }
        event_free(timer_ev_);
        timer_ev_ = NULL;
    }
#endif
    std::map<int, struct event *>::iterator it;
    for (it = sig_ev_.begin(); it != sig_ev_.end(); it++) {
        evsignal_del(it->second);
        event_free(it->second);
    }

    sig_ev_.clear();

    handler_ = NULL;

    return true;
}

void SignalEvent::AsyncWait(const Functor& handler)
{
    handler_ = handler;

    std::map<int, struct event *>::iterator it;
    for (it = sig_ev_.begin(); it != sig_ev_.end(); it++) {
        int rc = evsignal_add(it->second, NULL);
        if (rc != 0) {
            return;
        }
    }
}

void SignalEvent::Cancel()
{
    Clear();
}

void SignalEvent::OnSignal(int fd, short what, void *arg)
{

    int sig = fd;
    SignalEvent *self = static_cast<SignalEvent*>(arg);

    std::map<int, struct event *>::iterator pos = self->sig_ev_.find(sig);
    if (pos == self->sig_ev_.end()) {
        return;
    }

    self->handler_(sig);
}

