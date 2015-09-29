#include "easyevent.h"
//timer event
#include <string.h>

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

